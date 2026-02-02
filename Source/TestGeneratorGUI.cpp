#include "TestGeneratorGUI.h"

TestGeneratorGUI::TestGeneratorGUI(SettingsRegistry& _settings_reg) : settings_reg(_settings_reg) {
	addAndMakeVisible(done_button);

	group_gen_01 = new TestGenGroup("gen01", "GEN 01", settings_reg);
	group_gen_02 = new TestGenGroup("gen02", "GEN 02", settings_reg);
	group_gen_03 = new TestGenGroup("gen03", "GEN 03", settings_reg);

	done_button.onClick = [this] {
		juce::Logger::writeToLog("Done clicked");

		settings_reg.state.setProperty("gen01_enable", false, nullptr);
		settings_reg.state.setProperty("gen02_enable", false, nullptr);
		settings_reg.state.setProperty("gen03_enable", false, nullptr);

		if (auto* dw = findParentComponentOfClass<juce::Component>()) {
			dw->exitModalState(1);
		}
	};

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

	setSize(800, 400);
}

TestGeneratorGUI::~TestGeneratorGUI() {
	settings_reg.state.setProperty("gen01_enable", false, nullptr);
	settings_reg.state.setProperty("gen02_enable", false, nullptr);
	settings_reg.state.setProperty("gen03_enable", false, nullptr);

	delete group_gen_01;
	delete group_gen_02;
	delete group_gen_03;
}

void TestGeneratorGUI::paint(juce::Graphics& g) {
	//g.fillAll(juce::Colours::red);
}

void TestGeneratorGUI::resized() {
	auto area = getLocalBounds().reduced(10);
	auto button_area = area.removeFromBottom(32);

	auto gen_area = area.reduced(5);

	juce::FlexBox fb_gens;
	fb_gens.flexDirection = juce::FlexBox::Direction::row;
	fb_gens.items.add(juce::FlexItem(*group_gen_01).withFlex(1.0).withMargin(5));
	fb_gens.items.add(juce::FlexItem(*group_gen_02).withFlex(1.0).withMargin(5));
	fb_gens.items.add(juce::FlexItem(*group_gen_03).withFlex(1.0).withMargin(5));

	fb_gens.performLayout(gen_area);

	/*
	 * Layout the done button
	 */


	juce::FlexBox fb;
	fb.flexDirection = juce::FlexBox::Direction::row;
	fb.justifyContent = juce::FlexBox::JustifyContent::flexEnd;
	fb.alignContent = juce::FlexBox::AlignContent::center;

	fb.items.add(juce::FlexItem(done_button).withWidth(80).withHeight(30).withMargin({ 0,8,0,0 }));

	fb.performLayout(button_area);
}