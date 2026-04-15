#pragma once

#include <JuceHeader.h>

namespace fmsmoov {
    
    class PhaseRotatorBackend;

    class PhaseRotator : public juce::Component
    {
    public:
        PhaseRotator();
        virtual ~PhaseRotator();
        void paint(juce::Graphics&) override {};
        void resized() override {};

        void prepare(const juce::dsp::ProcessSpec spec);
        void update(const juce::dsp::AudioBlock<float>& block);
        void process(juce::dsp::ProcessContextReplacing<float>& context);

    private:
        std::unique_ptr<PhaseRotatorBackend> backend;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhaseRotator)
    };

} //namespace fmsmoov
