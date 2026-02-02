#include "TestGenGroup.h"

TestGenGroup::TestGenGroup(const juce::String& _component_name, const juce::String& _name,
    SettingsRegistry& _settings_reg) : group(_component_name, _name), settings_reg(_settings_reg) {

    gen_type_propname = _component_name + "_" + "type";
    frequency_propname = _component_name + "_" + "freq";
    amplitude_propname = _component_name + "_" + "ampl";
    enable_propname = _component_name + "_" + "enable";

    on_gen_type_changed = NULL;
    on_frequency_changed = NULL;
    on_amplitude_changed = NULL;
    on_enabled_changed = NULL;

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

    gen_type_value.referTo(settings_reg.state.getPropertyAsValue(gen_type_propname, nullptr));
    gen_type.getSelectedIdAsValue().referTo(gen_type_value);
    frequency.getValueObject().referTo(settings_reg.state.getPropertyAsValue(frequency_propname, nullptr));
    amplitude.getValueObject().referTo(settings_reg.state.getPropertyAsValue(amplitude_propname, nullptr));
    enabled.getToggleStateValue().referTo(settings_reg.state.getPropertyAsValue(enable_propname, nullptr));

    /* We just need to manually notify one of the generator properties, because this will trigger a refresh of
     * all of the generator atomics for the audio loop.
     */

    settings_reg.state.sendPropertyChangeMessage(frequency_propname);

    gen_type.onChange = [this] {
        auto selected_gen_type = static_cast<fmsmoov::GEN_TYPE>(gen_type.getSelectedId());
        settings_reg.state.setProperty(gen_type_propname, selected_gen_type, nullptr);
        if (on_gen_type_changed) {
            on_gen_type_changed(selected_gen_type);
        }
        };

    frequency.onValueChange = [this] {
        auto selected_frequency = frequency.getValue();
        if (on_frequency_changed) {
            on_frequency_changed(selected_frequency);
        }
    };

    amplitude.onValueChange = [this] {
        auto selected_amplitude = frequency.getValue();
        if (on_amplitude_changed) {
            on_frequency_changed(selected_amplitude);
        }
        };

    enabled.onStateChange = [this] {
        auto selected_enabled = enabled.getState();
        if (on_enabled_changed) {
            on_enabled_changed(selected_enabled);
        }
        };
}

TestGenGroup::~TestGenGroup() {

}

void TestGenGroup::init(int type, float freq, float ampl, bool enable) {
    gen_type.setSelectedId(type, juce::NotificationType::dontSendNotification);
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