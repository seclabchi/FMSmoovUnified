#pragma once

#include <JuceHeader.h>
#include "SettingsRegistry.h"

class AudioDeviceSelectorWindow : public juce::DocumentWindow
{
    public:
        AudioDeviceSelectorWindow(const juce::String& name, juce::AudioDeviceManager& manager);
        virtual ~AudioDeviceSelectorWindow();
        void closeButtonPressed() override;
        std::function<void()> on_close;
    private:
        std::unique_ptr<juce::AudioDeviceSelectorComponent> audio_device_selector;
        juce::AudioDeviceManager& device_manager;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioDeviceSelectorWindow)
};

