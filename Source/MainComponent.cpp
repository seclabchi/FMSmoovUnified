#include "MainComponent.h"

#include "TestGeneratorGUI.h"

//==============================================================================
MainComponent::MainComponent() {

    app_data_dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    settings_file = app_data_dir.getChildFile("tonekids.com/fmsmoov/fmsmoov_settings.xml");

    if (!settings_file.getParentDirectory().exists()) {
        DBG("Settings directory does not exist. Creating...");
        settings_file.getParentDirectory().createDirectory();
    }

    DBG("Connected to settings file " << settings_file.getFullPathName());

    const juce::String reg_name = "FMSmoov Settings Registry";
    settings_reg = new SettingsRegistry(reg_name);
    settings_reg->state.addListener(this);

    is_tone_on = false;
    test_generator = new TestGenerator(*settings_reg);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else 
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(this);
#else
    menu_bar = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menu_bar.get());
#endif

    deviceManager.initialiseWithDefaultDevices(2, 2);

    audio_device_selector = std::make_unique<juce::AudioDeviceSelectorComponent>(
        deviceManager,
        2, 2,
        2, 2,
        false, false,
        true, false
    );

    addAndMakeVisible(audio_device_selector.get());

    // Make sure you set the size of the component after
    // you add any child components.
    const uint32_t width = 1024;
    const uint32_t height = 768;

    setSize(width, height);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    delete test_generator;
    delete settings_reg;
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    test_generator->prepareToPlay(samplesPerBlockExpected, sampleRate);

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    test_generator->getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    test_generator->releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    auto area = getLocalBounds();
    juce::Rectangle<int> selector_bounds(0, 0, 500, 250);

#if !JUCE_MAC
    menu_bar->setBounds(area.removeFromTop(juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
#endif

    audio_device_selector->setBounds(selector_bounds.withPosition(5, menu_bar->getBottom() + 5));
}

juce::StringArray MainComponent::getMenuBarNames() {
    return { "File", "Tools" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) {
    juce::PopupMenu menu;
    if (menuName == "File") {
        menu.addItem(1, "Quit");
    }
    else if (menuName == "Tools") {
        // Use 'isToneOn' to show a checkmark in the menu
        menu.addItem(100, "Test Tone", true, is_tone_on);
        menu.addItem(101, "Test Generator", true, false);
    }
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex) {
    if (menuItemID == 1) {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
    else if (menuItemID == 100) {
        is_tone_on = !is_tone_on;
        // Important: this tells the bar to redraw so the checkmark updates!
        this->menuItemsChanged();
    }
    else if (menuItemID == 101) {
        juce::DialogWindow::LaunchOptions options;

        auto* gen_dlg = new TestGeneratorGUI(*settings_reg);
        options.content.setOwned(gen_dlg);

        options.dialogTitle = "Test Generator";
        options.dialogBackgroundColour = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = false;
        options.useBottomRightCornerResizer = false;

        options.launchAsync();
    }
}

void MainComponent::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) {
    DBG("Setting changed: " << p.toString());
    juce::AsyncUpdater::triggerAsyncUpdate();
}

void MainComponent::save_settings() {
    juce::TemporaryFile temp_file(settings_file);

    auto now = juce::Time::getCurrentTime();
    auto hostname = juce::SystemStats::getComputerName();
    auto os_name = juce::SystemStats::getOperatingSystemName();
    auto mem_size = juce::SystemStats::getMemorySizeInMegabytes();
    auto cpu_speed = juce::SystemStats::getCpuSpeedInMegahertz();
    auto cpu_vendor = juce::SystemStats::getCpuVendor();
    auto cpu_model = juce::SystemStats::getCpuModel();

    settings_reg->state.setProperty("last_saved", now.toString(true, true, true, true), nullptr);
    settings_reg->state.setProperty("hostname", hostname, nullptr);
    settings_reg->state.setProperty("os_name", os_name, nullptr);
    settings_reg->state.setProperty("mem_size", mem_size, nullptr);
    settings_reg->state.setProperty("cpu_vendor", cpu_vendor, nullptr);
    settings_reg->state.setProperty("cpu_model", cpu_model, nullptr);

    {
        auto out = std::unique_ptr<juce::FileOutputStream>(temp_file.getFile().createOutputStream());

        if (out) {
            auto xml = settings_reg->state.createXml();

            if (xml) {
                xml->writeTo(*out);
                out->flush();
            }
        }
    } /* Scope wrapper so that the stream is closed before the temp file swaps onto the real file */

    if (!temp_file.overwriteTargetFileWithTemporary()) {
        juce::Logger::writeToLog("Setting file save FAILED.  Couldn't overwrite target with temp file.");
    }

}

void MainComponent::handleAsyncUpdate() {
    save_settings();
}