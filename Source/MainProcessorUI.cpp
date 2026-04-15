#include "MainProcessorUI.h"
#include "SettingsTextButtonWire.h"

MainProcessorUI::MainProcessorUI(SettingsRegistry& _settings_reg) : settings_reg(_settings_reg)
{
    button_main_bypass = std::make_unique<fmsmoov::LightedButton>(juce::String("MAIN BYPASS"), settings_reg, juce::String("main_bypass"), juce::Colours::red, juce::Colours::white, juce::Colours::white);
    button_generator_state = std::make_unique<fmsmoov::LightedButton>(juce::String("GENERATOR ON"), settings_reg, juce::String("generator_state"), juce::Colours::yellow, juce::Colours::white, juce::Colours::black);
    button_processor_bypass = std::make_unique<fmsmoov::LightedButton>(juce::String("PROCESSOR BYPASS"), settings_reg, juce::String("processor_bypass"), juce::Colours::red, juce::Colours::white, juce::Colours::white);

    settings_reg.add_main_proc_settings_listener(this);

    lighted_toggle_style.color_button_off = juce::Colours::lightgrey;
    lighted_toggle_style.text_color_button_off = juce::Colours::black;
    lighted_toggle_style.color_button_on = juce::Colours::red;
    lighted_toggle_style.text_color_button_on = juce::Colours::white;

    button_main_bypass->setLookAndFeel(&lighted_toggle_style);
    button_main_bypass->setTooltip(TOOLTIP_BUTTON_MAIN_BYPASS);
    button_main_bypass->setToggleState(settings_reg.get_main_bypass(), juce::dontSendNotification);
  
    addAndMakeVisible(*button_main_bypass);

    button_generator_state->setLookAndFeel(&lighted_toggle_style);
    button_generator_state->setTooltip(TOOLTIP_BUTTON_GENERATOR_STATE);
    button_generator_state->setToggleState(settings_reg.get_generator_state(), juce::dontSendNotification);

    addAndMakeVisible(*button_generator_state);

    button_processor_bypass->setLookAndFeel(&lighted_toggle_style);
    button_processor_bypass->setTooltip(TOOLTIP_BUTTON_PROCESSOR_BYPASS);
    button_processor_bypass->setToggleState(settings_reg.get_processor_bypass(), juce::dontSendNotification);

    addAndMakeVisible(*button_processor_bypass);

    /**************** MAIN PROCESSOR COMPONENTS ***************************************************************/

 

}

MainProcessorUI::~MainProcessorUI() {
    settings_reg.remove_main_proc_settings_listener(this);
}

void MainProcessorUI::add_level_meter_in(fmsmoov::LevelMeter* _level_meter_in) {

    level_meter_in = _level_meter_in;
    addAndMakeVisible(*level_meter_in);
    level_meter_in->setSize(35, 300);
    level_meter_in->setTopLeftPosition(50, 50);
}

void MainProcessorUI::add_loudness_meter_in(fmsmoov::LoudnessMeter* _loudness_meter_in) {
    loudness_meter_in = _loudness_meter_in;
    addAndMakeVisible(*loudness_meter_in);
    loudness_meter_in->setSize(35, 300);
    loudness_meter_in->setTopLeftPosition(120, 50);
}

void MainProcessorUI::add_two_band_slow_agc(fmsmoov::TwoBandSlowAGC* _two_band_slow_agc) {
    two_band_slow_agc = _two_band_slow_agc;
    addAndMakeVisible(*two_band_slow_agc);
    two_band_slow_agc->setSize(70, 300);
    two_band_slow_agc->setTopLeftPosition(190, 50);
}

void MainProcessorUI::add_level_meter_out(fmsmoov::LevelMeter* _level_meter_out) {
    level_meter_out = _level_meter_out;
    addAndMakeVisible(*level_meter_out);
    level_meter_out->setSize(35, 300);
    level_meter_out->setTopLeftPosition(400, 50);
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

    fb.items.add(juce::FlexItem(*button_main_bypass).withWidth(150).withHeight(30).withMargin({ 0,8,0,0 }));
    fb.items.add(juce::FlexItem(*button_processor_bypass).withWidth(150).withHeight(30).withMargin({ 0,8,0,0 }));
    fb.items.add(juce::FlexItem(*button_generator_state).withWidth(150).withHeight(30).withMargin({ 0,8,0,0 }));

    fb.performLayout(button_area);
}

