#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"
#include "LightedToggleButtonStyle.h"
#include "LightedButton.h"

class MainProcessorUI : public juce::Component, public SettingsRegistry::MainProcSettingsListener
{
    public:
        MainProcessorUI(SettingsRegistry& _settings_reg);
        virtual ~MainProcessorUI();
        void paint(juce::Graphics&) override;
        void resized() override;
        void master_bypass_changed(bool master_bypass) override;
        void generator_state_changed(bool mute_all_gens) override {};
        void processor_bypass_changed(bool mute_processor) override {};
private:
        LightedToggleButtonStyle lighted_toggle_style;
        fmsmoov::LightedButton button_master_bypass{ "MASTER BYPASS", juce::Colours::red, juce::Colours::white, juce::Colours::white };
        fmsmoov::LightedButton  button_generator_state{ "GENERATOR ON", juce::Colours::yellow, juce::Colours::white, juce::Colours::black };
        fmsmoov::LightedButton  button_processor_bypass{ "PROCESSOR BYPASS", juce::Colours::red, juce::Colours::white, juce::Colours::white };

        SettingsRegistry& settings_reg;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessorUI)
};

