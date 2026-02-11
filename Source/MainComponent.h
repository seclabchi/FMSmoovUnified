#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"
#include "TestGeneratorWindow.h"
#include "TestGenerator.h"
#include "AudioDeviceSelectorWindow.h"
#include "MainProcessorUI.h"
#include "MainProcessor.h"


#define NUM_CHANNELS 2

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent :
    public juce::Component,
    public juce::AudioSource,
    public juce::MenuBarModel,
    public juce::ValueTree::Listener,
    public juce::ComponentListener,
    public juce::ChangeListener,
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
    void componentBeingDeleted(juce::Component& component) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
protected:
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

private:
    std::unique_ptr<juce::AudioDeviceManager> device_manager;
    std::unique_ptr<juce::AudioSourcePlayer> audio_source_player;

    void initialize_audio();
    void shutdown_audio();

    std::unique_ptr<juce::MixerAudioSource> main_mixer;

    std::unique_ptr<MainProcessorUI> main_proc_ui;
    
    std::unique_ptr<juce::MenuBarComponent> menu_bar;

    std::unique_ptr<SettingsRegistry> settings_reg;
    
    void handleAsyncUpdate() override;

    std::unique_ptr<MainProcessor> main_processor;
    
    std::unique_ptr<TestGenerator> test_generator;
    std::unique_ptr<TestGeneratorWindow> test_generator_window;

    std::unique_ptr<AudioDeviceSelectorWindow> audio_device_selector;

    void update_status_bar();
    juce::Label label_sb_audio_input_device;
    juce::Label label_sb_audio_output_device;
    juce::Label label_sb_sample_rate;
    juce::Label label_sb_buffer_size;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
