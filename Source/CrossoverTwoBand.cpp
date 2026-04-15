#include "CrossoverTwoBand.h"

namespace fmsmoov {
    void CrossoverTwoBand::prepare(const juce::dsp::ProcessSpec& spec) {
        auto spec_mod = spec;
        spec_mod.numChannels = 1;

        filtLoL.prepare(spec_mod);
        filtLoR.prepare(spec_mod);
        filtHiL.prepare(spec_mod);
        filtHiR.prepare(spec_mod);

        filtLoL.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        filtLoL.setCutoffFrequency(200.0f);

        filtLoR.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        filtLoR.setCutoffFrequency(200.0f);

        filtHiL.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
        filtHiL.setCutoffFrequency(200.0f);

        filtHiR.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
        filtHiR.setCutoffFrequency(200.0f);

        bufL.setSize(2, spec.maximumBlockSize);
        bufH.setSize(2, spec.maximumBlockSize);
    }

    void CrossoverTwoBand::process(juce::dsp::ProcessContextReplacing<float>& context) {
        auto& block = context.getOutputBlock();
        const auto numSamples = block.getNumSamples();
        const int numChannels = block.getNumChannels();

        /* copy the input buffer into two new stereo buffers for the L and H band */

        for (int ch = 0; ch < numChannels; ++ch) {
            bufL.copyFrom(ch, 0, block.getChannelPointer(ch), numSamples);
            bufH.copyFrom(ch, 0, block.getChannelPointer(ch), numSamples);
        }

        /* Define audio blocks that wrap the new buffers, and then run them through
        *  the crossover.
         */
        auto lbL = juce::dsp::AudioBlock<float>(bufL).getSingleChannelBlock(0);
        filtLoL.process(juce::dsp::ProcessContextReplacing<float>(lbL));
        auto lbR = juce::dsp::AudioBlock<float>(bufL).getSingleChannelBlock(1);
        filtLoR.process(juce::dsp::ProcessContextReplacing<float>(lbR));
        auto hbL = juce::dsp::AudioBlock<float>(bufH).getSingleChannelBlock(0);
        filtHiL.process(juce::dsp::ProcessContextReplacing<float>(hbL));
        auto hbR = juce::dsp::AudioBlock<float>(bufH).getSingleChannelBlock(1);
        filtHiR.process(juce::dsp::ProcessContextReplacing<float>(hbR));

        block.clear();

        for (int ch = 0; ch < numChannels; ++ch) {
            float* dest = block.getChannelPointer(ch);
            juce::FloatVectorOperations::copy(dest, bufL.getReadPointer(ch), numSamples);
            juce::FloatVectorOperations::add(dest, bufH.getReadPointer(ch), numSamples);
        }
    }

    void CrossoverTwoBand::get_buffers(juce::dsp::AudioBlock<float>& block_lb, juce::dsp::AudioBlock<float>& block_hb) {
        block_lb = juce::dsp::AudioBlock<float>(bufL);
        block_hb = juce::dsp::AudioBlock<float>(bufH);
    }
}