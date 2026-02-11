#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"
#include "LightedToggleButtonStyle.h"

class MainProcessorUI : public juce::Component, public juce::ValueTree::Listener
{
    public:
        MainProcessorUI(SettingsRegistry& _settings_reg);
        virtual ~MainProcessorUI();
        void paint(juce::Graphics&) override;
        void resized() override;
        void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;
    private:
        //TestGenGroup* group_gen_01;
        //TestGenGroup* group_gen_02;
        //TestGenGroup* group_gen_03;
        LightedToggleButtonStyle lighted_toggle_style;
        juce::ToggleButton button_master_bypass{ "MASTER BYPASS" };

        SettingsRegistry& settings_reg;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainProcessorUI)
};

