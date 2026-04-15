#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"
#include "MainProcessorUI.h"
#include "LevelMeter.h"
#include "HPF30Hz.h"
#include "PhaseRotator.h"
#include "BassCentering.h"
#include "LoudnessMeter.h"
#include "CrossoverTwoBand.h"
#include "TwoBandSlowAGC.h"

class MainProcessor : public juce::Component, public juce::AudioSource, public SettingsRegistry::MainProcSettingsListener
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

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    int current_block_size;
    float current_sample_rate;
    uint32_t num_channels;
    SettingsRegistry& settings_reg;

    std::unique_ptr<MainProcessorUI> main_proc_ui;
    std::unique_ptr<fmsmoov::LevelMeter> level_meter_in;
    std::unique_ptr<fmsmoov::HPF30Hz> hpf_30Hz;
    std::unique_ptr<fmsmoov::PhaseRotator> phase_rotator;
    std::unique_ptr<fmsmoov::BassCentering> bass_centering;
    std::unique_ptr<fmsmoov::LoudnessMeter> loudness_meter_in;
    std::unique_ptr<fmsmoov::CrossoverTwoBand> crossover_two_band;
    std::unique_ptr<fmsmoov::TwoBandSlowAGC> two_band_slow_agc;
    std::unique_ptr<fmsmoov::LevelMeter> level_meter_out;

    std::atomic<bool> bypass_hpf_30Hz{ false };
    std::atomic<bool> bypass_phase_rotator{ false };
    std::atomic<bool> bypass_bass_centering{ false };
    std::atomic<bool> bypass_two_band_slow_agc{ false };
};

