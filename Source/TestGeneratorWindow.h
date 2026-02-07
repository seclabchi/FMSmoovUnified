#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"
#include "TestGeneratorGUI.h"

class TestGeneratorWindow : public juce::DocumentWindow
{
    public:
        TestGeneratorWindow(const juce::String& name, juce::Component* content_component);
        virtual ~TestGeneratorWindow();
        void closeButtonPressed() override;
        std::function<void()> on_close;
    private:

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestGeneratorWindow)
};

