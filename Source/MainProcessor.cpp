#include "MainProcessor.h"
#include "LufsEngine.h"

MainProcessor::MainProcessor(SettingsRegistry& _settings_reg, uint32_t _num_channels) :
    settings_reg(_settings_reg), 
    num_channels(_num_channels) {

    juce::ScopedNoDenormals noDenormals;

    settings_reg.add_main_proc_settings_listener(this);

    main_proc_ui = std::make_unique<MainProcessorUI>(settings_reg);

    level_meter_in = std::make_unique<fmsmoov::LevelMeter>();
    main_proc_ui->add_level_meter_in(level_meter_in.get());
    
    hpf_30Hz = std::make_unique <fmsmoov::HPF30Hz>();
    phase_rotator = std::make_unique <fmsmoov::PhaseRotator>();
    bass_centering = std::make_unique<fmsmoov::BassCentering>();

    loudness_meter_in = std::make_unique <fmsmoov::LoudnessMeter>();
    main_proc_ui->add_loudness_meter_in(loudness_meter_in.get());

    crossover_two_band = std::make_unique <fmsmoov::CrossoverTwoBand>();

    fmsmoov::LufsDataSource& lufs_src = loudness_meter_in->get_lufs_source();

    two_band_slow_agc = std::make_unique<fmsmoov::TwoBandSlowAGC>(lufs_src, *crossover_two_band);
    main_proc_ui->add_two_band_slow_agc(two_band_slow_agc.get());

    level_meter_out = std::make_unique<fmsmoov::LevelMeter>();
    main_proc_ui->add_level_meter_out(level_meter_out.get());

    addAndMakeVisible(main_proc_ui.get());

    current_block_size = 0;
    current_sample_rate = 0;
}

MainProcessor::~MainProcessor() {
    settings_reg.remove_main_proc_settings_listener(this);
    main_proc_ui.reset();
    level_meter_in.reset();
    hpf_30Hz.reset();
    phase_rotator.reset();
    bass_centering.reset();

    loudness_meter_in.reset();
    crossover_two_band.reset();
    two_band_slow_agc.reset();
    level_meter_out.reset();
}

void MainProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    current_block_size = samplesPerBlockExpected;
    current_sample_rate = sampleRate;

    juce::dsp::ProcessSpec spec{
        sampleRate,
        samplesPerBlockExpected,
        2 };

    level_meter_in->prepare(spec);
    hpf_30Hz->prepare(spec);
    phase_rotator->prepare(spec);
    bass_centering->prepare(spec);

    loudness_meter_in->prepare(spec);
    crossover_two_band->prepare(spec);
    two_band_slow_agc->prepare(spec);
    level_meter_out->prepare(spec);
}

void MainProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
//    juce::dsp::AudioBlock<float> block (*bufferToFill.buffer, (size_t)bufferToFill.startSample);

//    auto sub_block = block.getSubBlock(0, (size_t)bufferToFill.numSamples);
//    juce::dsp::ProcessContextReplacing<float> context(sub_block);

    juce::dsp::AudioBlock<float> block(*bufferToFill.buffer);
    auto sub_block = block.getSubBlock((size_t)bufferToFill.startSample, (size_t)bufferToFill.numSamples);

    juce::dsp::ProcessContextReplacing<float> context(sub_block);

    level_meter_in->update(sub_block);
    hpf_30Hz->process(context);
    phase_rotator->process(context);
    bass_centering->process(context);

    loudness_meter_in->update(sub_block);

    /* 
     * The crossover should just return the context unchanged, but it will have split the input
     * into two audio buffers representing the low and high bands.
     */

    crossover_two_band->process(context);

    /*
     * The AGC will do its magic from the two crossover buffers and then modify the context with 
     * a collapsed buffer matrix into a stereo output.
     */
  
    two_band_slow_agc->process(context);

    level_meter_out->update(sub_block);
}

void MainProcessor::releaseResources() {

}

void MainProcessor::main_bypass_changed(bool master_bypass) {

}

void MainProcessor::paint(juce::Graphics& g) {
    //g.fillAll(juce::Colours::red);
}

void MainProcessor::resized() {
    main_proc_ui->setBounds(getLocalBounds());
}

