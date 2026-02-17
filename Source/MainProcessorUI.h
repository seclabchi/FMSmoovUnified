#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"
#include "LightedToggleButtonStyle.h"
#include "LightedButton.h"
#include "LevelMeter.h"

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
private:
        LightedToggleButtonStyle lighted_toggle_style;
        std::unique_ptr<fmsmoov::LightedButton> button_main_bypass;
        std::unique_ptr <fmsmoov::LightedButton> button_generator_state;
        std::unique_ptr <fmsmoov::LightedButton> button_processor_bypass;

        SettingsRegistry& settings_reg;
        fmsmoov::LevelMeter* level_meter_in;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessorUI)
};

