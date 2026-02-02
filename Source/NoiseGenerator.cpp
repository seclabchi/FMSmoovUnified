#include "NoiseGenerator.h"

NoiseGenerator::NoiseGenerator(SettingsRegistry& _settings_reg) : settings_reg(_settings_reg) {
    settings_reg.state.addListener(this);
}

NoiseGenerator::~NoiseGenerator() {

}

void NoiseGenerator::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) {

}