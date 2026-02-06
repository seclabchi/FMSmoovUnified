#pragma once

#include <JuceHeader.h>

#include "fmsmoov_types.h"
#include "SettingsRegistry.h"

namespace fmsmoov {

    class OscillatorSource :
        public juce::AudioSource,
        public juce::ValueTree::Listener
    {
    public:
        OscillatorSource(SettingsRegistry& _settings_reg, const juce::String& _component_name, uint32_t _num_channels);
        virtual ~OscillatorSource();
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;
        void valueTreePropertyChanged(juce::ValueTree& vt, const juce::Identifier& p) override;
        void set_must_clear_buffer(bool _must_clear_buffer);

    private:
        SettingsRegistry& settings_reg;
        uint32_t num_channels;

        std::unique_ptr<juce::dsp::Oscillator<float>> osc;
        std::unique_ptr<juce::dsp::Gain<float>> gain;

        std::atomic<float> freq{ 440.0f };
        std::atomic<float> ampl{ -12.0f };
        std::atomic<fmsmoov::GEN_TYPE> type{ fmsmoov::GEN_TYPE::SINE };
        std::atomic<fmsmoov::GEN_TYPE> type_current{ fmsmoov::GEN_TYPE::SINE };
        void set_type(fmsmoov::GEN_TYPE _type);

        juce::AudioBuffer<float> tmp_buf;
        std::atomic<bool> must_clear_buffer{ true };

        juce::String component_name;
        juce::String gen_type_propname;
        juce::String frequency_propname;
        juce::String amplitude_propname;
        juce::String enable_propname;

        juce::Random random;

        float pink_b0{ 0.0f };
        float pink_b1{ 0.0f };
        float pink_b2{ 0.0f };
        float pink_b3{ 0.0f };
        float pink_b4{ 0.0f };
        float pink_b5{ 0.0f };
        float pink_b6{ 0.0f };

    };

} //namespace fmsmoov
