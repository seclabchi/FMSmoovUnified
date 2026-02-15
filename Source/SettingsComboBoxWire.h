#pragma once

#include <JuceHeader.h>

namespace fmsmoov {

    struct SettingsComboBoxWire : private juce::ValueTree::Listener {
        SettingsComboBoxWire(juce::ValueTree& vt, const juce::Identifier& prop, juce::ComboBox& cb) :
            tree(vt), property(prop), combo_box(cb) {

            tree.addListener(this);
            combo_box.onChange = [this] {
                int new_sel = combo_box.getSelectedId();

                if (new_sel != (int)(tree.getProperty(property))) {
                    tree.setProperty(property, new_sel, nullptr);
                }
                
                };
            combo_box.setSelectedId(tree.getProperty(property), juce::dontSendNotification);  
        }

        ~SettingsComboBoxWire() override { tree.removeListener(this); }

    private:
        juce::ValueTree tree;
        juce::Identifier property;
        juce::ComboBox& combo_box;

        void valueTreePropertyChanged(juce::ValueTree& v, const juce::Identifier& ident) override {
            if (ident == property) {
                juce::MessageManager::callAsync([this] {
                    combo_box.setSelectedId(tree.getProperty(property), juce::dontSendNotification);
                    });
            }
        }
    };
    
} //namespace fmsmoov