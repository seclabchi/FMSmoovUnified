#pragma once

#include <JuceHeader.h>

namespace fmsmoov {
    
    class HPF30HzBackend;

    class HPF30Hz : public juce::Component
    {
    public:
        HPF30Hz();
        virtual ~HPF30Hz();
        void paint(juce::Graphics&) override {};
        void resized() override {};

        void prepare(const juce::dsp::ProcessSpec spec);
        void update(const juce::dsp::AudioBlock<float>& block);
        void process(juce::dsp::ProcessContextReplacing<float>& context);

    private:
        std::unique_ptr<HPF30HzBackend> backend;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HPF30Hz)
    };

} //namespace fmsmoov
