#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"

class TestGenerator : public juce::AudioSource, public juce::ValueTree::Listener
{
public:
	TestGenerator(SettingsRegistry& _settings_reg);
	virtual ~TestGenerator();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;


private:
	juce::ToneGeneratorAudioSource* tone_gen_01;
	juce::ToneGeneratorAudioSource* tone_gen_02;
	juce::ToneGeneratorAudioSource* tone_gen_03;

	juce::MixerAudioSource* mixer;
	std::atomic<bool> should_update_mixer{ false };
	std::atomic<bool> should_update_gens{ false };
	void update_gens();
	void update_mixer();

	SettingsRegistry& settings_reg;

};

