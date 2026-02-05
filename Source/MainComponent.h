#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"
#include "TestGenerator.h"


#define NUM_CHANNELS 2

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent :
    public juce::AudioAppComponent,
    public juce::MenuBarModel,
    public juce::ValueTree::Listener,
    public juce::ComponentListener,
    private juce::AsyncUpdater,
    private juce::Timer
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
    void timerCallback() override;
    void componentBeingDeleted(juce::Component& component) override;

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

    std::unique_ptr<SettingsRegistry> settings_reg;
    juce::File app_data_dir;
    juce::File settings_file;
    void save_settings();
    void load_settings();
    std::atomic<bool> settings_file_needs_update{ false };
    void handleAsyncUpdate() override;
    
    std::unique_ptr<TestGenerator> test_generator;
    std::unique_ptr<juce::DialogWindow> test_generator_dlg;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
