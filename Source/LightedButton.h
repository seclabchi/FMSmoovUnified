#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"

namespace fmsmoov {

    class LightedButton : public juce::TextButton
    {
    public:
        LightedButton(const juce::String& label);
        virtual ~LightedButton();
        void paintButton(juce::Graphics& g, bool isMouseOver, bool isButtonDown) override;
        void clicked() override;
        void update_animation_state();
    private:
        juce::VBlankAnimatorUpdater updater{ this };
        std::unique_ptr<juce::Animator> pulse_animator;
        float pulseAlpha = 0.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightedButton)

    };

} //namespace fmsmoov