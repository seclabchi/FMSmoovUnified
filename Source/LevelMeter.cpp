#include "LevelMeter.h"
#include <JuceHeader.h>

namespace fmsmoov {

    class LevelMeterBackend {
    public:
        void update(const juce::dsp::AudioBlock<float>& block) {

            float* channel_pointers[2];
            channel_pointers[0] = block.getChannelPointer(0);
            channel_pointers[1] = block.getChannelPointer(1);

            juce::AudioBuffer<float> tempBuffer(channel_pointers,
                (int)block.getNumChannels(),
                (int)block.getNumSamples());

            for (int ch = 0; ch < std::min(2, (int)tempBuffer.getNumChannels()); ++ch) {
                float rms = tempBuffer.getRMSLevel(ch, 0, tempBuffer.getNumSamples());
                levels[ch].setTargetValue(juce::Decibels::gainToDecibels(rms, -60.0f));
                levels[ch].skip(tempBuffer.getNumSamples());
                raw_level = juce::Decibels::gainToDecibels(rms, -60.0f);
            }
        }

        float getLevel(int ch) { 
            return levels[ch].getCurrentValue(); 
            //return raw_level;
        }

        void prepare(const juce::dsp::ProcessSpec& spec) {
            for (auto& l : levels) { l.reset(spec.sampleRate, (float)(1/60.0)); }
        }

    private:
        juce::LinearSmoothedValue<float> levels[2];
        float raw_level;
    };

    LevelMeter::LevelMeter() : backend(std::make_unique<LevelMeterBackend>()) {
        startTimerHz(60);
    }

    LevelMeter::~LevelMeter() {
        backend.reset();
    }

    void LevelMeter::prepare(const juce::dsp::ProcessSpec spec) {
        backend->prepare(spec);
    }

    void LevelMeter::update(const juce::dsp::AudioBlock<float>& buffer) {
        backend->update(buffer);
    }

    void LevelMeter::timerCallback() {
        repaint();
    }

    void LevelMeter::paint(juce::Graphics& g) {
        auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();

        auto bar_width = (width / 2.0f) - 1;
        auto left_bar = bounds.withWidth(bar_width);
        auto right_bar = bounds.withWidth(bar_width).translated(bar_width + 2, 0);

        auto left_height = juce::jmap(backend->getLevel(0), -60.0f, 0.0f, +6.0f, left_bar.getHeight());
        auto right_height = juce::jmap(backend->getLevel(1), -60.0f, 0.0f, +6.0f, right_bar.getHeight());

        g.setColour(juce::Colours::black);
        g.fillRect(bounds);

        g.setColour(juce::Colours::aqua);

        g.fillRect(left_bar.withSizeKeepingCentre(left_bar.getWidth() - 2, (int)left_height).withY(left_bar.getBottom() - (int)left_height));
        g.fillRect(right_bar.withSizeKeepingCentre(right_bar.getWidth() - 2, (int)right_height).withY(right_bar.getBottom() - (int)right_height));

    }

    void LevelMeter::resized() {}

}