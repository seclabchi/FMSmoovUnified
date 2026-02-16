#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"

namespace fmsmoov {

    class LightedButton : public juce::TextButton
    {
    public:
        LightedButton(const juce::String& label, juce::Colour _base_color, juce::Colour _off_text_color, juce::Colour _on_text_color);
        virtual ~LightedButton();
        void paintButton(juce::Graphics& g, bool isMouseOver, bool isButtonDown) override;
        void clicked() override;
        void update_animation_state();
    private:
        std::unique_ptr<juce::Animator> pulse_animator;
        float pulseAlpha = 0.0f;

        std::unique_ptr<juce::VBlankAttachment> v_blank_attachment;
        juce::Colour base_color;
        juce::Colour off_text_color;
        juce::Colour on_text_color;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LightedButton)

    };

} //namespace fmsmoov