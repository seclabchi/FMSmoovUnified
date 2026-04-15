#include "BassCentering.h"
#include <JuceHeader.h>

namespace fmsmoov {

    class BassCenteringBackend {

    public:
        void prepare(const juce::dsp::ProcessSpec& spec) {

            auto spec_mod = spec;
            spec_mod.numChannels = 1;

            schp_stage1.prepare(spec_mod);
            schp_stage2.prepare(spec_mod);

            schp_stage1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            schp_stage1.setCutoffFrequency(150.0f);
            schp_stage2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
            schp_stage2.setCutoffFrequency(150.0f);
            
        }

        void process(juce::dsp::ProcessContextReplacing<float>& context) {
            juce::ScopedNoDenormals noDenormals;

            /*
            * ProcessContextReplacing is wrapping an AudioBlock.  Get the block.
            */
            auto& block = context.getOutputBlock();

            /* Now get the channel pointers to the raw data */
            float* L = block.getChannelPointer(0);
            float* R = block.getChannelPointer(1);
            size_t numSamples = block.getNumSamples();

            /* We're going to do the mid/side encoding in-place on the L and R buffers,
             * L becomes MID and R becomes SIDE
             */

            for (size_t i = 0; i < numSamples; ++i) {
                float l = L[i];
                float r = R[i];
                L[i] = (l + r) * 0.5f;
                R[i] = (l - r) * 0.5f;
            }

            /* Now we want to high-pass the SIDE channel */

            juce::dsp::AudioBlock<float> side_block(&R, 1, numSamples);
            juce::dsp::ProcessContextReplacing<float> ctx_side(side_block);
            schp_stage1.process(ctx_side);
            schp_stage2.process(ctx_side);

            /* Now lets decode the mid and side back into L and R */

            for (size_t i = 0; i < numSamples; ++i) {
                float m = L[i];
                float s = R[i];
                L[i] = m + s;
                R[i] = m - s;
            }
        }

        void update(const juce::dsp::AudioBlock<float>&) {}

    private:
        using LR4Filter = juce::dsp::LinkwitzRileyFilter<float>;
        LR4Filter schp_stage1, schp_stage2;
    };

    BassCentering::BassCentering() : backend(std::make_unique<BassCenteringBackend>()) {
    }

    BassCentering::~BassCentering() {
        backend.reset();
    }

    void BassCentering::prepare(const juce::dsp::ProcessSpec spec) {
        backend->prepare(spec);
    }

    void BassCentering::update(const juce::dsp::AudioBlock<float>& buffer) {
        backend->update(buffer);
    }

    void BassCentering::process(juce::dsp::ProcessContextReplacing<float>& context) {
        backend->process(context);
    }

}