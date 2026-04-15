#pragma once

#include <JuceHeader.h>

namespace fmsmoov {

    class CrossoverTwoBandDataSource {
    public:
        virtual ~CrossoverTwoBandDataSource() = default;
        virtual void get_buffers(juce::dsp::AudioBlock<float>& block_lb, juce::dsp::AudioBlock<float>& block_hb) = 0;
    };

    class CrossoverTwoBand : public CrossoverTwoBandDataSource
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::dsp::ProcessContextReplacing<float>& context);
        void get_buffers(juce::dsp::AudioBlock<float>& block_lb, juce::dsp::AudioBlock<float>& block_hb) override;
    private:
        juce::AudioBuffer<float> bufL;
        juce::AudioBuffer<float> bufH;
        using LR4Filter = juce::dsp::LinkwitzRileyFilter<float>;
        LR4Filter filtLoL, filtLoR, filtHiL, filtHiR;
    };

} //namespace fmsmoov

