#include "PhaseRotator.h"
#include <JuceHeader.h>

namespace fmsmoov {

    class PhaseRotatorBackend {

    public:
        void prepare(const juce::dsp::ProcessSpec& spec) {

            auto spec_mod = spec;
            spec_mod.numChannels = 1;

            filtL.prepare(spec_mod);
            filtR.prepare(spec_mod);

            filt_coeffs_s1 = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(spec_mod.sampleRate, 120.0f);
            filt_coeffs_s2 = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(spec_mod.sampleRate, 180.0f);
            filt_coeffs_s3 = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(spec_mod.sampleRate, 240.0f);
            filt_coeffs_s4 = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(spec_mod.sampleRate, 300.0f);

            filtL.get<0>().coefficients = filt_coeffs_s1;
            filtL.get<1>().coefficients = filt_coeffs_s2;
            filtL.get<2>().coefficients = filt_coeffs_s3;
            filtL.get<3>().coefficients = filt_coeffs_s4;
            filtR.get<0>().coefficients = filt_coeffs_s1;
            filtR.get<1>().coefficients = filt_coeffs_s2;
            filtR.get<2>().coefficients = filt_coeffs_s3;
            filtR.get<3>().coefficients = filt_coeffs_s4;
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
        using FilterChain = juce::dsp::ProcessorChain<FilterStage, FilterStage, FilterStage, FilterStage>;
        FilterChain filtL, filtR;
        juce::dsp::IIR::Coefficients<float>::Ptr filt_coeffs_s1;
        juce::dsp::IIR::Coefficients<float>::Ptr filt_coeffs_s2;
        juce::dsp::IIR::Coefficients<float>::Ptr filt_coeffs_s3;
        juce::dsp::IIR::Coefficients<float>::Ptr filt_coeffs_s4;
    };

    PhaseRotator::PhaseRotator() : backend(std::make_unique<PhaseRotatorBackend>()) {
    }

    PhaseRotator::~PhaseRotator() {
        backend.reset();
    }

    void PhaseRotator::prepare(const juce::dsp::ProcessSpec spec) {
        backend->prepare(spec);
    }

    void PhaseRotator::update(const juce::dsp::AudioBlock<float>& buffer) {
        backend->update(buffer);
    }

    void PhaseRotator::process(juce::dsp::ProcessContextReplacing<float>& context) {
        backend->process(context);
    }

}