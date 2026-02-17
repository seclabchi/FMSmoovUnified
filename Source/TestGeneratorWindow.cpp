#include "TestGeneratorWindow.h"

TestGeneratorWindow::TestGeneratorWindow(const juce::String& name, juce::Component* content_component) :
    DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::closeButton) {

    setContentOwned(content_component, true);
    setResizable(false, false);
    setUsingNativeTitleBar(false);
    setVisible(true);
}

TestGeneratorWindow::~TestGeneratorWindow() {
    
}

void TestGeneratorWindow::closeButtonPressed() {
    on_close();
}