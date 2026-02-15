#include "LightedButton.h"

namespace fmsmoov {

    LightedButton::LightedButton(const juce::String& label) : juce::TextButton(label)
    {        
        pulse_animator = std::make_unique<juce::Animator>(
            juce::ValueAnimatorBuilder()
            .withDurationMs(500)
            .withEasing([](auto progress) {
                // Map 0->1 progress to 0->1->0 pulse using a sine wave
                return (std::sin(progress * juce::MathConstants<double>::twoPi - juce::MathConstants<double>::halfPi) + 1.0) / 2.0;
                })
            .runningInfinitely()
            .withValueChangedCallback([this](auto value) {
                pulseAlpha = (float)value;
                repaint();
                })
            .build()
        );

        setClickingTogglesState(true);

    }

    LightedButton::~LightedButton() {

    }

    void LightedButton::clicked() {
        update_animation_state();
    }

    void LightedButton::paintButton(juce::Graphics & g, bool isMouseOver, bool isButtonDown) {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        auto cornerSize = 6.0f;

        // 1. Draw the "Off" or "Base" background
        g.setColour(findColour(juce::TextButton::buttonColourId));
        g.fillRoundedRectangle(bounds, cornerSize);

        // 2. Draw the "On" Pulse (only if toggled on)
        if (getToggleState())
        {
            // Use pulseAlpha to change brightness or opacity
            auto glowColor = juce::Colours::cyan.withAlpha(0.3f + (0.7f * pulseAlpha));

            g.setColour(glowColor);
            g.fillRoundedRectangle(bounds, cornerSize);

            // Optional: Draw a bright border that pulses too
            g.setColour(juce::Colours::white.withAlpha(pulseAlpha));
            g.drawRoundedRectangle(bounds, cornerSize, 2.0f);
        }

        // 3. Draw the Text (optional: JUCE can do this for you if you don't override)
        // Since we inherited from TextButton, we can just call the base text drawer
        // but usually, you'd just draw it manually here for full control:
        g.setColour(findColour(juce::TextButton::textColourOnId));


        auto font = getLookAndFeel().getTextButtonFont(*this, getHeight());
        g.setFont(font);
        g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred);

    }

    void LightedButton::update_animation_state() {
        if (getToggleState()) {
            updater.addAnimator(*pulse_animator);
            pulse_animator->start();
        }
        else {
            updater.removeAnimator(*pulse_animator);
            pulseAlpha = 0.0f;
        }
        repaint();
    }

} //namespace fmsmoov