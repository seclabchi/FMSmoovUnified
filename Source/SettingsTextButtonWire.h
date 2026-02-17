#pragma once

#include <JuceHeader.h>

namespace fmsmoov {

    struct SettingsTextButtonWire : private juce::ValueTree::Listener {
        SettingsTextButtonWire(juce::ValueTree& vt, const juce::Identifier& prop, juce::TextButton& tb) :
            tree(vt), property(prop), text_button(tb) {

            tree.addListener(this);
            tb.setClickingTogglesState(true);
            text_button.onClick = [this] {
                bool new_state = text_button.getToggleState();

                if (new_state != (bool)(tree.getProperty(property))) {
                    tree.setProperty(property, text_button.getToggleState(), nullptr);
                }
                
                };
            text_button.setToggleState(tree.getProperty(property), juce::dontSendNotification);
            
        }

        ~SettingsTextButtonWire() override { tree.removeListener(this); }

    private:
        juce::ValueTree tree;
        juce::Identifier property;
        juce::TextButton& text_button;

        void valueTreePropertyChanged(juce::ValueTree& v, const juce::Identifier& ident) override {
            if (ident == property) {
                juce::MessageManager::callAsync([this] {
                    text_button.setToggleState(tree.getProperty(property), juce::dontSendNotification);
                    });
            }
        }
    };
    
} //namespace fmsmoov