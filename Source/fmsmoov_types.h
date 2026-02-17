#pragma once


#define TOOLTIP_BUTTON_MAIN_BYPASS "Bypass entire FMSmoov toolchain.  Raw in-to-out."
#define TOOLTIP_BUTTON_GENERATOR_STATE "Enable the test tone generator."
#define TOOLTIP_BUTTON_PROCESSOR_BYPASS "Bypass the main processor chain."

namespace fmsmoov {

    typedef enum {
        INVALID = 0,
        SINE = 1,
        SAWTOOTH,
        TRIANGLE,
        SQUARE,
        NOISE_WHITE,
        NOISE_PINK,
        NOISE_BROWN,
        NOISE_BLUE
    } GEN_TYPE;

} //namespace fmsmoov