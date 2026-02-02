#pragma once

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