#pragma once

#include <JuceHeader.h>
#include "LufsEngine.h"

namespace fmsmoov {
   
    class LoudnessMeterBackend;

    class LoudnessMeter : public juce::Component, private juce::Timer
    {
    public:
        LoudnessMeter();
        virtual ~LoudnessMeter();
        void paint(juce::Graphics&) override;
        void resized() override;

        void prepare(const juce::dsp::ProcessSpec spec);
        void update(const juce::dsp::AudioBlock<float>& block);

        LufsDataSource& get_lufs_source();

    private:
        void timerCallback() override;
        std::unique_ptr<LoudnessMeterBackend> backend;

        juce::Image bg_img;
        juce::Colour meter_rms_color;
        juce::Colour meter_peak_color;

        /* Meter limits (in dB) */
        float meter_max{ +6.0f };
        float meter_min{ -60.0f };
        int text_offset = 30;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoudnessMeter)
    };

} //namespace fmsmoov
