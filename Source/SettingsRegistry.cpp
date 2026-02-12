#include "SettingsRegistry.h"

SettingsRegistry::SettingsRegistry(const juce::String& _reg_name) : reg_name(_reg_name) {

    app_data_dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    settings_file = app_data_dir.getChildFile("tonekids.com/fmsmoov/fmsmoov_settings.xml");

    if (!settings_file.getParentDirectory().exists()) {
        DBG("Settings directory does not exist. Creating...");
        settings_file.getParentDirectory().createDirectory();
    }

    DBG("Connected to settings file " << settings_file.getFullPathName());
    load_settings();
	

    /********************** AUDIO DEVICE SETUP *************************/
    

    /****************** MAIN PROCESSOR CHAIN **********************/
    if (!state.hasProperty("master_bypass")) {
        state.setProperty("master_bypass", false, nullptr);
    }

    /********************** TEST GENERATOR *************************/

    if (!state.hasProperty("gen01_type")) {
		state.setProperty("gen01_type", static_cast<int>(fmsmoov::GEN_TYPE::SINE), nullptr);
	}
	if (!state.hasProperty("gen02_type")) {
		state.setProperty("gen02_type", static_cast<int>(fmsmoov::GEN_TYPE::SINE), nullptr);
	}
	if (!state.hasProperty("gen03_type")) {
		state.setProperty("gen03_type", static_cast<int>(fmsmoov::GEN_TYPE::SINE), nullptr);
	}
	if (!state.hasProperty("gen01_freq")) {
		state.setProperty("gen01_freq", 440.0f, nullptr);
	}
	if (!state.hasProperty("gen02_freq")) {
		state.setProperty("gen02_freq", 1000.0f, nullptr);
	}
	if (!state.hasProperty("gen03_freq")) {
		state.setProperty("gen03_freq", 5000.0f, nullptr);
	}
	if (!state.hasProperty("gen01_ampl")) {
		state.setProperty("gen01_ampl", -12.0f, nullptr);
	}
	if (!state.hasProperty("gen02_ampl")) {
		state.setProperty("gen02_ampl", -12.0f, nullptr);
	}
	if (!state.hasProperty("gen03_ampl")) {
		state.setProperty("gen03_ampl", -12.0f, nullptr);
	}
	if (!state.hasProperty("gen01_enable")) {
		state.setProperty("gen01_enable", false, nullptr);
	}
	if (!state.hasProperty("gen02_enable")) {
		state.setProperty("gen02_enable", false, nullptr);
	}
	if (!state.hasProperty("gen03_enable")) {
		state.setProperty("gen03_enable", false, nullptr);
	}
    if (!state.hasProperty("all_gens_mute")) {
        state.setProperty("all_gens_mute", false, nullptr);
    }

	update_atomics();
    dump_settings();

    state.addListener(this);
    device_setup.addListener(this);
    startTimer(5000);
}

SettingsRegistry::~SettingsRegistry() {
    stopTimer();
    save_settings();
}

void SettingsRegistry::update_atomics() {

    master_bypass.store(state.getProperty("master_bypass", false));

	gen01_type.store(static_cast<fmsmoov::GEN_TYPE>(static_cast<int>(state.getProperty("gen01_type", 1))));
	gen02_type.store(static_cast<fmsmoov::GEN_TYPE>(static_cast<int>(state.getProperty("gen02_type", 1))));
	gen03_type.store(static_cast<fmsmoov::GEN_TYPE>(static_cast<int>(state.getProperty("gen03_type", 1))));

	gen01_freq.store(state.getProperty("gen01_freq", 440.0f));
	gen02_freq.store(state.getProperty("gen02_freq", 1000.0f));
	gen03_freq.store(state.getProperty("gen03_freq", 5000.0f));

	gen01_ampl.store(state.getProperty("gen01_ampl", -12.0f));
	gen02_ampl.store(state.getProperty("gen02_ampl", -12.0f));
	gen03_ampl.store(state.getProperty("gen03_ampl", -12.0f));

	gen01_enable.store(state.getProperty("gen01_enable", false));
	gen02_enable.store(state.getProperty("gen02_enable", false));
	gen03_enable.store(state.getProperty("gen03_enable", false));

    all_gens_mute.store(state.getProperty("all_gens_mute", false));

	
}

void SettingsRegistry::flag_update() {
    needs_update.store(true);
}

void SettingsRegistry::timerCallback() {
    if (needs_update) {
        save_settings();
        needs_update.store(false);
    }
}

void SettingsRegistry::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& ident) {
    //juce::Logger::writeToLog("Setting changed: " + ident.toString());

    /**************** MAIN PROCESSOR ****************/
    if (ident == juce::Identifier("master_bypass")) {
        master_bypass.store(vt[ident]);
    }

    /*************** TEST GENERATOR ****************/

	if (ident == juce::Identifier("gen01_type")) {
		auto int_val = static_cast<int> (vt.getProperty(ident));
		gen01_type.store(static_cast<fmsmoov::GEN_TYPE>(int_val));
	}

	if (ident == juce::Identifier("gen02_type")) {
		auto int_val = static_cast<int> (vt.getProperty(ident));
		gen02_type.store(static_cast<fmsmoov::GEN_TYPE>(int_val));
	}

	if (ident == juce::Identifier("gen03_type")) {
		auto int_val = static_cast<int> (vt.getProperty(ident));
		gen03_type.store(static_cast<fmsmoov::GEN_TYPE>(int_val));
	}

	if (ident == juce::Identifier("gen01_freq")) {
		gen01_freq.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen02_freq")) {
		gen02_freq.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen03_freq")) {
		gen03_freq.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen01_ampl")) {
		gen01_ampl.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen02_ampl")) {
		gen02_ampl.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen03_ampl")) {
		gen03_ampl.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen01_enable")) {
		gen01_enable.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen02_enable")) {
		gen02_enable.store(vt[ident]);
	}

	if (ident == juce::Identifier("gen03_enable")) {
		gen03_enable.store(vt[ident]);
	}

    if (ident == juce::Identifier("all_gens_mute")) {
        all_gens_mute.store(vt[ident]);
    }
}

void SettingsRegistry::dump_settings() {
    juce::String state_dump = state.createXml()->toString();
    juce::Logger::writeToLog(state_dump);

    juce::String device_setup_dump = device_setup.createXml()->toString();
    juce::Logger::writeToLog(device_setup_dump);
    juce::Logger::writeToLog("dump_settings_complete");
}

void SettingsRegistry::save_settings() {
    DBG("setting save triggered");

    juce::TemporaryFile temp_file(settings_file);

    auto now = juce::Time::getCurrentTime();
    auto hostname = juce::SystemStats::getComputerName();
    auto os_name = juce::SystemStats::getOperatingSystemName();
    auto mem_size = juce::SystemStats::getMemorySizeInMegabytes();
    auto cpu_speed = juce::SystemStats::getCpuSpeedInMegahertz();
    auto cpu_vendor = juce::SystemStats::getCpuVendor();
    auto cpu_model = juce::SystemStats::getCpuModel();

    state.setProperty("last_saved", now.toString(true, true, true, true), nullptr);
    state.setProperty("hostname", hostname, nullptr);
    state.setProperty("os_name", os_name, nullptr);
    state.setProperty("mem_size", mem_size, nullptr);
    state.setProperty("cpu_vendor", cpu_vendor, nullptr);
    state.setProperty("cpu_model", cpu_model, nullptr);

    juce::ValueTree main_settings("SETTINGS");

    dump_settings();

    main_settings.addChild(state.createCopy(), -1, nullptr);
    main_settings.addChild(device_setup.createCopy(), -1, nullptr);

    {
        auto out = std::unique_ptr<juce::FileOutputStream>(temp_file.getFile().createOutputStream());

        if (out) {
            auto xml = main_settings.createXml();

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

void SettingsRegistry::load_settings() {
    if (settings_file.existsAsFile()) {
        if (auto xml = juce::XmlDocument::parse(settings_file)) {
            auto loaded_tree = juce::ValueTree::fromXml(*xml);

            auto state_data = loaded_tree.getChildWithName("MAINAPP");
            state.copyPropertiesAndChildrenFrom(state_data, nullptr);

            auto device_setup_data = loaded_tree.getChildWithName("DEVICESETUP");
            device_setup.copyPropertiesAndChildrenFrom(device_setup_data, nullptr);

            dump_settings();

            if (!(state.isValid())) {
                DBG("SETTINGS REGISTRY IS INVALID.");
            }
            else {
                update_atomics();
            }
        }
    }
}