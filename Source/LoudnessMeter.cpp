#include "LoudnessMeter.h"
#include <JuceHeader.h>
#include "LufsEngine.h"

namespace fmsmoov {

    class LoudnessMeterBackend {
    public:

        LoudnessMeterBackend() {
            lufs_engine = std::make_unique<LufsEngine>();
        }

        void update(const juce::dsp::AudioBlock<float>& block) {
            lufs_engine->update(block);
        }

        void prepare(const juce::dsp::ProcessSpec& spec) {
            lufs_engine->prepare(spec);
        }

        LufsDataSource& get_lufs_source()  {
            return lufs_engine->get_data_src();
        }

        float get_M() {
            return lufs_engine->get_M();
        }

        float get_S() {
            return lufs_engine->get_S();
        }

    private:
        std::unique_ptr<LufsEngine> lufs_engine;
    };

    LoudnessMeter::LoudnessMeter() : backend(std::make_unique<LoudnessMeterBackend>()) {
        startTimerHz(60);
        meter_rms_color = juce::Colour::fromString("#ff00ffa0");
        meter_peak_color = juce::Colours::beige;
    }

    LoudnessMeter::~LoudnessMeter() {
        backend.reset();
    }

    void LoudnessMeter::prepare(const juce::dsp::ProcessSpec spec) {
        backend->prepare(spec);
    }

    void LoudnessMeter::update(const juce::dsp::AudioBlock<float>& buffer) {
        backend->update(buffer);
    }

    void LoudnessMeter::timerCallback() {
        repaint();
    }

    LufsDataSource& LoudnessMeter::get_lufs_source() {
        return backend->get_lufs_source();
    }


    /*
     * 1. Standard Meter Scales 
    You should offer two selectable ranges for your Momentary (M) and Short-term (S) meters: 
    EBU +9 Scale (Default): Best for standard program material.
        Absolute Range: -41.0 LUFS to -14.0 LUFS.
        Relative Range: -18.0 LU to +9.0 LU.
    EBU +18 Scale: Best for wide dynamic range content (e.g., classical music or films).
        Absolute Range: -59.0 LUFS to -5.0 LUFS.
        Relative Range: -36.0 LU to +18.0 LU. 

    2. Target Alignment
    In "EBU Mode," the zero point of the relative (LU) scale is mapped to the target loudness: 
    0 LU = -23.0 LUFS. 

    3. Practical FM Broadcast Limits
    Since you are processing for FM broadcast, your actual operating levels will likely be "hotter" than the -23 LUFS production target to compete on the dial. 
    Maximum Permitted Short-term (S): Aim for a limit of -18.0 LUFS (+5.0 LU on the relative scale).
    Maximum Permitted Momentary (M): Alternatively, some stations use a limit of -15.0 LUFS (+8.0 LU on the relative scale). 
    
    4. Visualization Recommendations
    Color Coding: Use a green zone up to -23 LUFS (0 LU), a yellow/amber zone from 0 LU to +5 LU, and red for anything above +5 or +8 LU.
    True Peak Ceiling: Regardless of your M and S range, your True Peak meter must have a clear "ceiling" line at -1.0 dBTP. 
     * 
     */

    void LoudnessMeter::paint(juce::Graphics& g) {

        auto bounds = getLocalBounds().toFloat();
        auto width = bounds.getWidth();
        auto height = bounds.getHeight() - text_offset;

        auto bar_width = (width / 2.0f) - 8;
        auto left_bar = bounds.withWidth(bar_width).withHeight(height);
        auto right_bar = bounds.withWidth(bar_width).withHeight(height).translated(width - bar_width, 0);

        auto height_M = juce::jmap(backend->get_M(), -36.0f, -6.0f, 0.0f, left_bar.getHeight());
        auto height_S = juce::jmap(backend->get_S(), -36.0f, -6.0f, 0.0f, left_bar.getHeight());

        //auto left_height_peak = juce::jmap(backend->get_peak_level_db(0), -60.0f, +3.0f, 0.0f, left_bar.getHeight());
        //auto right_height_peak = juce::jmap(backend->get_peak_level_db(1), -60.0f, +3.0f, 0.0f, right_bar.getHeight());

        //auto left_height_rms = juce::jmap(backend->get_rms_level_db(0), -60.0f, +3.0f, 0.0f, left_bar.getHeight());
        //auto right_height_rms = juce::jmap(backend->get_rms_level_db(1), -60.0f, +3.0f, 0.0f, right_bar.getHeight());

        //auto left_height_vu = juce::jmap(backend->get_vu_level(0), -60.0f, 0.0f, 0.0f, left_bar.getHeight());
        //auto right_height_vu = juce::jmap(backend->get_vu_level(1), -60.0f, 0.0f, 0.0f, right_bar.getHeight());


        g.setColour(juce::Colours::black);
        g.fillRect(bg_img.getBounds());
        g.setColour(juce::Colours::white);
        g.drawFittedText(juce::String("LUFS M/S"), 0, height, width, text_offset, juce::Justification::centred, 2);

        g.setColour(meter_peak_color);

        g.fillRect(left_bar.withSizeKeepingCentre(left_bar.getWidth() - 2, (int)height_M).withY(left_bar.getBottom() - (int)height_M));
        g.fillRect(right_bar.withSizeKeepingCentre(right_bar.getWidth() - 2, (int)height_S).withY(right_bar.getBottom() - (int)height_S));
        
        //g.setColour(meter_rms_color);

        //g.fillRect(left_bar.withSizeKeepingCentre(left_bar.getWidth() - 2, (int)left_height_rms).withY(left_bar.getBottom() - (int)left_height_rms));
        //g.fillRect(right_bar.withSizeKeepingCentre(right_bar.getWidth() - 2, (int)right_height_rms).withY(right_bar.getBottom() - (int)right_height_rms));
        
        g.drawImageAt(bg_img, 0, 0);
    }

    void LoudnessMeter::resized() {
        
        bg_img = juce::Image(juce::Image::ARGB, getWidth(), getHeight() - text_offset, true);
        juce::Graphics g(bg_img);

#define NUM_TICKS 16

        float db_ticks[NUM_TICKS] = { -6.0, -8.0, -10.0, -12.0, -14.0, -16.0, -18.0, -20.0, -22.0, -24.0, -26.0, -28.0, -30.0, -32.0, -34.0, -36.0 };
        juce::String db_tick_labels[NUM_TICKS] = { "-6", "-8", "-10", "-12", "-14", "-16", "-18", "-20", "-22", "-24", "-26", "-28", "-30", "-32", "-34", "-36"};
        float tick_heights[NUM_TICKS];
        memset(tick_heights, 0, sizeof(tick_heights));
        float h;

        for (uint32_t i = 0; i < NUM_TICKS; i++) {
            h = juce::jmap(db_ticks[i], -36.0f, -6.0f, (float)getHeight() - text_offset, 0.0f);
            tick_heights[i] = h;
        }

        g.setColour(juce::Colours::darkgrey);

        for (uint32_t i = 0; i < NUM_TICKS; i++) {
            float f = tick_heights[i];
            g.drawHorizontalLine(f, 0, getWidth());
        }

        g.setColour(juce::Colours::white);
        g.drawRect(bg_img.getBounds());

        g.setFont(11.0f);

        g.setColour(juce::Colours::white);

        for (uint32_t i = 0; i < NUM_TICKS; i++) {
            juce::String l = db_tick_labels[i];
            juce::Rectangle<float> rect{ 0, tick_heights[i], (float)getWidth(), 11.0 };
            //g.setColour(juce::Colours::darkblue);
            //g.fillRect(rect);

            g.drawText(l, rect, juce::Justification::centredBottom, false);
        }
    }

}