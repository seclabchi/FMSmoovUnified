#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"
#include "TestGenerator.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : 
    public juce::AudioAppComponent, 
    public juce::MenuBarModel, 
    public juce::ValueTree::Listener,
    private juce::AsyncUpdater
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;

protected:
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

private:
    //==============================================================================
    // Your private member variables go here...
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audio_device_selector;

    std::unique_ptr<juce::MenuBarComponent> menu_bar;

    std::atomic<bool> is_tone_on;

    SettingsRegistry* settings_reg;
    juce::File app_data_dir;
    juce::File settings_file;
    void save_settings();
    void handleAsyncUpdate() override;
    
    TestGenerator* test_generator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
