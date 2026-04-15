#include "HPF30Hz.h"
#include <JuceHeader.h>

namespace fmsmoov {

    class HPF30HzBackend {

    public:
        

        void prepare(const juce::dsp::ProcessSpec& spec) {
            auto spec_mod = spec;
            spec_mod.numChannels = 1;

            filtL.prepare(spec_mod);
            filtR.prepare(spec_mod);

            filt_coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec_mod.sampleRate, 30.0f, 0.707f);
            filtL.get<0>().coefficients = filt_coeffs;
            filtL.get<1>().coefficients = filt_coeffs;
            filtR.get<0>().coefficients = filt_coeffs;
            filtR.get<1>().coefficients = filt_coeffs;
        }

        void process(juce::dsp::ProcessContextReplacing<float>& context) {
            juce::ScopedNoDenormals noDenormals;

            auto& block = context.getOutputBlock();
            const auto numSamples = block.getNumSamples();

            auto blockL = block.getSingleChannelBlock(0);
            juce::dsp::ProcessContextReplacing<float> ctxL(blockL);
            filtL.process(ctxL);

            if (block.getNumChannels() > 1)
            {
                auto blockR = block.getSingleChannelBlock(1);
                juce::dsp::ProcessContextReplacing<float> ctxR(blockR);
                filtR.process(ctxR);
            }
            
        }

        void update(const juce::dsp::AudioBlock<float>&) {}

    private:
        using FilterStage = juce::dsp::IIR::Filter<float>;
        using FilterChain = juce::dsp::ProcessorChain<FilterStage, FilterStage>;
        FilterChain filtL, filtR;
        juce::dsp::IIR::Coefficients<float>::Ptr filt_coeffs;
    };

    HPF30Hz::HPF30Hz() : backend(std::make_unique<HPF30HzBackend>()) {
    }

    HPF30Hz::~HPF30Hz() {
        backend.reset();
    }

    void HPF30Hz::prepare(const juce::dsp::ProcessSpec spec) {
        backend->prepare(spec);
    }

    void HPF30Hz::update(const juce::dsp::AudioBlock<float>& buffer) {
        backend->update(buffer);
    }

    void HPF30Hz::process(juce::dsp::ProcessContextReplacing<float>& context) {
        backend->process(context);
    }

}