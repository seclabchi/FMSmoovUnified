#include "AudioDeviceSelectorWindow.h"

AudioDeviceSelectorWindow::AudioDeviceSelectorWindow(const juce::String& name, juce::AudioDeviceManager& manager) :
    device_manager(manager),
    DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::closeButton) {

    audio_device_selector = std::make_unique<juce::AudioDeviceSelectorComponent>(
        device_manager,
        2, 2,
        2, 2,
        false, false,
        true, false
    );

    const int preferred_width = 800;
    const int preferred_height = 600;
    audio_device_selector->setSize(800, 600);
    auto* main_window = juce::Desktop::getInstance().getComponent(0);
    auto main_bounds = main_window->getScreenBounds();
    this->centreAroundComponent(main_window, 800, 600);

    setContentNonOwned(audio_device_selector.get(), true);
    setResizable(false, false);
    setUsingNativeTitleBar(false);
    setVisible(true);
}

AudioDeviceSelectorWindow::~AudioDeviceSelectorWindow() {

}

void AudioDeviceSelectorWindow::closeButtonPressed() {
    on_close();
}