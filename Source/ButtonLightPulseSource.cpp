#include "ButtonLightPulseSource.h"

SettingsRegistry* ButtonLightPulseSource::settings_reg = nullptr;

JUCE_IMPLEMENT_SINGLETON(ButtonLightPulseSource)

void ButtonLightPulseSource::init(SettingsRegistry* _settings_reg, juce::Component& root_component) {
    /* re-initing this with a different settings registry is a no-no in my current design */
    auto instance = getInstance();
    jassert((instance->settings_reg == nullptr) && (instance->updater == nullptr));
    settings_reg = _settings_reg;
    getInstance()->updater = std::make_unique<juce::VBlankAnimatorUpdater>(&root_component);
    
    instance->master_pulse = std::make_unique<juce::Animator>(
        juce::ValueAnimatorBuilder()
        .withDurationMs(750)
        .withEasing([](auto progress) {
            // Map 0->1 progress to 0->1->0 pulse using a sine wave
            return (std::sin(progress * juce::MathConstants<double>::twoPi - juce::MathConstants<double>::halfPi) + 1.0) / 2.0;
            })
        .runningInfinitely()
        .withValueChangedCallback([instance](auto value) {
            instance->master_vblank_time.store((float)value);
            })
        .build()
    );

    instance->updater->addAnimator(*(instance->master_pulse));
    instance->master_pulse->start();
}

ButtonLightPulseSource::ButtonLightPulseSource() {
}


double ButtonLightPulseSource::get_vblank_time() {
    return master_vblank_time.load(std::memory_order_relaxed);
}

void ButtonLightPulseSource::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) {

}