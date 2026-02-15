#pragma once

#include <JuceHeader.h>

#include "TestGenGroup.h"
#include "SettingsRegistry.h"
#include "LightedToggleButtonStyle.h"

class TestGeneratorGUI : public juce::Component, public SettingsRegistry::TestGenSettingsListener
{
    public:
        TestGeneratorGUI(SettingsRegistry& _settings_reg);
        virtual ~TestGeneratorGUI();
        void paint(juce::Graphics&) override;
        void resized() override;
        void gen_params_changed(const juce::String& gen_num) override;
        std::function<void()> on_done_clicked;
    private:
        juce::TextButton done_button{ "Done" };
        TestGenGroup* group_gen_01;
        TestGenGroup* group_gen_02;
        TestGenGroup* group_gen_03;
        LightedToggleButtonStyle lighted_toggle_style;

        SettingsRegistry& settings_reg;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestGeneratorGUI)
};

