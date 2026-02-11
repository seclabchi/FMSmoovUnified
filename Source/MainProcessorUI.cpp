#include "MainProcessorUI.h"

MainProcessorUI::MainProcessorUI(SettingsRegistry& _settings_reg) : settings_reg(_settings_reg) {
    settings_reg.state.addListener(this);

    //group_gen_01 = new TestGenGroup("gen01", "GEN 01", settings_reg);
    //group_gen_02 = new TestGenGroup("gen02", "GEN 02", settings_reg);
    //group_gen_03 = new TestGenGroup("gen03", "GEN 03", settings_reg);

    button_master_bypass.onClick = [this] {
        bool master_bypass = button_master_bypass.getToggleState();
        settings_reg.state.setProperty("master_bypass", master_bypass, nullptr);
    };

    lighted_toggle_style.color_button_off = juce::Colours::lightgrey;
    lighted_toggle_style.text_color_button_off = juce::Colours::black;
    lighted_toggle_style.color_button_on = juce::Colours::red;
    lighted_toggle_style.text_color_button_on = juce::Colours::white;

    button_master_bypass.setLookAndFeel(&lighted_toggle_style);
    button_master_bypass.setToggleState(settings_reg.state.getProperty("master_bypass"), juce::dontSendNotification);
  
    addAndMakeVisible(button_master_bypass);

    /*
    group_gen_01->init(settings_reg.state.getProperty("gen01_type"),
        settings_reg.state.getProperty("gen01_freq"),
        settings_reg.state.getProperty("gen01_ampl"),
        settings_reg.state.getProperty("gen01_enable"));

    addAndMakeVisible(group_gen_01);

    group_gen_02->init(settings_reg.state.getProperty("gen02_type"),
        settings_reg.state.getProperty("gen02_freq"),
        settings_reg.state.getProperty("gen02_ampl"),
        settings_reg.state.getProperty("gen02_enable"));

    addAndMakeVisible(group_gen_02);

    group_gen_03->init(settings_reg.state.getProperty("gen03_type"),
        settings_reg.state.getProperty("gen03_freq"),
        settings_reg.state.getProperty("gen03_ampl"),
        settings_reg.state.getProperty("gen03_enable"));

    addAndMakeVisible(group_gen_03);
    */
    settings_reg.state.addListener(this);
    setSize(800, 400);
}

MainProcessorUI::~MainProcessorUI() {
    settings_reg.state.removeListener(this);

    //delete group_gen_01;
    //delete group_gen_02;
    //delete group_gen_03;
}

void MainProcessorUI::paint(juce::Graphics& g) {
    //g.fillAll(juce::Colours::red);
}

void MainProcessorUI::resized() {
    auto area = getLocalBounds().reduced(10);
    auto button_area = area.removeFromBottom(32);

    auto gen_area = area.reduced(5);

    /*
    juce::FlexBox fb_gens;
    fb_gens.flexDirection = juce::FlexBox::Direction::row;
    fb_gens.items.add(juce::FlexItem(*group_gen_01).withFlex(1.0).withMargin(5));
    fb_gens.items.add(juce::FlexItem(*group_gen_02).withFlex(1.0).withMargin(5));
    fb_gens.items.add(juce::FlexItem(*group_gen_03).withFlex(1.0).withMargin(5));

    fb_gens.performLayout(gen_area);
    */

    /*
     * Layout the buttons
     */


    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
    fb.alignContent = juce::FlexBox::AlignContent::center;

    fb.items.add(juce::FlexItem(button_master_bypass).withWidth(120).withHeight(30).withMargin({ 0,8,0,0 }));

    fb.performLayout(button_area);
}

void MainProcessorUI::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) {

}