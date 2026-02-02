#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"

class SettingsRegistry : public juce::ValueTree::Listener
{
public:
	juce::ValueTree state{ "SETTINGS" };

	std::atomic<fmsmoov::GEN_TYPE> gen01_type { fmsmoov::GEN_TYPE::SINE };
	std::atomic<fmsmoov::GEN_TYPE> gen02_type { fmsmoov::GEN_TYPE::SINE };
	std::atomic<fmsmoov::GEN_TYPE> gen03_type { fmsmoov::GEN_TYPE::SINE };

	std::atomic<float> gen01_freq{ 440.0f };
	std::atomic<float> gen02_freq{ 1000.0f };
	std::atomic<float> gen03_freq{ 5000.0f };

	std::atomic<float> gen01_ampl{ -12.0f };
	std::atomic<float> gen02_ampl{ -12.0f };
	std::atomic<float> gen03_ampl{ -12.0f };

	std::atomic<bool> gen01_enable{ false };
	std::atomic<bool> gen02_enable{ false };
	std::atomic<bool> gen03_enable{ false };

	SettingsRegistry(const juce::String& _reg_name);
	virtual ~SettingsRegistry();

	void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& ident) override;

private:
	const juce::String& reg_name;
	void update_atomics();

};

