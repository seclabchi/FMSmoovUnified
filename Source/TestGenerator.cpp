#include "TestGenerator.h"

TestGenerator::TestGenerator(SettingsRegistry& _settings_reg, uint32_t _num_channels) : 
    settings_reg(_settings_reg), 
    num_channels(_num_channels) {
    
    settings_reg.add_test_gen_settings_listener(this);

    tone_gen_01 = std::make_unique<fmsmoov::OscillatorSource>(settings_reg, juce::String("gen01"), num_channels);
    tone_gen_02 = std::make_unique<fmsmoov::OscillatorSource>(settings_reg, juce::String("gen02"), num_channels);
    tone_gen_03 = std::make_unique<fmsmoov::OscillatorSource>(settings_reg, juce::String("gen03"), num_channels);

    mixer = std::make_unique<juce::MixerAudioSource>();
    current_block_size = 0;
    current_sample_rate = 0;
}

TestGenerator::~TestGenerator() {
    settings_reg.remove_test_gen_settings_listener(this);
    mixer->removeAllInputs();
}

void TestGenerator::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    mixer->removeAllInputs();

    if (settings_reg.get_gen01_enable()) {
        add_source_if_needed(tone_gen_01.get());
    }

    if (settings_reg.get_gen02_enable()) {
        add_source_if_needed(tone_gen_02.get());

    }

    if (settings_reg.get_gen03_enable()) {
        add_source_if_needed(tone_gen_03.get());
    }
    
    mixer->prepareToPlay(samplesPerBlockExpected, sampleRate);

    current_block_size = samplesPerBlockExpected;
    current_sample_rate = sampleRate;
}

void TestGenerator::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {

    if (should_update_mixer.load()) {
        update_mixer();
    }

    /* 
     * SIDE EFFECT NOTE:  This TestGenerator MUST NOT be the first source in the upstream mixer.
     * The following call to clearActiveBufferingRegion will clear the buffer passed in.  If this
     * is the first source in the main mixer chain, then the input samples will be cleared.  For
     * Some reason, AudioSourceMixer uses "copyFrom" on the first buffer, and then addFrom in the
     * subsequent buffers into its own internal temp buffer.
     */

    bufferToFill.clearActiveBufferRegion();
    
    /* You must have a valid audio buffer from the parent, or this will just output garbage
     * which probably has the side effect of muting the output.
     */
    if (settings_reg.get_generator_state()) {
        mixer->getNextAudioBlock(bufferToFill);
    }
}

void TestGenerator::releaseResources() {
    tone_gen_01->releaseResources();
    tone_gen_02->releaseResources();
    tone_gen_03->releaseResources();
    mixer->releaseResources();
}

void TestGenerator::gen_params_changed(const juce::String& gen_num) {

    fmsmoov::GEN_TYPE new_type;
    float new_freq;
    float new_ampl;
    bool new_enable;
    
    settings_reg.get_gen_params(gen_num, new_type, new_freq, new_ampl, new_enable);

    if (gen_num == "gen01") {
        if (new_enable != sources.contains(tone_gen_01.get())) {
            if (new_enable) {
                add_source_if_needed(tone_gen_01.get());
            }
            else {
                remove_source_if_present(tone_gen_01.get());
            }
            should_update_mixer.store(true);
        }
    }
    else if (gen_num == "gen02") {
        if (new_enable != sources.contains(tone_gen_02.get())) {
            if (new_enable) {
                add_source_if_needed(tone_gen_02.get());
            }
            else {
                remove_source_if_present(tone_gen_02.get());
            }
            should_update_mixer.store(true);
        }
    }
    else if (gen_num == "gen03") {
        if (new_enable != sources.contains(tone_gen_03.get())) {
            if (new_enable) {
                add_source_if_needed(tone_gen_03.get());
            }
            else {
                remove_source_if_present(tone_gen_03.get());
            }
            should_update_mixer.store(true);
        }
    }
    else if (gen_num == "gener") {
        //TODO:  hokey as fuck, this accounts for the other params that start with "generator"
        //don't do anything
    }
    else {
        //invalid gen num
        jassert(false);
    }
}

void TestGenerator::update_mixer() {
    DBG("TestGenerator::update_mixer");

    if (settings_reg.get_gen01_enable()) {
        add_source_if_needed(tone_gen_01.get());
    }
    else {
        remove_source_if_present(tone_gen_01.get());
    }

    if (settings_reg.get_gen02_enable()) {
        add_source_if_needed(tone_gen_02.get());
    }
    else {
        remove_source_if_present(tone_gen_02.get());
    }

    if (settings_reg.get_gen03_enable()) {
        add_source_if_needed(tone_gen_03.get());
    }
    else {
        remove_source_if_present(tone_gen_03.get());
    }

    should_update_mixer.store(false);
}

/*
 *  I spent a ton of time debugging this.  The MixerAudioSource passes in the
 *  audio buffer to the first item in the mixer list.  For the 1->n sources,
 *  it passes it's own temporary buffer.  THIS IS A MAJOR LANDMINE.
 * 
 *  For buffers 1 through n, you must clear the buffer passed to the source
 *  from the mixer, i.e. for the first buffer, the buffer is cleared in the parent
 *  of the mixer.  For every subsequent buffer, the source itself needs to clear
 *  the buffer passed to it.  So when you are adding and removing sources, you
 *  need to manage the source behavior: does it clear the buffer handed to it before
 *  calling addFrom? 
 * 
 *  Buffer 0:  buffer is cleared in the mixer parent
 *  Buffer 1-n:  clear the buffer first (this is a temp buffer inside the mixer that is reused)
 * 
 *  So when we're adding or removing sources, we need to tell that source whether or not
 *  to clear the buffer in its getNextAudioBlock function.  You don't want to clear for the 0th
 *  buffer because that will destroy whatever was upstream of the mixer.
 * 
 *  UPDATE:  this is no longer necessary since the test generator is now just part of the main
 *  audio chain, so this component should ALWAYS clear the first buffer handed to it, since it
 *  will get add-mixed properly upstream.
 * 
 */
void TestGenerator::add_source_if_needed(fmsmoov::OscillatorSource* src, bool mixer_is_running) {
    if (false == sources.contains(src)) {
        DBG("Adding source 0x" << juce::String::toHexString((juce::uint64) src));
        src->prepareToPlay(current_block_size, current_sample_rate);
        sources.add(src);
        mixer->addInputSource(src, false);
    }
}

void TestGenerator::remove_source_if_present(fmsmoov::OscillatorSource* src) {
    if (true == sources.contains(src)) {
        DBG("Removing source 0x" << juce::String::toHexString((juce::uint64)src));
        mixer->removeInputSource(src);
        sources.remove(sources.indexOf(src));
    }
}