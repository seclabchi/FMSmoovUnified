#pragma once

#include <JuceHeader.h>

namespace fmsmoov {

    struct SettingsToggleButtonWire : private juce::ValueTree::Listener {
        SettingsToggleButtonWire(juce::ValueTree& vt, const juce::Identifier& prop, juce::ToggleButton& b) :
            tree(vt), property(prop), toggle_button(b) {

            tree.addListener(this);
            b.setClickingTogglesState(true);
            toggle_button.onClick = [this] {
                bool new_state = toggle_button.getToggleState();

                if (new_state != (bool)(tree.getProperty(property))) {
                    tree.setProperty(property, toggle_button.getToggleState(), nullptr);
                }
                
                };
            toggle_button.setToggleState(tree.getProperty(property), juce::dontSendNotification);
            
        }

        ~SettingsToggleButtonWire() override { tree.removeListener(this); }

    private:
        juce::ValueTree tree;
        juce::Identifier property;
        juce::ToggleButton& toggle_button;

        void valueTreePropertyChanged(juce::ValueTree& v, const juce::Identifier& ident) override {
            if (ident == property) {
                juce::MessageManager::callAsync([this] {
                    toggle_button.setToggleState(tree.getProperty(property), juce::dontSendNotification);
                    });
            }
        }
    };
    
} //namespace fmsmoov