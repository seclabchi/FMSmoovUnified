#include "LufsEngine.h"



namespace fmsmoov {

    LufsEngine::LufsEngine() : libebu_state(nullptr) {

    }

    LufsEngine::~LufsEngine() {

    }

    void LufsEngine::prepare(const juce::dsp::ProcessSpec& spec) {

        sample_rate = (float)(spec.sampleRate);
        buffer_size = (float)(spec.maximumBlockSize);

        int mode = EBUR128_MODE_M | EBUR128_MODE_S | EBUR128_MODE_TRUE_PEAK;

        if (nullptr != libebu_state) {
            ebur128_destroy(&libebu_state);
            libebu_state = nullptr;
        }

        libebu_state = ebur128_init(spec.numChannels, spec.sampleRate, mode);
        
        if (interleaved_buffer.size() < spec.numChannels * spec.maximumBlockSize) {
            interleaved_buffer.resize(spec.numChannels * spec.maximumBlockSize);
        }

        /* We don't want to slam the ebur128 library with updates too fast if the 
         * audio input device buffer size is small.  Limit the updates to a maximum
         * of 50 ms.
         */

    }

    void LufsEngine::update(const juce::dsp::AudioBlock<const float>& block) {

        const int num_channels = block.getNumChannels();
        const int num_samples = block.getNumSamples();

        // 1. Safety check for the state
        const juce::ScopedLock sl(state_lock);
        jassert(nullptr != libebu_state);

        // 3. Interleave directly from the block
        for (int s = 0; s < num_samples; ++s) {
            for (int ch = 0; ch < num_channels; ++ch) {
                interleaved_buffer[s * num_channels + ch] = block.getSample(ch, s);
            }
        }
        
        int rc = 0;

        // 4. Update the LUFS state
        rc = ebur128_add_frames_float(libebu_state, interleaved_buffer.data(), (size_t)num_samples);
        jassert(rc == 0);

        double tmp_M;
        rc = ebur128_loudness_momentary(libebu_state, &tmp_M);
        jassert(rc == 0);
        M.store(tmp_M);

        /* 
         * TODO: Fix this so that it only kicks in for high sample rates and small buffer sizes.
         */
        if (++rate_counter >= 3) { // ~9ms at 96kHz/288 samples
            double tmp_S;
            rc = ebur128_loudness_shortterm(libebu_state, &tmp_S);
            jassert(rc == 0);
            S.store(tmp_S);

            double tmp_TP_L;
            double tmp_TP_R;
            rc = ebur128_true_peak(libebu_state, 0, &tmp_TP_L);
            jassert(rc == 0);
            rc = ebur128_true_peak(libebu_state, 1, &tmp_TP_R);
            jassert(rc == 0);
            TP_L.store(tmp_TP_L);
            TP_R.store(tmp_TP_R);

            rate_counter = 0;
        }
    }

    float LufsEngine::get_M() {
        return M.load();
    }

    float LufsEngine::get_S() {
        return S.load();
    }

    float LufsEngine::get_TP_L() {
        return TP_L.load();
    }

    float LufsEngine::get_TP_R() {
        return TP_R.load();
    }

    void LufsEngine::get_lufs(float& _M, float& _S) {
        _M = M.load();
        _S = S.load();
    }

    LufsDataSource& LufsEngine::get_data_src() {
        return *this;
    }

} //namespace fmsmoov