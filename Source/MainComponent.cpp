#include "MainComponent.h"

#include "TestGeneratorWindow.h"
#include "TestGeneratorGUI.h"

//==============================================================================
MainComponent::MainComponent() {

    device_manager = std::make_unique<juce::AudioDeviceManager>();
    audio_source_player = std::make_unique<juce::AudioSourcePlayer>();
   
    const juce::String reg_name = "FMSmoov Settings Registry";
    settings_reg = std::make_unique<SettingsRegistry>(reg_name);

    main_processor = std::make_unique<MainProcessor>(*settings_reg, NUM_CHANNELS);
    test_generator = std::make_unique<TestGenerator>(*settings_reg, NUM_CHANNELS);

    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio, [this](bool granted)
            {
                this->initialize_audio();
            });
    }
    else
    {
        this->initialize_audio();
    }

#if JUCE_MAC
    juce::MenuBarModel::setMacMainMenu(this);
#else
    menu_bar = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(menu_bar.get());
#endif

    main_proc_ui = std::make_unique<MainProcessorUI>(*settings_reg);
    addAndMakeVisible(main_proc_ui.get());

    addAndMakeVisible(label_sb_audio_input_device);
    addAndMakeVisible(label_sb_audio_output_device);
    addAndMakeVisible(label_sb_sample_rate);
    addAndMakeVisible(label_sb_buffer_size);
    update_status_bar();

    // Make sure you set the size of the component after
    // you add any child components.
    auto screen_area = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
    float main_win_width = 0.75 * (float)(screen_area.getWidth());
    float main_win_height = 0.75 * (float)(screen_area.getHeight());

    setSize((int)main_win_width, (int)main_win_height);
    button_light_pulse_source->getInstance()->init(settings_reg.get(), *this);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdown_audio();
    device_manager->removeChangeListener(this);
    main_proc_ui.reset();
    settings_reg.reset();
}

void MainComponent::initialize_audio() {
    device_manager->addChangeListener(this);
    std::unique_ptr<juce::XmlElement> dm_loaded = settings_reg->device_setup.createXml();

    device_manager->initialise(2, 2, dm_loaded.get(), false);

    main_loop_tmpbuf = std::make_unique<juce::AudioBuffer<float>>();

    main_mixer = std::make_unique<juce::MixerAudioSource>();

    main_mixer->addInputSource(main_processor.get(), false);
    main_mixer->addInputSource(test_generator.get(), false);

    audio_source_player->setSource(this);
    device_manager->addAudioCallback(audio_source_player.get());

}

void MainComponent::shutdown_audio() {
    main_mixer->removeAllInputs();
    device_manager->removeAudioCallback(audio_source_player.get());
    audio_source_player->setSource(nullptr);
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    main_loop_tmpbuf->setSize(NUM_CHANNELS, samplesPerBlockExpected);
    main_loop_tmpbuf_info.buffer = main_loop_tmpbuf.get();
    main_loop_tmpbuf_info.startSample = 0;
    main_loop_tmpbuf_info.numSamples = samplesPerBlockExpected;

    main_mixer->prepareToPlay(samplesPerBlockExpected, sampleRate);

}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (settings_reg->get_main_bypass()) {
        return;
    }

    main_loop_tmpbuf->clear();

    main_mixer->getNextAudioBlock(main_loop_tmpbuf_info);

    for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
        bufferToFill.buffer->addFrom(
            chan,
            bufferToFill.startSample,
            *(main_loop_tmpbuf_info.buffer),
            chan,
            0,
            bufferToFill.numSamples
        );
    }

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    test_generator->releaseResources();
    main_processor->releaseResources();
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

#if !JUCE_MAC
    menu_bar->setBounds(area.removeFromTop(juce::LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
#endif
    
    /* Layout the status bar along the bottom with the current audio device selections */
    juce::FlexBox fb_sb;
    fb_sb.flexDirection = juce::FlexBox::Direction::rowReverse;
    fb_sb.alignItems = juce::FlexBox::AlignItems::flexEnd;
    fb_sb.justifyContent = juce::FlexBox::JustifyContent::flexStart;

    auto font = label_sb_audio_input_device.getFont();
    float text_width_input_device = font.getStringWidthFloat(label_sb_audio_input_device.getText()) + 10.0f;
    float text_width_output_device = font.getStringWidthFloat(label_sb_audio_output_device.getText()) + 10.0f;
    float text_width_sample_rate = font.getStringWidthFloat(label_sb_sample_rate.getText()) + 10.0f;
    float text_width_buffer_size = font.getStringWidthFloat(label_sb_buffer_size.getText()) + 10.0f;


    label_sb_audio_output_device.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    label_sb_audio_output_device.setColour(juce::Label::outlineColourId, juce::Colours::white);
    label_sb_audio_input_device.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    label_sb_audio_input_device.setColour(juce::Label::outlineColourId, juce::Colours::white);
    label_sb_sample_rate.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    label_sb_sample_rate.setColour(juce::Label::outlineColourId, juce::Colours::white);
    label_sb_buffer_size.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    label_sb_buffer_size.setColour(juce::Label::outlineColourId, juce::Colours::white);

    fb_sb.items.add(juce::FlexItem(label_sb_buffer_size).withWidth(text_width_buffer_size).withMinHeight(25.0f).withMargin({ 5, -1, 5, 5 }));
    fb_sb.items.add(juce::FlexItem(label_sb_sample_rate).withWidth(text_width_sample_rate).withMinHeight(25.0f).withMargin({ 5, 0, 5, 5 }));
    fb_sb.items.add(juce::FlexItem(label_sb_audio_output_device).withWidth(text_width_output_device).withMinHeight(25.0f).withMargin({ 5, 0, 5, 5 }));
    fb_sb.items.add(juce::FlexItem(label_sb_audio_input_device).withWidth(text_width_input_device).withMinHeight(25.0f).withMargin({ 5, 0, 5, 5 }));

    fb_sb.performLayout(area);
    area.removeFromBottom(35); //TODO: figure out how to automatically get the height of the status bar
    main_proc_ui->setBounds(area);
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == device_manager.get()) {
        update_status_bar();
    }
}

juce::StringArray MainComponent::getMenuBarNames() {
    return { "File", "Audio Devices", "Tools" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) {
    juce::PopupMenu menu;
    if (menuName == "File") {
        menu.addItem(1, "Quit");
    }
    else if (menuName == "Audio Devices") {
        menu.addItem(100, "Setup", true, false);
    }
    else if (menuName == "Tools") {
        menu.addItem(200, "Test Generator", true, false);
    }
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex) {
    if (menuItemID == 1) {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
    else if (menuItemID == 100) {
        if (audio_device_selector == nullptr) {
            audio_device_selector = std::make_unique<AudioDeviceSelectorWindow>("FMSmoov Audio Device Selector", *device_manager);
            audio_device_selector->on_close = [this]() {audio_device_selector.reset(); };
        }
        else {
            audio_device_selector->toFront(true);
        }
    }
    else if (menuItemID == 200) {
        if (test_generator_window == nullptr) {
            auto* content = new TestGeneratorGUI(*settings_reg);
            test_generator_window = std::make_unique<TestGeneratorWindow>("FMSmoov Test Generator", content);
            test_generator_window->on_close = [this]() {test_generator_window.reset(); };
            content->on_done_clicked = [this]() { test_generator_window.reset(); };
            test_generator_window->setSize(800, 600);
            test_generator_window->centreAroundComponent(getTopLevelComponent(), 800, 600);
        }
        else {
            test_generator_window->toFront(true);
        }
    }
}

void MainComponent::componentBeingDeleted(juce::Component& component) {
    
}

void MainComponent::update_status_bar() {
    auto setup = device_manager->getAudioDeviceSetup();

    juce::String input_device = setup.inputDeviceName.isEmpty() ? "NO INPUT DEVICE" : setup.inputDeviceName;
    juce::String output_device = setup.outputDeviceName.isEmpty() ? "NO OUTPUT DEVICE" : setup.outputDeviceName;
    juce::String sample_rate = juce::String(setup.sampleRate);
    juce::String buffer_size = juce::String(setup.bufferSize);

    label_sb_audio_input_device.setText("Input: " + input_device, juce::dontSendNotification);
    label_sb_audio_output_device.setText("Output: " + output_device, juce::dontSendNotification);
    label_sb_sample_rate.setText("Sample Rate: " + sample_rate, juce::dontSendNotification);
    label_sb_buffer_size.setText("Buffer Size: " + buffer_size, juce::dontSendNotification);
}

void MainComponent::initialize_system_animation_pulse() {
    
}