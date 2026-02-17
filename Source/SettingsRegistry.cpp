#include "SettingsRegistry.h"

SettingsRegistry::SettingsRegistry(const juce::String& _reg_name) : reg_name(_reg_name),
    state("MAINAPP"), device_setup("DEVICESETUP") {

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
    if (!state.hasProperty("main_bypass")) {
        state.setProperty("main_bypass", false, nullptr);
    }

    if (!state.hasProperty("processor_bypass")) {
        state.setProperty("processor_bypass", false, nullptr);
    }

    if (!state.hasProperty("generator_state")) {
        state.setProperty("generator_state", false, nullptr);
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
  

	update_atomics();
    dump_settings();

    state.addListener(this);
    device_setup.addListener(this);
    startTimer(30000);
}

SettingsRegistry::~SettingsRegistry() {
    stopTimer();
    save_settings();
}

void SettingsRegistry::update_atomics() {

    main_bypass.store(state.getProperty("main_bypass", false));
    processor_bypass.store(state.getProperty("processor_bypass", false));
    generator_state.store(state.getProperty("generator_state", false));

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
    if (ident == juce::Identifier("main_bypass")) {
        main_bypass.store(vt[ident]);
        main_proc_settings_listeners.call([&](MainProcSettingsListener& l)
            {
                l.main_bypass_changed(main_bypass.load());
            });
    }

    if (ident == juce::Identifier("processor_bypass")) {
        processor_bypass.store(vt[ident]);
        main_proc_settings_listeners.call([&](MainProcSettingsListener& l)
            {
                l.processor_bypass_changed(processor_bypass.load());
            });
    }

    if (ident == juce::Identifier("generator_state")) {
        generator_state.store(vt[ident]);
        main_proc_settings_listeners.call([&](MainProcSettingsListener& l)
            {
                l.generator_state_changed(generator_state.load());
            });
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

    update_atomics();
    needs_update.store(true);

    /*
    * This is kind of hokey, we want to detemine which listeners to notify based on the
    * property that has changed...
    */

    juce::String substr = ident.toString();
    substr = substr.substring(0, 3);

    if (substr == "gen") {
        test_gen_settings_listeners.call([&](TestGenSettingsListener& l)
            {
                l.gen_params_changed(ident.toString().substring(0,5));
            });
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

void SettingsRegistry::set_gen_params(const juce::String& gen_num, fmsmoov::GEN_TYPE type, float freq, float ampl, bool enable) {
    juce::String prop_prefix = gen_num;
    juce::String gen_type_prop = gen_num + "_type";
    juce::String gen_freq_prop = gen_num + "_freq";
    juce::String gen_ampl_prop = gen_num + "_ampl";
    juce::String gen_enable_prop = gen_num + "_enable";

    state.setProperty(gen_type_prop, static_cast<int>(type), nullptr);
    state.setProperty(gen_freq_prop, freq, nullptr);
    state.setProperty(gen_ampl_prop, ampl, nullptr);
    state.setProperty(gen_enable_prop, enable, nullptr);
}

void SettingsRegistry::get_gen_params(const juce::String& gen_num, fmsmoov::GEN_TYPE& type, float& freq, float& ampl, bool& enable) {
    juce::String prop_prefix = gen_num;
    juce::String gen_type_prop = gen_num + "_type";
    juce::String gen_freq_prop = gen_num + "_freq";
    juce::String gen_ampl_prop = gen_num + "_ampl";
    juce::String gen_enable_prop = gen_num + "_enable";

    type = static_cast<fmsmoov::GEN_TYPE>((int)(state.getProperty(gen_type_prop)));
    freq = state.getProperty(gen_freq_prop);
    ampl = state.getProperty(gen_ampl_prop);
    enable = state.getProperty(gen_enable_prop);
}

void SettingsRegistry::set_generator_state(bool mute_all_gens) {
    bool existing_mute_all_gens = state.getProperty("mute_all_gens");
    if (existing_mute_all_gens != mute_all_gens) {
        state.setProperty("mute_all_gens", mute_all_gens, nullptr);
        update_atomics();
        main_proc_settings_listeners.call([&](MainProcSettingsListener& l)
            {
                l.generator_state_changed(mute_all_gens);
            });
    }
}

void SettingsRegistry::set_main_bypass(bool main_bypass) {
    bool existing_main_bypass = state.getProperty("main_bypass");
    if (existing_main_bypass != main_bypass) {
        state.setProperty("main_bypass", main_bypass, nullptr);
        update_atomics();
    }
}

void SettingsRegistry::set_processor_bypass(bool processor_bypass) {
    state.setProperty("processor_bypass", processor_bypass, nullptr);
}

bool SettingsRegistry::get_main_bypass() {
    return main_bypass.load();
}

bool SettingsRegistry::get_processor_bypass() {
    return processor_bypass.load();
}

bool SettingsRegistry::get_generator_state() {
    return generator_state.load();
}

fmsmoov::GEN_TYPE SettingsRegistry::get_gen01_type() {
    return gen01_type.load();
}

fmsmoov::GEN_TYPE SettingsRegistry::get_gen02_type() {
    return gen02_type.load();
}

fmsmoov::GEN_TYPE SettingsRegistry::get_gen03_type() {
    return gen03_type.load();
}

float SettingsRegistry::get_gen01_freq() {
    return gen01_freq.load();
}

float SettingsRegistry::get_gen02_freq() {
    return gen02_freq.load();
}

float SettingsRegistry::get_gen03_freq() {
    return gen03_freq.load();
}

float SettingsRegistry::get_gen01_ampl() {
    return gen01_ampl.load();
}

float SettingsRegistry::get_gen02_ampl() {
    return gen02_ampl.load();
}

float SettingsRegistry::get_gen03_ampl() {
    return gen03_ampl.load();
}

bool SettingsRegistry::get_gen01_enable() {
    return gen01_enable.load();
}

bool SettingsRegistry::get_gen02_enable() {
    return gen02_enable.load();
}

bool SettingsRegistry::get_gen03_enable() {
    return gen03_enable.load();
}

float SettingsRegistry::get_sample_rate() {
    return sample_rate.load();
}