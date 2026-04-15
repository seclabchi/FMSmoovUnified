#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"
#include "LightedToggleButtonStyle.h"
#include "LightedButton.h"
#include "LevelMeter.h"
#include "LoudnessMeter.h"
#include "TwoBandSlowAGC.h"

class MainProcessorUI : public juce::Component, public SettingsRegistry::MainProcSettingsListener
{
    public:
        MainProcessorUI(SettingsRegistry& _settings_reg);
        virtual ~MainProcessorUI();
        void paint(juce::Graphics&) override;
        void resized() override;
        void main_bypass_changed(bool master_bypass) override {};
        void generator_state_changed(bool mute_all_gens) override {};
        void processor_bypass_changed(bool mute_processor) override {};

        void add_level_meter_in(fmsmoov::LevelMeter* _level_meter_in);
        void add_loudness_meter_in(fmsmoov::LoudnessMeter* _loudness_meter_in);
        void add_two_band_slow_agc(fmsmoov::TwoBandSlowAGC* _two_band_slow_agc);
        void add_level_meter_out(fmsmoov::LevelMeter* _level_meter_out);
private:
        LightedToggleButtonStyle lighted_toggle_style;
        std::unique_ptr<fmsmoov::LightedButton> button_main_bypass;
        std::unique_ptr <fmsmoov::LightedButton> button_generator_state;
        std::unique_ptr <fmsmoov::LightedButton> button_processor_bypass;

        SettingsRegistry& settings_reg;
        fmsmoov::LevelMeter* level_meter_in;
        fmsmoov::LoudnessMeter* loudness_meter_in;
        fmsmoov::TwoBandSlowAGC* two_band_slow_agc;
        fmsmoov::LevelMeter* level_meter_out;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessorUI)
};

