#include "TwoBandSlowAGC.h"
#include <JuceHeader.h>

namespace fmsmoov {

    class TwoBandSlowAGCBackend {
        friend TwoBandSlowAGC;
    public:
        TwoBandSlowAGCBackend(fmsmoov::LufsDataSource& _lufs_src, fmsmoov::CrossoverTwoBandDataSource& _data_src) : 
                lufs_src(_lufs_src), bands_src(_data_src) {
            lufs = std::make_unique<LufsEngine>();
        }

        void prepare(const juce::dsp::ProcessSpec& spec) {

            gateThreshLinearLo = juce::Decibels::decibelsToGain(settings.gate_thresh_lo.load());
            gateThreshLinearHi = juce::Decibels::decibelsToGain(settings.gate_thresh_hi.load());

            bands_src.get_buffers(bufLo, bufHi);

            /*
             * Do I need to make the intermediate step to a new AudioBlock?  Not sure.  It's cleaner?
             */

            bufLoL = bufLo.getSingleChannelBlock(0);
            bufLoR = bufLo.getSingleChannelBlock(1);
            bufHiL = bufHi.getSingleChannelBlock(0);
            bufHiR = bufHi.getSingleChannelBlock(1);

            pLoL = bufLoL.getChannelPointer(0);
            pLoR = bufLoR.getChannelPointer(0);
            pHiL = bufHiL.getChannelPointer(0);
            pHiR = bufHiR.getChannelPointer(0);

            /*
            peak_follower_cf_lo.prepare(spec_mod);
            peak_follower_cf_lo.setAttackTime(settings.attack_peak_cf_lo.load());
            peak_follower_cf_lo.setReleaseTime(settings.release_peak_cf_lo.load());
            peak_follower_cf_lo.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);
            peak_follower_cf_lo.reset();

            rms_follower_cf_lo.prepare(spec_mod);
            rms_follower_cf_lo.setAttackTime(settings.attack_rms_cf_lo.load());
            rms_follower_cf_lo.setReleaseTime(settings.release_rms_cf_lo.load());
            rms_follower_cf_lo.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::RMS);
            rms_follower_cf_lo.reset();

            peak_follower_cf_hi.prepare(spec_mod);
            peak_follower_cf_hi.setAttackTime(settings.attack_peak_cf_hi.load());
            peak_follower_cf_hi.setReleaseTime(settings.release_peak_cf_hi.load());
            peak_follower_cf_hi.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::peak);
            peak_follower_cf_hi.reset();

            rms_follower_cf_hi.prepare(spec_mod);
            rms_follower_cf_hi.setAttackTime(settings.attack_rms_cf_hi.load());
            rms_follower_cf_hi.setReleaseTime(settings.release_rms_cf_hi.load());
            rms_follower_cf_hi.setLevelCalculationType(juce::dsp::BallisticsFilterLevelCalculationType::RMS);
            rms_follower_cf_hi.reset();

            cf_smoother_lo.reset(spec_mod.sampleRate, settings.cf_smoothing_time.load() / 1000.0f);
            cf_smoother_hi.reset(spec_mod.sampleRate, settings.cf_smoothing_time.load() / 1000.0f);

            window_size = static_cast<int>(spec_mod.sampleRate * 0.3);
            circ_buf.setSize(1, window_size);
            circ_buf.clear();

            int numSegments = std::ceil((float)window_size / segment_size);
            segment_maxes.assign(numSegments, 0.0f);
            write_ptr = 0;
            sum_of_squares = 0.0f;

            bufL.setSize(2, spec.maximumBlockSize);
            bufH.setSize(2, spec.maximumBlockSize);
            */
            lufs->prepare(spec);
        }

        void process(juce::dsp::ProcessContextReplacing<float>& context) {
            juce::ScopedNoDenormals noDenormals;

            lufs_src.get_lufs(in_M, in_S);

            auto& inblock = context.getInputBlock();

            auto& block = context.getOutputBlock();
            const auto numSamples = block.getNumSamples();
            const int numChannels = block.getNumChannels();

            block.clear();
            block.copyFrom(bufLo);
            block.add(bufHi);

            for (size_t i = 0; i < block.getNumSamples(); i++) {
                lo_samp = std::max(std::abs(pLoL[i]), std::abs(pLoR[i]));
                hi_samp = std::max(std::abs(pHiL[i]), std::abs(pHiR[i]));
                tot_samp = lo_samp + hi_samp;
                lo_samp_db = juce::Decibels::gainToDecibels(lo_samp, -90.0f);
                hi_samp_db = juce::Decibels::gainToDecibels(hi_samp, -90.0f);
                tot_samp_db = juce::Decibels::gainToDecibels(tot_samp, -90.0f);

                lufs_error_S.store(loop_S - settings.target_lufs_S);

            }

            block.copyFrom(bufLo);
            block.add(bufHi);
            
            /* This is the feedback LUFS measurement */
            lufs->update(block);
            lufs_src.get_lufs(loop_M, loop_S);
            

            //auto out_L = block.getSingleChannelBlock(0);
            //auto out_R = block.getSingleChannelBlock(1);

            //out_R.copyFrom(bufLoL.getSingleChannelBlock(0));
            //out_R.add(bufLoR.getSingleChannelBlock(0));
            //out_L.copyFrom(bufHiL.getSingleChannelBlock(0));
            //out_L.add(bufHiR.getSingleChannelBlock(0));

            
            //for (size_t i = 0; i < block.getNumSamples(); i++) {
            //    lo_samp = std::max(std::abs(pLoL[i]), std::abs(pLoR[i]));
            //    hi_samp = std::max(std::abs(pHiL[i]), std::abs(pHiR[i]));
            //}

                /*
                lo_samp = std::max(std::abs(pLoL[i]), std::abs(pLoR[i]));
                hi_samp = std::max(std::abs(pHiL[i]), std::abs(pHiR[i]));

                lo_peak = peak_follower_cf_lo.processSample(0, lo_samp);
                lo_peak_db = juce::Decibels::gainToDecibels(lo_peak, -60.0f);
                lo_rms = rms_follower_cf_lo.processSample(0, lo_samp);
                lo_rms_db = juce::Decibels::gainToDecibels(lo_rms, -60.0f);
                hi_peak = peak_follower_cf_hi.processSample(0, hi_samp);
                hi_peak_db = juce::Decibels::gainToDecibels(hi_peak, -60.0f);
                hi_rms = rms_follower_cf_hi.processSample(0, hi_samp);
                hi_rms_db = juce::Decibels::gainToDecibels(hi_rms, -60.0f);

                cf_lo_db = lo_peak_db - lo_rms_db;
                cf_hi_db = hi_peak_db - hi_rms_db;

                cf_smoother_lo.setTargetValue(cf_lo_db);
                cf_smoother_hi.setTargetValue(cf_hi_db);
                current_cf_lo.store(cf_smoother_lo.getNextValue());
                current_cf_hi.store(cf_smoother_hi.getNextValue());
                */
            /*
                pLoL[i] *= 1.0f;
                pLoR[i] *= 1.0f;
                pHiL[i] *= 1.0f;
                pHiR[i] *= 1.0f;
            }

            block.clear();
//
//            for (int ch = 0; ch < numChannels; ++ch) {
//                float* dest = block.getChannelPointer(ch);
//                juce::FloatVectorOperations::copy(dest, bufL.getReadPointer(ch), numSamples);
//                juce::FloatVectorOperations::add(dest, bufH.getReadPointer(ch), numSamples);
//            }    
*/
        }

        float get_current_gr_lo() {
            return current_gr_lo.load();
        }

        float get_current_gr_hi() {
            return current_gr_hi.load();
        }

        float get_current_gr_lo_db() {
            return current_gr_lo_db.load();
        }

        float get_current_gr_hi_db() {
            return current_gr_hi_db.load();
        }

        float get_current_cf_lo() {
            return current_cf_lo.load();
        }

        float get_current_cf_hi() {
            return current_cf_hi.load();
        }

        float get_crest_factor_min() {
            return cf_min.load();
        }

        float get_crest_factor_max() {
            return cf_max.load();
        }

        float get_current_m() {
            return lufs->get_M();
        }

        float get_current_s() {
            return lufs->get_S();
        }

        float get_lufs_error_s() {
            return lufs_error_S.load();
        }

        void update(const juce::dsp::AudioBlock<float>&) {}

    private:
        fmsmoov::CrossoverTwoBandDataSource& bands_src;
        fmsmoov::LufsDataSource& lufs_src; //input LUFS measurements, not to be confused w/the control loop LUFS measurements
        std::unique_ptr<LufsEngine> lufs;

        juce::dsp::AudioBlock<float> bufLo;
        juce::dsp::AudioBlock<float> bufHi;

        float lo_samp, hi_samp, tot_samp, lo_samp_db, hi_samp_db, tot_samp_db;
        float in_M = 0.0f;
        float in_S = 0.0f;
        float loop_M = 0.0f;
        float loop_S = 0.0f;
        std::atomic<float> lufs_error_S;

        juce::dsp::AudioBlock<float> bufLoL, bufLoR, bufHiL, bufHiR;
        float* pLoL, * pLoR, * pHiL, * pHiR;

        float dT{ 0.00f };  
        float agcLoDb{ 0.00f };
        float agcHiDb{ 0.00f };

        std::atomic<bool> window_active_lo;
        std::atomic<bool> window_active_hi;
        std::atomic<bool> gate_active_lo;
        std::atomic<bool> gate_active_hi;

        std::atomic<float> current_gr_lo{ 0.0f };
        std::atomic<float> current_gr_hi{ 0.0f };
        std::atomic<float> current_gr_lo_db{ 0.0f };
        std::atomic<float> current_gr_hi_db{ 0.0f };

        juce::LinearSmoothedValue<float> cf_lo_smoothed;
        juce::LinearSmoothedValue<float> cf_hi_smoothed;
        std::atomic<float> current_cf_lo{ 0.0f };
        std::atomic<float> current_cf_hi{ 0.0f };

        std::atomic<float> cf_min{ -0.5f };
        std::atomic<float> cf_max{ 3.0f };

        float targetLevel{ 0.0f };
        float targetGainLo{ 0.0f };
        float targetGainHi{ 0.0f };
        float envelopeLo{ 0.0f };
        float envelopeHi{ 0.0f };

        float gateThreshLinearLo{ 0.0f };
        float gateThreshLinearHi{ 0.0f };

        fmsmoov::TWO_BAND_SLOW_AGC_SETTINGS settings;

        juce::dsp::BallisticsFilter<float> peak_follower_lo;
        juce::dsp::BallisticsFilter<float> rms_follower_lo;
        juce::dsp::BallisticsFilter<float> peak_follower_hi;
        juce::dsp::BallisticsFilter<float> rms_follower_hi;

        juce::dsp::BallisticsFilter<float> peak_follower_cf_lo;
        juce::dsp::BallisticsFilter<float> rms_follower_cf_lo;
        juce::dsp::BallisticsFilter<float> peak_follower_cf_hi;
        juce::dsp::BallisticsFilter<float> rms_follower_cf_hi;

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> cf_smoother_lo;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> cf_smoother_hi;
        
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain_smoother_lo;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain_smoother_hi;

        juce::AudioBuffer<float> circ_buf;
        int write_ptr;
        int window_size;
        double sum_of_squares;

        static constexpr int segment_size = 64;
        std::vector<float> segment_maxes;
        int current_segment_index = 0;
        float current_segment_running_max = 0.0f;

    };

    TwoBandSlowAGC::TwoBandSlowAGC(LufsDataSource& _lufs_src, CrossoverTwoBandDataSource& data_src) : 
        backend(std::make_unique<TwoBandSlowAGCBackend>(_lufs_src, data_src)) {
        startTimerHz(60);
    }

    TwoBandSlowAGC::~TwoBandSlowAGC() {
        backend.reset();
    }

    
    void TwoBandSlowAGC::timerCallback() {
        repaint();
    }

    float TwoBandSlowAGC::get_current_gr_lo_db() {
        return backend->get_current_gr_lo_db();
    }

    float TwoBandSlowAGC::get_current_gr_hi_db() {
        return backend->get_current_gr_hi_db();
    }

    void TwoBandSlowAGC::paint(juce::Graphics& g) {

        float current_gr_lo = backend->get_current_gr_lo_db();
        float current_gr_hi = backend->get_current_gr_hi_db();

        float current_loudness_m = backend->get_current_m();
        float current_loudness_s = backend->get_current_s();

        float lufs_error_s = backend->get_lufs_error_s();

        auto bounds = getLocalBounds().toFloat();

        auto ctrl_width = bounds.getWidth();
        auto ctrl_height = bounds.getHeight();

        auto meter_width = ctrl_width - 20.0f;
        auto meter_height = ctrl_height;

        auto bar_width = (meter_width / 8.0f);
        auto gr_lowband = bounds.withWidth(bar_width);
        auto gr_highband = bounds.withWidth(bar_width).translated(bar_width + 2, 0);

        auto bar_lufs_in_M = bounds.withWidth(bar_width).translated(gr_highband.getRight() + 2, 0);
        auto bar_lufs_in_S = bounds.withWidth(bar_width).translated(bar_lufs_in_M.getRight() + 2, 0);

        auto bar_lufs_S_err = bounds.withWidth(bar_width).translated(bar_lufs_in_S.getRight() + 2, 0);

        auto bar_height_gr_lo = juce::jmap(current_gr_lo, backend->settings.max_gr.load(), backend->settings.min_gr.load(), 0.0f, gr_lowband.getHeight());
        auto bar_height_gr_hi = juce::jmap(current_gr_hi, backend->settings.max_gr.load(), backend->settings.min_gr.load(), 0.0f, gr_highband.getHeight());

        auto bar_height_lufs_in_M = juce::jmap(current_loudness_m, -60.0f, 0.0f, 0.0f, bar_lufs_in_M.getHeight());
        auto bar_height_lufs_in_S = juce::jmap(current_loudness_s, -60.0f, 0.0f, 0.0f, bar_lufs_in_S.getHeight());

        auto bar_height_S_err = juce::jmap(lufs_error_s, -15.0f, +15.0f, 0.0f, bar_lufs_S_err.getHeight());

        g.setColour(juce::Colours::lightgrey);
        g.drawRect(juce::Rectangle<float>(0, 0, meter_width, meter_height));

        //g.setColour(juce::Colours::black);
        //g.fillRect(bg_img.getBounds());

        g.setColour(meter_gr_color);
        g.fillRect(gr_lowband.withSizeKeepingCentre(gr_lowband.getWidth() - 2, (int)bar_height_gr_lo).withY(gr_lowband.getBottom() - (int)bar_height_gr_lo));
        g.fillRect(gr_highband.withSizeKeepingCentre(gr_highband.getWidth() - 2, (int)bar_height_gr_hi).withY(gr_highband.getBottom() - (int)bar_height_gr_hi));

        g.setColour(meter_cf_color);
        g.fillRect(bar_lufs_in_M.withSizeKeepingCentre(bar_lufs_in_M.getWidth() - 2, (int)bar_height_lufs_in_M).withY(bar_lufs_in_M.getBottom() - (int)bar_height_lufs_in_M));
        g.fillRect(bar_lufs_in_S.withSizeKeepingCentre(bar_lufs_in_S.getWidth() - 2, (int)bar_height_lufs_in_S).withY(bar_lufs_in_S.getBottom() - (int)bar_height_lufs_in_S));

        g.setColour(meter_lufs_error_color);
        
        //TODO: Make the bar move from the middle in either direction, depending upon
        //whether the LUFS S error is positive or negative
        g.fillRect(bar_lufs_S_err.withSizeKeepingCentre(bar_lufs_S_err.getWidth() - 2, (int)bar_height_S_err).withY(bar_lufs_S_err.getBottom() - (int)bar_height_S_err));
    
        if (lufs_error_s < 0.0f) {
        }
        else {
        }


        //g.drawImageAt(bg_img, 0, 0);
        
    }

    void TwoBandSlowAGC::resized() {
       
        bg_img = juce::Image(juce::Image::ARGB, getWidth(), getHeight(), true);
        juce::Graphics g(bg_img);

        /*

        float db_ticks[12] = { 3.0, 0.0, -6.0, -12.0, -18.0, -24.0, -30.0, -36.0, -42.0, -48.0, -54.0, -60.0 };
        juce::String db_tick_labels[12] = { "+3", "0", "-6", "-12", "-18", "-24", "-30", "-36", "-42", "-48", "-54", "-60" };
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
        */
    }


    void TwoBandSlowAGC::prepare(const juce::dsp::ProcessSpec spec) {
        backend->prepare(spec);
    }

    void TwoBandSlowAGC::update(const juce::dsp::AudioBlock<float>& buffer) {
        backend->update(buffer);
    }

    void TwoBandSlowAGC::process(juce::dsp::ProcessContextReplacing<float>& context) {
        backend->process(context);
    }



}