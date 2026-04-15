#include "TestGenGroup.h"

TestGenGroup::TestGenGroup(const juce::String& _component_name, const juce::String& _name,
    SettingsRegistry& _settings_reg) : group(_component_name, _name), settings_reg(_settings_reg) {

    gen_type_propname = _component_name + "_" + "type";
    frequency_propname = _component_name + "_" + "freq";
    amplitude_propname = _component_name + "_" + "ampl";
    enable_propname = _component_name + "_" + "enable";

    addAndMakeVisible(group);

    addAndMakeVisible(gen_type_label);
    addAndMakeVisible(gen_type);
    gen_type_label.setText("Gen Type", juce::dontSendNotification);

    addAndMakeVisible(frequency_label);
    frequency.setRange(20.0, 20000.0, 0.1);
    frequency.setSkewFactor(0.3);

    addAndMakeVisible(frequency);
    frequency_label.setText("Frequency", juce::dontSendNotification);
    
    addAndMakeVisible(amplitude_label);
    amplitude.setRange(-60.0, 20.0, 0.1);
    amplitude.setSkewFactor(2);
    addAndMakeVisible(amplitude);
    amplitude_label.setText("Amplitude", juce::dontSendNotification);
    
    enabled.setButtonText("Enabled");
    addAndMakeVisible(enabled);

    const std::map<fmsmoov::GEN_TYPE, juce::String> gen_type_map = {
        { fmsmoov::GEN_TYPE::SINE, "Sine" },
        { fmsmoov::GEN_TYPE::SAWTOOTH, "Sawtooth" },
        { fmsmoov::GEN_TYPE::TRIANGLE, "Triangle" },
        { fmsmoov::GEN_TYPE::SQUARE, "Square" },
        { fmsmoov::GEN_TYPE::NOISE_WHITE, "White Noise" },
        { fmsmoov::GEN_TYPE::NOISE_PINK, "Pink Noise" },
        { fmsmoov::GEN_TYPE::NOISE_BROWN, "Brown Noise" },
        { fmsmoov::GEN_TYPE::NOISE_BLUE, "Blue Noise" }
    };

    for (auto const& [type, name] : gen_type_map) {
        /* JUCE IDs must be >= 1 */
        gen_type.addItem(name, static_cast<int>(type));
    }

    fmsmoov::GEN_TYPE type;
    float freq;
    float ampl;
    bool enable;

    settings_reg.get_gen_params(_component_name, type, freq, ampl, enable);
    this->init(type, freq, ampl, enable);
    
    type_combo_box_wire = settings_reg.create_combo_box_attachment(gen_type_propname, gen_type);
    frequency_slider_wire = settings_reg.create_slider_attachment(frequency_propname, frequency);
    amplitude_slider_wire = settings_reg.create_slider_attachment(amplitude_propname, amplitude);
    enable_toggle_button_wire = settings_reg.create_toggle_button_attachment(enable_propname, enabled);

}

TestGenGroup::~TestGenGroup() {

}

void TestGenGroup::init(fmsmoov::GEN_TYPE type, float freq, float ampl, bool enable) {
    gen_type.setSelectedId(static_cast<int>(type), juce::NotificationType::dontSendNotification);
    frequency.setValue(freq, juce::NotificationType::dontSendNotification);
    amplitude.setValue(ampl, juce::NotificationType::dontSendNotification);
    enabled.setToggleState(enable, juce::NotificationType::dontSendNotification);
}

void TestGenGroup::paint(juce::Graphics& g) {
    //g.fillAll(juce::Colours::red);
}

void TestGenGroup::resized() {
    group.setBounds(getLocalBounds());

    auto inner_area = getLocalBounds().reduced(15);
    inner_area.removeFromTop(10);

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    fb.alignItems = juce::FlexBox::AlignItems::stretch;

    fb.items.add(juce::FlexItem(gen_type_label).withHeight(24.0f).withMargin({ 0,0,5,0 }));
    fb.items.add(juce::FlexItem(gen_type).withHeight(24.0f).withMargin({ 0,0,20,0 }));
    fb.items.add(juce::FlexItem(frequency_label).withHeight(24.0f).withMargin({ 0,0,5,0 }));
    fb.items.add(juce::FlexItem(frequency).withHeight(24.0f).withMargin({ 0,0,20,0 }));
    fb.items.add(juce::FlexItem(amplitude_label).withHeight(24.0f).withMargin({ 0,0,5,0 }));
    fb.items.add(juce::FlexItem(amplitude).withHeight(24.0f).withMargin({ 0,0,20,0 }));
    fb.items.add(juce::FlexItem(enabled).withHeight(24.0f).withMargin({ 0,0,10,0 }));

    fb.performLayout(inner_area);

}