#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"
#include "SettingsRegistry.h"
#include "SettingsSliderWire.h"
#include "SettingsToggleButtonWire.h"
#include "SettingsComboBoxWire.h"

class TestGenGroup : public juce::Component
{
public:
    TestGenGroup(const juce::String& _component_name, const juce::String& _name, SettingsRegistry& _settings_reg);
    virtual ~TestGenGroup();
    void init(fmsmoov::GEN_TYPE type, float freq, float ampl, bool enable);

    void paint(juce::Graphics&) override;
    void resized() override;

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

    std::unique_ptr<fmsmoov::SettingsComboBoxWire> type_combo_box_wire;
    std::unique_ptr<fmsmoov::SettingsSliderWire> frequency_slider_wire;
    std::unique_ptr<fmsmoov::SettingsSliderWire> amplitude_slider_wire;
    std::unique_ptr<fmsmoov::SettingsToggleButtonWire> enable_toggle_button_wire;
};

