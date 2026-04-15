#pragma once

#include <JuceHeader.h>

namespace fmsmoov {
    
    class BassCenteringBackend;

    class BassCentering : public juce::Component
    {
    public:
        BassCentering();
        virtual ~BassCentering();
        void paint(juce::Graphics&) override {};
        void resized() override {};

        void prepare(const juce::dsp::ProcessSpec spec);
        void update(const juce::dsp::AudioBlock<float>& block);
        void process(juce::dsp::ProcessContextReplacing<float>& context);

    private:
        std::unique_ptr<BassCenteringBackend> backend;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BassCentering)
    };

} //namespace fmsmoov
