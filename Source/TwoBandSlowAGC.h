#pragma once

#include <JuceHeader.h>
#include "LufsEngine.h"
#include "CrossoverTwoBand.h"

namespace fmsmoov {

    typedef struct {
        std::atomic<float> min_gr{ 23.0f };
        std::atomic<float> max_gr{ -20.0f };
        std::atomic<float> hysteresis{ 3.0f };
        std::atomic<float> ratio{ 100.0f };
        std::atomic<float> knee_width{ 4.0f };

        std::atomic<float> target{ -18.0f };

        std::atomic<float> drive_input{ 6.0f };
        std::atomic<float> target_lufs_S{ -14.0f };

        std::atomic<float> gate_thresh_lo{ -35.0f };
        std::atomic<float> gate_thresh_hi{ -35.0f };

        std::atomic<float> attack_peak_cf_lo{ 20.0f };
        std::atomic<float> attack_rms_cf_lo{ 200.0f };
        std::atomic<float> release_peak_cf_lo{ 200.0f };
        std::atomic<float> release_rms_cf_lo{ 300.0f };

        std::atomic<float> attack_peak_cf_hi{ 5.0f };
        std::atomic<float> attack_rms_cf_hi{ 100.0f };
        std::atomic<float> release_peak_cf_hi{ 100.0f };
        std::atomic<float> release_rms_cf_hi{ 300.0f };

        std::atomic<float> cf_smoothing_time{ 300.0f };

    } TWO_BAND_SLOW_AGC_SETTINGS;
    
    class TwoBandSlowAGCBackend;

    class TwoBandSlowAGC : public juce::Component, private juce::Timer
    {
    public:
        TwoBandSlowAGC(fmsmoov::LufsDataSource& lufs_src, fmsmoov::CrossoverTwoBandDataSource& data_src);
        virtual ~TwoBandSlowAGC();

        float get_current_gr_lo_db();
        float get_current_gr_hi_db();

        void paint(juce::Graphics&) override;
        void resized() override;

        void prepare(const juce::dsp::ProcessSpec spec);
        void update(const juce::dsp::AudioBlock<float>& block);
        void process(juce::dsp::ProcessContextReplacing<float>& context);
    private:
        void timerCallback() override;

        juce::Image bg_img;
        juce::Colour meter_gr_color{ juce::Colours::pink };
        juce::Colour meter_cf_color{ juce::Colours::yellow };
        juce::Colour meter_lufs_error_color{ juce::Colours::magenta };

        std::unique_ptr<TwoBandSlowAGCBackend> backend;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwoBandSlowAGC)
    };

} //namespace fmsmoov
