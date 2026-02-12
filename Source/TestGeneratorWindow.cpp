#include "TestGeneratorWindow.h"

TestGeneratorWindow::TestGeneratorWindow(const juce::String& name, juce::Component* content_component) :
    DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::allButtons) {

    setContentOwned(content_component, true);
    setResizable(true, true);
    setUsingNativeTitleBar(true);
    setVisible(true);
}

TestGeneratorWindow::~TestGeneratorWindow() {
    
}

void TestGeneratorWindow::closeButtonPressed() {
    on_close();
}