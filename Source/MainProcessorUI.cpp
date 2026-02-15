#include "MainProcessorUI.h"

#define TOOLTIP_BUTTON_MASTER_BYPASS "Bypass entire FMSmoov Toolchain.  Raw in-to-out."
#define TOOLTIP_BUTTON_GENERATOR_STATE "Enable the test tone generator."
#define TOOLTIP_BUTTON_PROCESSOR_BYPASS "Disable the main processor chain."

MainProcessorUI::MainProcessorUI(SettingsRegistry& _settings_reg) : settings_reg(_settings_reg) {
    settings_reg.add_main_proc_settings_listener(this);

    button_master_bypass.onClick = [this] {
        bool master_bypass = button_master_bypass.getToggleState();
        settings_reg.set_master_bypass(master_bypass);
    };

    button_generator_state.onClick = [this] {
        bool generator_state = button_generator_state.getToggleState();
        settings_reg.set_generator_state(generator_state);
        };

    button_master_bypass.onClick = [this] {
        bool processor_bypass = button_processor_bypass.getToggleState();
        settings_reg.set_processor_bypass(processor_bypass);
        };

    lighted_toggle_style.color_button_off = juce::Colours::lightgrey;
    lighted_toggle_style.text_color_button_off = juce::Colours::black;
    lighted_toggle_style.color_button_on = juce::Colours::red;
    lighted_toggle_style.text_color_button_on = juce::Colours::white;

    button_master_bypass.setLookAndFeel(&lighted_toggle_style);
    button_master_bypass.setTooltip(TOOLTIP_BUTTON_MASTER_BYPASS);
    button_master_bypass.setToggleState(settings_reg.get_master_bypass(), juce::dontSendNotification);
  
    addAndMakeVisible(button_master_bypass);

    button_generator_state.setLookAndFeel(&lighted_toggle_style);
    button_generator_state.setTooltip(TOOLTIP_BUTTON_GENERATOR_STATE);
    button_generator_state.setToggleState(settings_reg.get_generator_state(), juce::dontSendNotification);

    addAndMakeVisible(button_generator_state);

    button_processor_bypass.setLookAndFeel(&lighted_toggle_style);
    button_processor_bypass.setTooltip(TOOLTIP_BUTTON_PROCESSOR_BYPASS);
    button_processor_bypass.setToggleState(settings_reg.get_processor_bypass(), juce::dontSendNotification);

    addAndMakeVisible(button_processor_bypass);

    setSize(800, 400);
}

MainProcessorUI::~MainProcessorUI() {
    settings_reg.remove_main_proc_settings_listener(this);
}

void MainProcessorUI::paint(juce::Graphics& g) {
    //g.fillAll(juce::Colours::red);
}

void MainProcessorUI::resized() {
    auto area = getLocalBounds().reduced(10);
    auto button_area = area.removeFromBottom(32);

    auto gen_area = area.reduced(5);

    /*
     * Layout the buttons
     */

    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add(juce::FlexItem(button_master_bypass).withWidth(150).withHeight(30).withMargin({ 0,8,0,0 }));
    fb.items.add(juce::FlexItem(button_processor_bypass).withWidth(150).withHeight(30).withMargin({ 0,8,0,0 }));
    fb.items.add(juce::FlexItem(button_generator_state).withWidth(150).withHeight(30).withMargin({ 0,8,0,0 }));

    fb.performLayout(button_area);
}

void MainProcessorUI::master_bypass_changed(bool master_bypass) {

}