#pragma once

#include <JuceHeader.h>

#include "TestGenGroup.h"
#include "SettingsRegistry.h"

class TestGeneratorGUI : public juce::Component
{
	public:
		TestGeneratorGUI(SettingsRegistry& settings_reg);
		virtual ~TestGeneratorGUI();
		void paint(juce::Graphics&) override;
		void resized() override;

	private:
		juce::TextButton done_button{ "Done" };
		TestGenGroup* group_gen_01;
		TestGenGroup* group_gen_02;
		TestGenGroup* group_gen_03;

		SettingsRegistry& settings_reg;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestGeneratorGUI)
};

