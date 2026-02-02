#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"
#include "SettingsRegistry.h"

class TestGenGroup : public juce::Component
{
public:
    TestGenGroup(const juce::String& _component_name, const juce::String& _name, SettingsRegistry& _settings_reg);
    virtual ~TestGenGroup();
    void init(int type, float freq, float ampl, bool enable);

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void(fmsmoov::GEN_TYPE)> on_gen_type_changed;
    std::function<void(double)> on_frequency_changed;
    std::function<void(double)> on_amplitude_changed;
    std::function<void(bool)> on_enabled_changed;

private:
    juce::GroupComponent group;
    juce::Label gen_type_label;
    juce::ComboBox gen_type;
    juce::Value gen_type_value;
    juce::String gen_type_propname;
    juce::Label frequency_label;
    juce::Slider frequency;
    juce::String frequency_propname;
    juce::Label amplitude_label;
    juce::Slider amplitude;
    juce::String amplitude_propname;
    juce::ToggleButton enabled;
    juce::String enable_propname;
    SettingsRegistry& settings_reg;
};

