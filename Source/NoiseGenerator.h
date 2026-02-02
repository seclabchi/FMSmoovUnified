#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"
#include "SettingsRegistry.h"

class NoiseGenerator : 
	public juce::AudioSource,
	public juce::ValueTree::Listener
{
public:
	NoiseGenerator(SettingsRegistry& _settings_reg);
	virtual ~NoiseGenerator();
	void NoiseGenerator::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;

private:
	fmsmoov::GEN_TYPE type;
	SettingsRegistry& settings_reg;
};

