/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2025  offa
 * Copyright (C) 2010-2016  piorekf <piorek@piorekf.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

namespace plug
{

    // list of all amplifiers
    enum class amps
    {
        FENDER_57_DELUXE,
        FENDER_59_BASSMAN,
        FENDER_57_CHAMP,
        FENDER_65_DELUXE_REVERB,
        FENDER_65_PRINCETON,
        FENDER_65_TWIN_REVERB,
        FENDER_SUPER_SONIC,
        BRITISH_60S,
        BRITISH_70S,
        BRITISH_80S,
        AMERICAN_90S,
        METAL_2000,

        // Mustang v2
        STUDIO_PREAMP,
        FENDER_57_TWIN,
        FENDER_60_THRIFT,
        BRITISH_COLOUR,
        BRITISH_WATTS
    };

    constexpr bool isV2Amp(amps amp)
    {
        switch (amp)
        {
            case amps::STUDIO_PREAMP:
            case amps::FENDER_57_TWIN:
            case amps::FENDER_60_THRIFT:
            case amps::BRITISH_COLOUR:
            case amps::BRITISH_WATTS:
                return true;
            default:
                return false;
        }
    }

    // list of all effects
    enum class effects
    {
        EMPTY,
        /* Gain/Stomp Box */
        OVERDRIVE,
        WAH,
        TOUCH_WAH,
        FUZZ,
        FUZZ_TOUCH_WAH,
        SIMPLE_COMP,
        COMPRESSOR,

        /* Modulation */
        SINE_CHORUS,
        TRIANGLE_CHORUS,
        SINE_FLANGER,
        TRIANGLE_FLANGER,
        VIBRATONE,
        VINTAGE_TREMOLO,
        SINE_TREMOLO,
        RING_MODULATOR,
        STEP_FILTER,
        PHASER,
        PITCH_SHIFTER,

        /* Delay */
        MONO_DELAY,
        MONO_ECHO_FILTER,
        STEREO_ECHO_FILTER,
        MULTITAP_DELAY,
        PING_PONG_DELAY,
        DUCKING_DELAY,
        REVERSE_DELAY,
        TAPE_DELAY,
        STEREO_TAPE_DELAY,

        /* Reverb */
        SMALL_HALL_REVERB,
        LARGE_HALL_REVERB,
        SMALL_ROOM_REVERB,
        LARGE_ROOM_REVERB,
        SMALL_PLATE_REVERB,
        LARGE_PLATE_REVERB,
        AMBIENT_REVERB,
        ARENA_REVERB,
        FENDER_63_SPRING_REVERB,
        FENDER_65_SPRING_REVERB,

        /* Mustang I V2 Effect */
        RANGER_BOOST,
        GREENBOX,
        ORANGEBOX,
        BLACKBOX,
        BIG_FUZZ,
        /* Mustang I V2 Modulation */
        WAH_MOD,
        TOUCH_WAH_MOD,
        DIATONIC_PITCH_SHIFTER
    };

    // list of all cabinets
    enum class cabinets
    {
        OFF,
        cab57DLX,
        cabBSSMN,
        cab65DLX,
        cab65PRN,
        cabCHAMP,
        cab4x12M,
        cab2x12C,
        cab4x12G,
        cab65TWN,
        cab4x12V,
        cabSS212,
        cabSS112
    };


    // Helper functions - for compatibility only.
    constexpr auto value(amps a)
    {
        return static_cast<std::uint8_t>(a);
    }

    constexpr auto value(effects e)
    {
        return static_cast<std::uint8_t>(e);
    }

    constexpr auto value(cabinets c)
    {
        return static_cast<std::uint8_t>(c);
    }

}
