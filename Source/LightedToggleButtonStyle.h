#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"

struct LightedToggleButtonStyle : public juce::LookAndFeel_V4
{
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& b, bool is_mouse_over_button, bool is_button_down) override {
        auto bounds = b.getLocalBounds().toFloat();

        auto baseColour = b.getToggleState() ? color_button_on : color_button_off;
        auto text_color = b.getToggleState() ? text_color_button_on : text_color_button_off;
        g.setColour(is_mouse_over_button ? baseColour.brighter(0.2f) : baseColour);
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(text_color);
        g.setFont(15.0f);
        g.drawText(b.getButtonText(), bounds, juce::Justification::centred);
    }

    juce::Colour color_button_off{ juce::Colours::red };
    juce::Colour text_color_button_off{ juce::Colours::white };
    juce::Colour color_button_on{ juce::Colours::green };
    juce::Colour text_color_button_on{ juce::Colours::white };

};

