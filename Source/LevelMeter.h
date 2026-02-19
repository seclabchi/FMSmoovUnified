#pragma once

#include <JuceHeader.h>

namespace fmsmoov {
    
    class LevelMeterBackend;

    class LevelMeter : public juce::Component, private juce::Timer
    {
    public:
        LevelMeter();
        virtual ~LevelMeter();
        void paint(juce::Graphics&) override;
        void resized() override;

        void prepare(const juce::dsp::ProcessSpec spec);
        void update(const juce::dsp::AudioBlock<float>& block);

    private:
        void timerCallback() override;
        std::unique_ptr<LevelMeterBackend> backend;

        juce::Image bg_img;

        /* Meter limits (in dB) */
        float meter_max{ +6.0f };
        float meter_min{ -60.0f };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
    };

} //namespace fmsmoov
