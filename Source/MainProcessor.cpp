#include "MainProcessor.h"

MainProcessor::MainProcessor(SettingsRegistry& _settings_reg, uint32_t _num_channels) :
    settings_reg(_settings_reg), 
    num_channels(_num_channels) {


    settings_reg.add_main_proc_settings_listener(this);

    main_proc_ui = std::make_unique<MainProcessorUI>(settings_reg);

    level_meter_in = std::make_unique<fmsmoov::LevelMeter>();
    main_proc_ui->add_level_meter_in(level_meter_in.get());

    addAndMakeVisible(main_proc_ui.get());

    current_block_size = 0;
    current_sample_rate = 0;
}

MainProcessor::~MainProcessor() {
    settings_reg.remove_main_proc_settings_listener(this);
    main_proc_ui.reset();
    level_meter_in.reset();
}

void MainProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    current_block_size = samplesPerBlockExpected;
    current_sample_rate = sampleRate;

    juce::dsp::ProcessSpec spec{
        sampleRate,
        samplesPerBlockExpected,
        2 };

    level_meter_in->prepare(spec);

}

void MainProcessor::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    juce::dsp::AudioBlock<float> block (*bufferToFill.buffer, (size_t)bufferToFill.startSample);

    auto sub_block = block.getSubBlock(0, (size_t)bufferToFill.numSamples);
    //juce::dsp::ProcessContextReplacing<float> context(sub_block);

    level_meter_in->update(sub_block);
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

