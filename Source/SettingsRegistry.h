#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"
#include "SettingsSliderWire.h"
#include "SettingsToggleButtonWire.h"
#include "SettingsTextButtonWire.h"
#include "SettingsComboBoxWire.h"

/*
* TODO: Refactor gen params into a struct and a sub-value-tree
*/


class SettingsRegistry : private juce::ValueTree::Listener, private juce::Timer
{
public:
    SettingsRegistry(const juce::String& _reg_name);
    virtual ~SettingsRegistry();

    juce::ValueTree device_setup;

    class MainProcSettingsListener {
    public:
        virtual ~MainProcSettingsListener() = default;
        virtual void main_bypass_changed(bool master_bypass) = 0;
        virtual void generator_state_changed(bool new_mute_all_gens) = 0;
        virtual void processor_bypass_changed(bool new_mute_processor) = 0;
    };

    class TestGenSettingsListener {
    public:
        virtual ~TestGenSettingsListener() = default;
        virtual void gen_params_changed(const juce::String& gen_num) = 0;
    };

    /*
    * These setters/getters are connected to the valuetrees and are intended for the UI components.  For the audio
    * engine, use the atomic getters and setters
    */

    void set_gen_params(const juce::String& gen_num, fmsmoov::GEN_TYPE type, float freq, float ampl, bool enable);
    void get_gen_params(const juce::String& gen_num, fmsmoov::GEN_TYPE& type, float& freq, float& ampl, bool& enable);
    void set_generator_state(bool mute_all_gens);
    void set_main_bypass(bool master_bypass);
    void set_processor_bypass(bool processor_bypass);

    /*
    * These getters read the atomic values and are intended for the audio engine.  For the UI
    * components, use the "wiring" that talks directly to the underlying value trees.
    */
    bool get_main_bypass();
    bool get_processor_bypass();
    
    fmsmoov::GEN_TYPE get_gen01_type();
    fmsmoov::GEN_TYPE get_gen02_type();
    fmsmoov::GEN_TYPE get_gen03_type();
    float get_gen01_freq();
    float get_gen02_freq();
    float get_gen03_freq();
    float get_gen01_ampl();
    float get_gen02_ampl();
    float get_gen03_ampl();
    bool get_gen01_enable();
    bool get_gen02_enable();
    bool get_gen03_enable();
    bool get_generator_state();

    void flag_update();
    void dump_settings();

    void add_main_proc_settings_listener(MainProcSettingsListener* l) { main_proc_settings_listeners.add(l); }
    void remove_main_proc_settings_listener(MainProcSettingsListener* l) { main_proc_settings_listeners.remove(l); }

    void add_test_gen_settings_listener(TestGenSettingsListener* l) { test_gen_settings_listeners.add(l); }
    void remove_test_gen_settings_listener(TestGenSettingsListener* l) { test_gen_settings_listeners.remove(l); }

    std::unique_ptr<fmsmoov::SettingsSliderWire> create_slider_attachment(juce::String& prop, juce::Slider& s) {
        if (state.hasProperty(prop)) {
            return std::make_unique<fmsmoov::SettingsSliderWire>(state, prop, s);
        }
        else {
            jassert(false);
            return nullptr;
        }
    }

    std::unique_ptr<fmsmoov::SettingsToggleButtonWire> create_toggle_button_attachment(juce::String& prop, juce::ToggleButton& b) {
        if (state.hasProperty(prop)) {
            return std::make_unique<fmsmoov::SettingsToggleButtonWire>(state, prop, b);
        }
        else {
            jassert(false);
            return nullptr;
        }
    }

    std::unique_ptr<fmsmoov::SettingsTextButtonWire> create_text_button_attachment(juce::String& prop, juce::TextButton& b) {
        if (state.hasProperty(prop)) {
            return std::make_unique<fmsmoov::SettingsTextButtonWire>(state, prop, b);
        }
        else {
            jassert(false);
            return nullptr;
        }
    }

    std::unique_ptr<fmsmoov::SettingsComboBoxWire> create_combo_box_attachment(juce::String& prop, juce::ComboBox& b) {
        if (state.hasProperty(prop)) {
            return std::make_unique<fmsmoov::SettingsComboBoxWire>(state, prop, b);
        }
        else {
            jassert(false);
            return nullptr;
        }
    }

private:
    juce::ValueTree state;
    void save_settings();
    void load_settings();

    void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& ident) override;

	const juce::String& reg_name;
    juce::File app_data_dir;
    juce::File settings_file;
	void update_atomics();
    std::atomic<bool> needs_update{ false };
    void timerCallback() override;

    std::atomic<bool> main_bypass{ false };
    std::atomic<bool> processor_bypass{ false };

    std::atomic<fmsmoov::GEN_TYPE> gen01_type{ fmsmoov::GEN_TYPE::SINE };
    std::atomic<fmsmoov::GEN_TYPE> gen02_type{ fmsmoov::GEN_TYPE::SINE };
    std::atomic<fmsmoov::GEN_TYPE> gen03_type{ fmsmoov::GEN_TYPE::SINE };

    std::atomic<float> gen01_freq{ 440.0f };
    std::atomic<float> gen02_freq{ 1000.0f };
    std::atomic<float> gen03_freq{ 5000.0f };

    std::atomic<float> gen01_ampl{ -12.0f };
    std::atomic<float> gen02_ampl{ -12.0f };
    std::atomic<float> gen03_ampl{ -12.0f };

    std::atomic<bool> gen01_enable{ false };
    std::atomic<bool> gen02_enable{ false };
    std::atomic<bool> gen03_enable{ false };

    std::atomic<bool> generator_state{ true };

    juce::ListenerList<MainProcSettingsListener> main_proc_settings_listeners;
    juce::ListenerList<TestGenSettingsListener> test_gen_settings_listeners;

};

