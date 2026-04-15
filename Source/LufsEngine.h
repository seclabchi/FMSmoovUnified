#pragma once

#include <JuceHeader.h>
#include "ebur128.h"

namespace fmsmoov {

    class LufsDataSource {
    public:
        ~LufsDataSource() = default;
        virtual void get_lufs(float& _M, float& _S) = 0;
    };

	class LufsEngine : public juce::Component, LufsDataSource
	{
    public:
        LufsEngine();
        virtual ~LufsEngine();
        void LufsEngine::prepare(const juce::dsp::ProcessSpec& spec);
        void LufsEngine::update(const juce::dsp::AudioBlock<const float>& block);
        float get_M();
        float get_S();
        float get_TP_L();
        float get_TP_R();
        void get_lufs(float& _M, float& _S) override;
        LufsDataSource& get_data_src();

    private:
        std::atomic<float> M;
        std::atomic<float> S;
        std::atomic<float> TP_L;
        std::atomic<float> TP_R;

        ebur128_state* libebu_state;
        std::vector<float> interleaved_buffer;

        juce::CriticalSection state_lock;
        int current_channels = 0;

        uint32_t rate_counter = 0;
        float sample_rate = 0.0f;
        float buffer_size = 0.0f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LufsEngine)
	};
} //namespace fmsmoov
