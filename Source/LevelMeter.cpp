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
                float peak = tempBuffer.getMagnitude(ch, 0, tempBuffer.getNumSamples());

                if (peak > peak_levels_filtered[ch]) {
                    peak_levels_filtered[ch] = peak;
                }
                else {
                    peak_levels_filtered[ch].store(peak_levels_filtered[ch].load() * 0.99f);
                }

                peak_levels[ch].store(juce::Decibels::gainToDecibels(peak, -60.0f));
                peak_levels_db[ch].store(juce::Decibels::gainToDecibels(peak_levels_filtered[ch].load(), -60.0f));

                if (rms > rms_levels_filtered[ch]) {
                    rms_levels_filtered[ch] = rms;
                }
                else {
                    rms_levels_filtered[ch].store((rms * (1.0f - 0.99005f)) + (rms_levels_filtered[ch] * 0.99005f));
                }
                
                rms_levels[ch].store(juce::Decibels::gainToDecibels(rms, -60.0f));
                rms_levels_db[ch].store(juce::Decibels::gainToDecibels(rms_levels_filtered[ch].load(), -60.0f));
                
                //levels[ch].setTargetValue(juce::Decibels::gainToDecibels(rms, -60.0f));
                //levels[ch].skip(tempBuffer.getNumSamples());

                auto* read_pointer = tempBuffer.getReadPointer(ch);
                float ballistic_level = 0.0f;

                for (uint32_t i = 0; i < tempBuffer.getNumSamples(); ++i) {
                    ballistic_level = ballistics[ch].processSample(0, std::abs(read_pointer[i]));
                }
                //float db = juce::Decibels::gainToDecibels(ballistic_level);
                //vu_level[ch].store(db);

                //raw_level = juce::Decibels::gainToDecibels(rms, -60.0f);
            }
        }

        float get_rms_level(int ch) { 
            return rms_levels[ch].load(); 
        }

        float get_rms_level_filtered(int ch) {
            return rms_levels_filtered[ch].load();
        }

        float get_rms_level_db(int ch) {
            return rms_levels_db[ch].load();
        }

        float get_peak_level(int ch) {
            return peak_levels[ch].load();
        }

        float get_peak_level_filtered(int ch) {
            return peak_levels_filtered[ch].load();
        }

        float get_peak_level_db(int ch) {
            return peak_levels_db[ch].load();
        }

        float get_vu_level(int ch) {
            return vu_levels[ch].load();
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

        std::atomic<float> peak_levels[2] = { 0.0f, 0.0f };
        std::atomic<float> peak_levels_filtered[2] = { 0.0f, 0.0f };
        std::atomic<float> peak_levels_db[2] = { -60.0f, -60.0f };

        std::atomic<float> rms_levels[2] = { 0.0f, 0.0f };
        std::atomic<float> rms_levels_filtered[2] = { 0.0f, 0.0f };
        std::atomic<float> rms_levels_db[2] = { -60.0f, -60.0f };

        std::atomic<float> vu_levels[2];
    };

    LevelMeter::LevelMeter() : backend(std::make_unique<LevelMeterBackend>()) {
        startTimerHz(60);
        meter_rms_color = juce::Colour::fromString("#ff00ffa0");
        meter_peak_color = juce::Colours::beige;
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
        auto height = bounds.getHeight() - text_offset;

        auto bar_width = (width / 2.0f) - 8;
        auto left_bar = bounds.withWidth(bar_width).withHeight(height);
        auto right_bar = bounds.withWidth(bar_width).withHeight(height).translated(width - bar_width, 0);

        //auto left_height = juce::jmap(backend->getLevel(0), -60.0f, 0.0f, +6.0f, left_bar.getHeight());
        //auto right_height = juce::jmap(backend->getLevel(1), -60.0f, 0.0f, +6.0f, right_bar.getHeight());

        auto left_height_peak = juce::jmap(backend->get_peak_level_db(0), -60.0f, +3.0f, 0.0f, left_bar.getHeight());
        auto right_height_peak = juce::jmap(backend->get_peak_level_db(1), -60.0f, +3.0f, 0.0f, right_bar.getHeight());

        auto left_height_rms = juce::jmap(backend->get_rms_level_db(0), -60.0f, +3.0f, 0.0f, left_bar.getHeight());
        auto right_height_rms = juce::jmap(backend->get_rms_level_db(1), -60.0f, +3.0f, 0.0f, right_bar.getHeight());

        //auto left_height_vu = juce::jmap(backend->get_vu_level(0), -60.0f, 0.0f, 0.0f, left_bar.getHeight());
        //auto right_height_vu = juce::jmap(backend->get_vu_level(1), -60.0f, 0.0f, 0.0f, right_bar.getHeight());


        g.setColour(juce::Colours::black);
        g.fillRect(bg_img.getBounds());

        g.setColour(juce::Colours::white);
        g.drawFittedText(juce::String("dB PEAK/RMS"), 0, height, width, text_offset, juce::Justification::centred, 2);

        g.setColour(meter_peak_color);

        g.fillRect(left_bar.withSizeKeepingCentre(left_bar.getWidth() - 2, (int)left_height_peak).withY(left_bar.getBottom() - (int)left_height_peak));
        g.fillRect(right_bar.withSizeKeepingCentre(right_bar.getWidth() - 2, (int)right_height_peak).withY(right_bar.getBottom() - (int)right_height_peak));
        
        g.setColour(meter_rms_color);

        g.fillRect(left_bar.withSizeKeepingCentre(left_bar.getWidth() - 2, (int)left_height_rms).withY(left_bar.getBottom() - (int)left_height_rms));
        g.fillRect(right_bar.withSizeKeepingCentre(right_bar.getWidth() - 2, (int)right_height_rms).withY(right_bar.getBottom() - (int)right_height_rms));
        
        g.drawImageAt(bg_img, 0, 0);
    }

    void LevelMeter::resized() { 
        bg_img = juce::Image(juce::Image::ARGB, getWidth(), getHeight() - text_offset, true);
        juce::Graphics g(bg_img);

        

        float db_ticks[12] = {3.0, 0.0, -6.0, -12.0, -18.0, -24.0, -30.0, -36.0, -42.0, -48.0, -54.0, -60.0};
        juce::String db_tick_labels[12] = {"+3", "0", "-6", "-12", "-18", "-24", "-30", "-36", "-42", "-48", "-54", "-60"};
        float tick_heights[12];
        memset(tick_heights, 0, sizeof(tick_heights));
        float h;

        for (uint32_t i = 0; i < 12; i++) {
            h = juce::jmap(db_ticks[i], -60.0f, +3.0f, (float)getHeight() - text_offset, 0.0f);
            tick_heights[i] = h;
        }
       
        g.setColour(juce::Colours::darkgrey);

        for (uint32_t i = 0; i < 12; i++) {
            float f = tick_heights[i];
            g.drawHorizontalLine(f, 0, getWidth());
        }

        g.setColour(juce::Colours::white);
        g.drawRect(bg_img.getBounds());

        g.setFont(11.0f);
        
        g.setColour(juce::Colours::white);

        for (uint32_t i = 0; i < 12; i++) {
            juce::String l = db_tick_labels[i];
            juce::Rectangle<float> rect{ 0, tick_heights[i], (float)getWidth(), 11.0 };
            //g.setColour(juce::Colours::darkblue);
            //g.fillRect(rect);
            
            g.drawText(l, rect, juce::Justification::centredBottom, false);
        }
    }
}