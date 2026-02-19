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

                auto* read_pointer = tempBuffer.getReadPointer(ch);
                float ballistic_level = 0.0f;

                for (uint32_t i = 0; i < tempBuffer.getNumSamples(); ++i) {
                    ballistic_level = ballistics[ch].processSample(0, std::abs(read_pointer[i]));
                }
                float db = juce::Decibels::gainToDecibels(ballistic_level);
                vu_level[ch].store(db);

                raw_level = juce::Decibels::gainToDecibels(rms, -60.0f);
            }
        }

        float getLevel(int ch) { 
            return levels[ch].getCurrentValue(); 
        }

        float get_vu_level(int ch) {
            return vu_level[ch].load();
        }

        void prepare(const juce::dsp::ProcessSpec& spec) {
            for (auto& l : levels) { 
                l.reset(spec.sampleRate, (float)(1/60.0)); 
            }

            for (auto& b : ballistics) {
                b.prepare(spec);
                b.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);
                b.setAttackTime(300.0f);
                b.setReleaseTime(300.0f);
            }

        }

    private:
        juce::LinearSmoothedValue<float> levels[2];
        juce::dsp::BallisticsFilter<float> ballistics[2];
        std::atomic<float> vu_level[2];

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

        //auto left_height = juce::jmap(backend->getLevel(0), -60.0f, 0.0f, +6.0f, left_bar.getHeight());
        //auto right_height = juce::jmap(backend->getLevel(1), -60.0f, 0.0f, +6.0f, right_bar.getHeight());
        auto left_height = juce::jmap(backend->get_vu_level(0), -60.0f, 0.0f, +6.0f, left_bar.getHeight());
        auto right_height = juce::jmap(backend->get_vu_level(0), -60.0f, 0.0f, +6.0f, right_bar.getHeight());

        g.drawImageAt(bg_img, 0, 0);
        
        g.setColour(juce::Colours::aqua);

        g.fillRect(left_bar.withSizeKeepingCentre(left_bar.getWidth() - 2, (int)left_height).withY(left_bar.getBottom() - (int)left_height));
        g.fillRect(right_bar.withSizeKeepingCentre(right_bar.getWidth() - 2, (int)right_height).withY(right_bar.getBottom() - (int)right_height));

        

    }

    void LevelMeter::resized() { 
        bg_img = juce::Image(juce::Image::ARGB, getWidth(), getHeight(), true);
        juce::Graphics g(bg_img);

        g.setColour(juce::Colours::black);
        g.fillRect(bg_img.getBounds());

        float db_ticks[12] = {3.0, 0.0, -6.0, -12.0, -18.0, -24.0, -30.0, -36.0, -42.0, -48.0, -54.0, -60.0};
        juce::String db_tick_labels[12] = {"+3", "0", "-6", "-12", "-18", "-24", "-30", "-36", "-42", "-48", "-54", "-60"};
        float tick_heights[12];
        memset(tick_heights, 0, sizeof(tick_heights));
        float h;

        for (uint32_t i = 0; i < 12; i++) {
            h = juce::jmap(db_ticks[i], -60.0f, +3.0f, (float)getHeight(), 0.0f);
            tick_heights[i] = h;
        }
       
        g.setColour(juce::Colours::darkgrey);

        for (uint32_t i = 0; i < 12; i++) {
            float f = tick_heights[i];
            g.drawHorizontalLine(f, 0, getWidth());
        }

        g.setColour(juce::Colours::lightgrey);
        g.drawRect(bg_img.getBounds());

        g.setFont(11.0f);
        

        for (uint32_t i = 0; i < 12; i++) {
            juce::String l = db_tick_labels[i];
            juce::Rectangle<float> rect{ 0, tick_heights[i], (float)getWidth(), 11.0 };
            //g.setColour(juce::Colours::darkblue);
            //g.fillRect(rect);
            g.setColour(juce::Colours::white);
            g.drawText(l, rect, juce::Justification::centredBottom, false);
        }
    }
}