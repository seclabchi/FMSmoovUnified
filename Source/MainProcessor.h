#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"

class MainProcessor : public juce::AudioSource, public SettingsRegistry::MainProcSettingsListener
{
public:
    MainProcessor(SettingsRegistry& _settings_reg, uint32_t _num_channels);
    virtual ~MainProcessor();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void main_bypass_changed(bool master_bypass) override;
    void generator_state_changed(bool mute_all_gens) override {};
    void processor_bypass_changed(bool mute_processor) override {};

private:
    int current_block_size;
    float current_sample_rate;
    uint32_t num_channels;
    SettingsRegistry& settings_reg;

};

