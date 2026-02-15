#pragma once

#include <JuceHeader.h>

namespace fmsmoov {

    struct SettingsSliderWire : private juce::ValueTree::Listener {
        SettingsSliderWire(juce::ValueTree& vt, const juce::Identifier& prop, juce::Slider& s) :
            tree(vt), property(prop), slider(s) {

            tree.addListener(this);

            slider.onValueChange = [this] {
                tree.setProperty(property, slider.getValue(), nullptr); 
                };
            slider.setValue(tree.getProperty(property), juce::dontSendNotification);
        }

        ~SettingsSliderWire() override { tree.removeListener(this); }

    private:
        juce::ValueTree tree;
        juce::Identifier property;
        juce::Slider& slider;

        void valueTreePropertyChanged(juce::ValueTree& v, const juce::Identifier& ident) override {
            if (ident == property) {
                juce::MessageManager::callAsync([this] {
                    slider.setValue(tree.getProperty(property), juce::dontSendNotification);
                    });
            }
        }
    };
    
} //namespace fmsmoov