#include "TestGenerator.h"

TestGenerator::TestGenerator(SettingsRegistry& _settings_reg) : settings_reg(_settings_reg) {
    
    settings_reg.state.addListener(this);

    tone_gen_01 = new juce::ToneGeneratorAudioSource();
    tone_gen_02 = new juce::ToneGeneratorAudioSource();
    tone_gen_03 = new juce::ToneGeneratorAudioSource();

    mixer = new juce::MixerAudioSource();
}

TestGenerator::~TestGenerator() {
    mixer->removeAllInputs();
    
    delete mixer;
    
    delete tone_gen_01;
    delete tone_gen_02;
    delete tone_gen_03;
}

void TestGenerator::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    mixer->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void TestGenerator::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    if (should_update_mixer.load()) {
        update_mixer();
    }

    if (should_update_gens.load()) {
        update_gens();
    }

    mixer->getNextAudioBlock(bufferToFill);
}

void TestGenerator::releaseResources() {
    mixer->releaseResources();
    tone_gen_01->releaseResources();
    tone_gen_02->releaseResources();
    tone_gen_03->releaseResources();
}

void TestGenerator::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) {
    if (p == juce::Identifier("gen01_enable") ||
        p == juce::Identifier("gen02_enable") ||
        p == juce::Identifier("gen03_enable")) {
        should_update_mixer.store(true);
    }

    if (p == juce::Identifier("gen01_freq") ||
        p == juce::Identifier("gen02_freq") ||
        p == juce::Identifier("gen03_freq") ||
        p == juce::Identifier("gen01_ampl") ||
        p == juce::Identifier("gen02_ampl") ||
        p == juce::Identifier("gen03_ampl") ||
        p == juce::Identifier("gen01_type") ||
        p == juce::Identifier("gen02_type") ||
        p == juce::Identifier("gen03_type")) {
        should_update_gens.store(true);
    }
}

void TestGenerator::update_gens() {
    tone_gen_01->setFrequency(settings_reg.gen01_freq);
    tone_gen_01->setAmplitude(juce::Decibels::decibelsToGain(settings_reg.gen01_ampl.load()));
    tone_gen_02->setFrequency(settings_reg.gen02_freq);
    tone_gen_02->setAmplitude(juce::Decibels::decibelsToGain(settings_reg.gen02_ampl.load()));
    tone_gen_03->setFrequency(settings_reg.gen03_freq);
    tone_gen_03->setAmplitude(juce::Decibels::decibelsToGain(settings_reg.gen03_ampl.load()));
    

    should_update_gens.store(false);
}

void TestGenerator::update_mixer() {
    if (settings_reg.gen01_enable.load()) {
        mixer->addInputSource(tone_gen_01, false);
    }
    else {
        mixer->removeInputSource(tone_gen_01);
    }

    if (settings_reg.gen02_enable.load()) {
        mixer->addInputSource(tone_gen_02, false);
    }
    else {
        mixer->removeInputSource(tone_gen_02);
    }

    if (settings_reg.gen03_enable.load()) {
        mixer->addInputSource(tone_gen_03, false);
    }
    else {
        mixer->removeInputSource(tone_gen_03);
    }

    should_update_mixer.store(false);
}