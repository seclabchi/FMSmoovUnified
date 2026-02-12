#include "MainProcessor.h"

MainProcessor::MainProcessor(SettingsRegistry& _settings_reg, uint32_t _num_channels) :
    settings_reg(_settings_reg), 
    num_channels(_num_channels) {


    settings_reg.state.addListener(this);

    current_block_size = 0;
    current_sample_rate = 0;
}

MainProcessor::~MainProcessor() {
}

void MainProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    current_block_size = samplesPerBlockExpected;
    current_sample_rate = sampleRate;
}

void MainProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    
}

void MainProcessor::releaseResources() {

}

void MainProcessor::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) {
    
}

