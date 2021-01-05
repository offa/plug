/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2021  offa
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

#include "data_structs.h"
#include <gmock/gmock.h>

namespace test::matcher
{
    MATCHER_P(EffectIs, value, "")
    {
        return std::tie(value.fx_slot, value.effect_num, value.knob1, value.knob2, value.knob3,
                        value.knob4, value.knob5, value.knob6, value.position) ==
               std::tie(arg.fx_slot, arg.effect_num, arg.knob1, arg.knob2, arg.knob3,
                        arg.knob4, arg.knob5, arg.knob6, arg.position);
    }

    MATCHER_P(AmpIs, value, "")
    {
        return std::tie(value.amp_num, value.volume, value.gain, value.treble, value.middle, value.bass,
                        value.cabinet, value.noise_gate, value.threshold, value.master_vol, value.gain2,
                        value.presence, value.depth, value.bias, value.sag, value.brightness, value.usb_gain) ==
               std::tie(arg.amp_num, arg.volume, arg.gain, arg.treble, arg.middle, arg.bass,
                        arg.cabinet, arg.noise_gate, arg.threshold, arg.master_vol, arg.gain2,
                        arg.presence, arg.depth, arg.bias, arg.sag, arg.brightness, arg.usb_gain);
    }
}


namespace plug
{

    inline void PrintTo(const fx_pedal_settings& e, std::ostream* os)
    {
        *os << "[slot: " + std::to_string(e.fx_slot)
            << ", effect: " << std::to_string(static_cast<int>(e.effect_num))
            << ", knobs: (" << std::to_string(e.knob1) << ", " << std::to_string(e.knob2)
            << ", " << std::to_string(e.knob3) << ", " << std::to_string(e.knob4)
            << ", " << std::to_string(e.knob5) << ", " << std::to_string(e.knob6) << ")"
            << ", position: " << std::to_string(static_cast<int>(e.position)) << "]";
    }


    inline void PrintTo(const amp_settings& a, std::ostream* os)
    {
        *os << "[amp.num: " << std::to_string(static_cast<int>(a.amp_num))
            << ", volume: " << std::to_string(a.volume)
            << ", gain: " << std::to_string(a.gain)
            << ", treble: " << std::to_string(a.treble)
            << ", middle: " << std::to_string(a.middle)
            << ", bass: " << std::to_string(a.bass)
            << ", cabinet: " << std::to_string(static_cast<int>(a.cabinet))
            << ", noise_gate: " << std::to_string(a.noise_gate)
            << ", threshold: " << std::to_string(a.threshold)
            << ", master_vol: " << std::to_string(a.master_vol)
            << ", gain2: " << std::to_string(a.gain2)
            << ", presence: " << std::to_string(a.presence)
            << ", depth: " << std::to_string(a.depth)
            << ", bias: " << std::to_string(a.bias)
            << ", sag: " << std::to_string(a.sag)
            << ", brightness: " << std::to_string(a.brightness)
            << ", usb_gain: " << std::to_string(a.usb_gain) << "]";
    }

}
