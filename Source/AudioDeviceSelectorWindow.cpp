#include "AudioDeviceSelectorWindow.h"

AudioDeviceSelectorWindow::AudioDeviceSelectorWindow(const juce::String& name, juce::AudioDeviceManager& manager) :
    device_manager(manager),
    DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::allButtons) {

    audio_device_selector = std::make_unique<juce::AudioDeviceSelectorComponent>(
        device_manager,
        2, 2,
        2, 2,
        false, false,
        true, false
    );

    const int preferred_width = 800;
    const int preferred_height = 600;

    this->setSize(800, 600);

    auto device_manager_setup = device_manager.getAudioDeviceSetup();

    setContentNonOwned(audio_device_selector.get(), true);
    setResizable(true, true);
    setUsingNativeTitleBar(true);
    setVisible(true);
}

AudioDeviceSelectorWindow::~AudioDeviceSelectorWindow() {

}

void AudioDeviceSelectorWindow::closeButtonPressed() {
    on_close();
}