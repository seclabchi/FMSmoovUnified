#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"

struct LightedToggleButtonStyle : public juce::LookAndFeel_V4
{
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& b, bool is_mouse_over_button, bool is_button_down) override {
        auto bounds = b.getLocalBounds().toFloat();

        auto baseColour = b.getToggleState() ? juce::Colours::lightpink : juce::Colours::lightblue;
        g.setColour(is_mouse_over_button ? baseColour.brighter(0.2f) : baseColour);
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(juce::Colours::black);
        g.setFont(15.0f);
        g.drawText(b.getButtonText(), bounds, juce::Justification::centred);
    }
};

