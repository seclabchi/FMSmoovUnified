#include "OscillatorSource.h"

namespace fmsmoov {

OscillatorSource::OscillatorSource(SettingsRegistry& _settings_reg, const juce::String& _component_name, uint32_t _num_channels) : 
    settings_reg(_settings_reg), 
    component_name(_component_name),
    num_channels(_num_channels) {

    gen_type_propname = _component_name + "_" + "type";
    frequency_propname = _component_name + "_" + "freq";
    amplitude_propname = _component_name + "_" + "ampl";
    enable_propname = _component_name + "_" + "enable";

    osc = std::make_unique<juce::dsp::Oscillator<float>>();
    gain = std::make_unique<juce::dsp::Gain<float>>();

    juce::ValueTree vt = settings_reg.state;

    juce::var gen_type_val = vt.getProperty(gen_type_propname);

    type.store(static_cast<fmsmoov::GEN_TYPE>((int)gen_type_val));
    freq.store((settings_reg.state.getProperty(frequency_propname, 440.0f)));
    ampl.store((settings_reg.state.getProperty(amplitude_propname, -12.0f)));
    
    settings_reg.state.addListener(this);

    DBG("This: " << (intptr_t)this);
    DBG("As AudioSource: " << (intptr_t)static_cast<juce::AudioSource*>(this));
}

OscillatorSource::~OscillatorSource() {
}

void OscillatorSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 2;

    tmp_buf.setSize(2, samplesPerBlockExpected, false, true, false);
    tmp_buf.clear();

    osc->prepare(spec);
    gain->prepare(spec);

    osc->setFrequency(freq.load());
    gain->setGainDecibels(ampl.load());

    set_type(type.load());
    prepare_brown_noise(sampleRate);
    prepare_blue_noise();
    blue_noise_lut_index = random.nextInt(BLUE_NOISE_LUT_SIZE);

}

void OscillatorSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {

    //if (must_clear_buffer) {
    //    bufferToFill.buffer->clear();
    //}

    tmp_buf.clear();

    /*
    for (int i = 0; i < bufferToFill.numSamples; ++i) {
        jassert(std::isfinite(bufferToFill.buffer->getSample(0, bufferToFill.startSample + i)));
        jassert(std::abs(bufferToFill.buffer->getSample(0, bufferToFill.startSample + i)) < 1.0f);
    }
    */

    juce::ScopedNoDenormals noDenormals;

    fmsmoov::GEN_TYPE typetmp = type.load();

    if (typetmp != type_current) {
        set_type(typetmp);
    }

    float freqtmp = freq.load();

    if (osc->getFrequency() != freqtmp) {
        osc->setFrequency(freqtmp);
    }

    float gaintmp = ampl.load();

    if (gain->getGainDecibels() != gaintmp) {
        gain->setGainDecibels(ampl.load());
    }

    if (type == fmsmoov::GEN_TYPE::SINE ||
        type == fmsmoov::GEN_TYPE::SAWTOOTH ||
        type == fmsmoov::GEN_TYPE::TRIANGLE ||
        type == fmsmoov::GEN_TYPE::SQUARE) {

        juce::dsp::AudioBlock<float> block(tmp_buf);
        auto sub_block = block.getSubBlock(0, (size_t)bufferToFill.numSamples);
        juce::dsp::ProcessContextReplacing<float> context(sub_block);

        osc->process(context);
        gain->process(context);
    }
    else {
        auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
        float gain_linear = juce::Decibels::decibelsToGain(ampl.load());

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            float noise_out = gain_linear * ((random.nextFloat() * 2.0f) - 1.0f);

            if (type == fmsmoov::GEN_TYPE::NOISE_PINK) {
                pink_b0 = 0.99886f * pink_b0 + noise_out * 0.0555179f;
                pink_b1 = 0.99332f * pink_b1 + noise_out * 0.0750759f;
                pink_b2 = 0.96900f * pink_b2 + noise_out * 0.1538520f;
                pink_b3 = 0.86650f * pink_b3 + noise_out * 0.3104856f;
                pink_b4 = 0.55000f * pink_b4 + noise_out * 0.5329522f;
                pink_b5 = -0.7616f * pink_b5 - noise_out * 0.0168980f;
                float pink = pink_b0 + pink_b1 + pink_b2 + pink_b3 + pink_b4 + pink_b5 + pink_b6 + noise_out * 0.5362f;
                pink_b6 = noise_out * 0.115926f;
                noise_out = gain_linear * pink * 0.11f; // Corrected scale for +/-1.0 range
            }
            else if (type == fmsmoov::GEN_TYPE::NOISE_BROWN) {
                float brown_samp = brown_b0 * noise_out + brown_b1 * brown_z1 - brown_a1 * brown_z1;
                brown_z1 = brown_samp;

                noise_out = gain_linear * brown_samp * 120.0f;
            }
            else if (type == fmsmoov::GEN_TYPE::NOISE_BLUE) {
                noise_out = gain_linear * blue_noise_lut[blue_noise_lut_index];
            }
            
            leftChannel[sample] = noise_out;
            rightChannel[sample] = noise_out;

            if (++blue_noise_lut_index >= BLUE_NOISE_LUT_SIZE) {
                blue_noise_lut_index = random.nextInt(BLUE_NOISE_LUT_SIZE);
            }
        }
    }

    for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
        bufferToFill.buffer->addFrom(chan,
            bufferToFill.startSample,
            tmp_buf,
            chan,
            0,
            bufferToFill.numSamples);
    }
}

void OscillatorSource::set_type(fmsmoov::GEN_TYPE _type) {

    switch (_type) {
    case fmsmoov::SINE:
        osc->initialise([](float x) { return std::sin(x); }, 4096);
        break;
    case fmsmoov::SAWTOOTH:
        osc->initialise([](float x) { return x / juce::MathConstants<float>::pi; }, 4096);
        break;
    case fmsmoov::TRIANGLE:
        osc->initialise([](float x) {
            return (x < 0.0f) ? (x * (2.0f / juce::MathConstants<float>::pi) + 1.0f)
                : (x * (-2.0f / juce::MathConstants<float>::pi) + 1.0f);
            }, 4096);
        break;
    case fmsmoov::SQUARE:
        osc->initialise([](float x) {return x < 0.0f ? -1.0f : 1.0f; }, 4096);
        break;
    case fmsmoov::NOISE_WHITE:
    case fmsmoov::NOISE_PINK:
    case fmsmoov::NOISE_BROWN:
    case fmsmoov::NOISE_BLUE:
        /* The noise sources do not use the dsp::Oscillator, so do nothing. */
        break;
    default:
        DBG("NOT YET IMPLEMENTED");
        break;
    }

    type_current = _type;
}

void OscillatorSource::releaseResources() {
    //delete tmp_buf;
}

void OscillatorSource::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& ident) {
    if (!ident.toString().compare(gen_type_propname)) {
        type.store(static_cast<fmsmoov::GEN_TYPE>((int)vt[ident]));
    }

    if (!ident.toString().compare(frequency_propname)) {
        freq.store(vt[ident]);
    }

    if (!ident.toString().compare(amplitude_propname)) {
        ampl.store(vt[ident]);
    }
}

void OscillatorSource::set_must_clear_buffer(bool _must_clear_buffer) {
    must_clear_buffer = _must_clear_buffer;
}

void OscillatorSource::prepare_brown_noise(double sample_rate) {
    float fc = 10.0f;
    float theta = 2.0f * juce::MathConstants<float>::pi * fc / sample_rate;
    float gamma = cos(theta) / (1.0f + sin(theta));

    brown_b0 = (1.0f - gamma) / 2.0f;
    brown_b1 = brown_b0;
    brown_a1 = -gamma;
}

/*
 * This gives +6 dB per octave, which is really purple noise, not blue
 * TODO: Fix this and then add purple noise?
 */
void OscillatorSource::prepare_blue_noise() {
    float last_sample = 0.0f;

    for (int i = 0; i < BLUE_NOISE_LUT_SIZE; ++i) {
        float white = (random.nextFloat() * 2.0f) - 1.0f;
        blue_noise_lut[i] = white - last_sample;
        last_sample = white;
    }
}

} //namespace fmsmoov



