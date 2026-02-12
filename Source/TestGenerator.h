#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"
#include "OscillatorSource.h"

class TestGenerator : public juce::AudioSource, public juce::ValueTree::Listener
{
public:
    TestGenerator(SettingsRegistry& _settings_reg, uint32_t _num_channels);
    virtual ~TestGenerator();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;


private:
    std::unique_ptr<fmsmoov::OscillatorSource> tone_gen_01;
    std::unique_ptr< fmsmoov::OscillatorSource> tone_gen_02;
    std::unique_ptr<fmsmoov::OscillatorSource> tone_gen_03;

    std::unique_ptr<juce::MixerAudioSource> mixer;
    std::atomic<bool> should_update_mixer{ false };
    std::atomic<bool> should_update_gens{ false };
    std::atomic<bool> all_gens_mute{ true };
    void update_gens();
    void update_mixer();

    int current_block_size;
    float current_sample_rate;
    juce::Array<fmsmoov::OscillatorSource*> sources;
    void add_source_if_needed(fmsmoov::OscillatorSource* src, bool mixer_is_running = true);
    void remove_source_if_present(fmsmoov::OscillatorSource* src);

    uint32_t num_channels;
    SettingsRegistry& settings_reg;

};

