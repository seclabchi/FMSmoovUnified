#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"
#include "SettingsRegistry.h"

class ButtonLightPulseSource : 
    public juce::ValueTree::Listener,
    public juce::DeletedAtShutdown
{
public:
    static void init(SettingsRegistry* _settings_reg, juce::Component& root_component);
    void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;
    double get_vblank_time();

    JUCE_DECLARE_SINGLETON(ButtonLightPulseSource, false);

private:
    ButtonLightPulseSource();
    virtual ~ButtonLightPulseSource() { clearSingletonInstance(); }
    static SettingsRegistry* settings_reg;

    float pulse_speed = 0.0f;
    std::unique_ptr<juce::VBlankAnimatorUpdater> updater;
    std::atomic<double> master_vblank_time{ 0.0f };
    std::unique_ptr<juce::Animator> master_pulse;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonLightPulseSource)
};

