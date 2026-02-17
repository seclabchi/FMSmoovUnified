#include "LightedButton.h"
#include "ButtonLightPulseSource.h"

namespace fmsmoov {

    LightedButton::LightedButton(const juce::String& label, SettingsRegistry& _settings_reg, juce::String& _prop_name, juce::Colour _base_color,
        juce::Colour _off_text_color, juce::Colour _on_text_color) : juce::TextButton(label), settings_reg(_settings_reg), prop_name(_prop_name),
        base_color(_base_color), off_text_color(_off_text_color), on_text_color(_on_text_color)
    {
        v_blank_attachment = std::make_unique<juce::VBlankAttachment>(this, [this] {
            repaint();
            });
        setClickingTogglesState(true);
        this->setColour(juce::TextButton::textColourOnId, on_text_color);
        this->setColour(juce::TextButton::textColourOffId, off_text_color);

        settings_wire = settings_reg.create_text_button_attachment(prop_name, *this);
    }

    LightedButton::~LightedButton() {

    }

    void LightedButton::clicked() {
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
            auto glowColor = base_color.withAlpha(0.6f + (0.4f * global_alpha));

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

} //namespace fmsmoov