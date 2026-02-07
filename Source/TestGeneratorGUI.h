#pragma once

#include <JuceHeader.h>

#include "TestGenGroup.h"
#include "SettingsRegistry.h"
#include "LightedToggleButtonStyle.h"

class TestGeneratorGUI : public juce::Component, public juce::ValueTree::Listener
{
    public:
        TestGeneratorGUI(SettingsRegistry& _settings_reg);
        virtual ~TestGeneratorGUI();
        void paint(juce::Graphics&) override;
        void resized() override;
        void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;
        std::function<void()> on_done_clicked;
    private:
        juce::TextButton done_button{ "Done" };
        TestGenGroup* group_gen_01;
        TestGenGroup* group_gen_02;
        TestGenGroup* group_gen_03;
        LightedToggleButtonStyle lighted_toggle_style;
        juce::ToggleButton button_gens_mute{ "MUTE ALL" };

        SettingsRegistry& settings_reg;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestGeneratorGUI)
};

