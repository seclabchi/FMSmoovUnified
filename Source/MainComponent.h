#pragma once

#include <JuceHeader.h>

#include "SettingsRegistry.h"
#include "TestGeneratorWindow.h"
#include "TestGenerator.h"
#include "AudioDeviceSelectorWindow.h"
#include "MainProcessor.h"
#include "ButtonLightPulseSource.h"


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
    public juce::ComponentListener,
    public juce::ChangeListener {
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

    
    
    std::unique_ptr<juce::MenuBarComponent> menu_bar;

    std::unique_ptr<SettingsRegistry> settings_reg;
    
    std::unique_ptr<juce::AudioBuffer<float>> main_loop_tmpbuf;
    juce::AudioSourceChannelInfo main_loop_tmpbuf_info;
    std::unique_ptr<MainProcessor> main_processor;
    
    std::unique_ptr<TestGenerator> test_generator;
    std::unique_ptr<TestGeneratorWindow> test_generator_window;

    std::unique_ptr<AudioDeviceSelectorWindow> audio_device_selector;

    void update_status_bar();
    juce::Label label_sb_audio_input_device;
    juce::Label label_sb_audio_output_device;
    juce::Label label_sb_sample_rate;
    juce::Label label_sb_buffer_size;

    juce::TooltipWindow tooltipWindow;  //apparently this just lives for the life of the app globally

    /* 
     * This is the master animator pulse source for all of the lighted buttons in the system
    */

    ButtonLightPulseSource* button_light_pulse_source;
    void initialize_system_animation_pulse();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
