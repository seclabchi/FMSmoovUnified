#include "SettingsRegistry.h"

SettingsRegistry::SettingsRegistry(const juce::String& _reg_name) : reg_name(_reg_name) {
	state.addListener(this);

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
}

SettingsRegistry::~SettingsRegistry() {

}

void SettingsRegistry::update_atomics() {
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

void SettingsRegistry::valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& ident) {
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
}