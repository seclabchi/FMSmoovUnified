#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"

class MainProcessor : public juce::AudioSource, public juce::ValueTree::Listener
{
public:
    MainProcessor(SettingsRegistry& _settings_reg, uint32_t _num_channels);
    virtual ~MainProcessor();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;


private:
    int current_block_size;
    float current_sample_rate;
    uint32_t num_channels;
    SettingsRegistry& settings_reg;

};

