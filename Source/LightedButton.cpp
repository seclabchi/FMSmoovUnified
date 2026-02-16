#include "LightedButton.h"
#include "ButtonLightPulseSource.h"

namespace fmsmoov {

    LightedButton::LightedButton(const juce::String& label, juce::Colour _base_color,
        juce::Colour _off_text_color, juce::Colour _on_text_color) : juce::TextButton(label), base_color(_base_color),
        off_text_color(_off_text_color), on_text_color(_on_text_color)
    {
        v_blank_attachment = std::make_unique<juce::VBlankAttachment>(this, [this] {
            repaint();
            });
        setClickingTogglesState(true);
        this->setColour(juce::TextButton::textColourOnId, on_text_color);
        this->setColour(juce::TextButton::textColourOffId, off_text_color);
    }

    LightedButton::~LightedButton() {

    }

    void LightedButton::clicked() {
        update_animation_state();
    }

    void LightedButton::paintButton(juce::Graphics & g, bool isMouseOver, bool isButtonDown) {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        auto cornerSize = 6.0f;
        float global_alpha = ButtonLightPulseSource::getInstance()->get_vblank_time();

        // 1. Draw the "Off" or "Base" background
        g.setColour(findColour(juce::TextButton::buttonColourId));
        g.fillRoundedRectangle(bounds, cornerSize);

        // 2. Draw the "On" Pulse (only if toggled on)
        if (getToggleState())
        {
            // Use pulseAlpha to change brightness or opacity
            auto glowColor = base_color.withAlpha(0.3f + (0.7f * global_alpha));

            g.setColour(glowColor);
            g.fillRoundedRectangle(bounds, cornerSize);

            // Optional: Draw a bright border that pulses too
            g.setColour(juce::Colours::white.withAlpha(global_alpha));
            g.drawRoundedRectangle(bounds, cornerSize, 2.0f);
            g.setColour(findColour(juce::TextButton::textColourOnId));
        }
        else {
            g.setColour(findColour(juce::TextButton::textColourOffId));
        }


        auto font = getLookAndFeel().getTextButtonFont(*this, getHeight());
        g.setFont(font);
        g.drawText(getButtonText(), getLocalBounds(), juce::Justification::centred);

    }

    void LightedButton::update_animation_state() {
        if (getToggleState()) {
            //pulse_animator->start();
        }
        else {
            //pulseAlpha = 0.0f;
        }
        repaint();
    }

} //namespace fmsmoov