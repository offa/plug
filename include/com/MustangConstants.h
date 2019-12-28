/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2019  offa
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
#include <cstddef>

namespace plug::com
{
    namespace v1
    {
    // amp's VID and PID while in update mode
    inline constexpr std::uint16_t USB_UPDATE_VID{0x1ed8};
    inline constexpr std::uint16_t SMALL_AMPS_USB_UPDATE_PID{0x0006};    //Mustang I and II
    inline constexpr std::uint16_t BIG_AMPS_USB_UPDATE_PID{0x0007};      //Mustang III, IV, V
    inline constexpr std::uint16_t MINI_USB_UPDATE_PID{0x0011};          //Mustang Mini
    inline constexpr std::uint16_t FLOOR_USB_UPDATE_PID{0x0013};         //Mustang Floor
    inline constexpr std::uint16_t SMALL_AMPS_V2_USB_UPDATE_PID{0x0015}; //Mustang I & II V2
    inline constexpr std::uint16_t BIG_AMPS_V2_USB_UPDATE_PID{0x0017};   //Mustang III+ V2

    // effect array fields
    inline constexpr std::size_t DSP{2};
    inline constexpr std::size_t EFFECT{16};
    inline constexpr std::size_t FXSLOT{18};
    inline constexpr std::size_t KNOB1{32};
    inline constexpr std::size_t KNOB2{33};
    inline constexpr std::size_t KNOB3{34};
    inline constexpr std::size_t KNOB4{35};
    inline constexpr std::size_t KNOB5{36};
    inline constexpr std::size_t KNOB6{37};

    // amp array fields
    inline constexpr std::size_t AMPLIFIER{16};
    inline constexpr std::size_t VOLUME{32};
    inline constexpr std::size_t GAIN{33};
    inline constexpr std::size_t TREBLE{36};
    inline constexpr std::size_t MIDDLE{37};
    inline constexpr std::size_t BASS{38};
    inline constexpr std::size_t CABINET{49};
    inline constexpr std::size_t NOISE_GATE{47};
    inline constexpr std::size_t THRESHOLD{48};
    inline constexpr std::size_t MASTER_VOL{35};
    inline constexpr std::size_t GAIN2{34};
    inline constexpr std::size_t PRESENCE{39};
    inline constexpr std::size_t DEPTH{41};
    inline constexpr std::size_t BIAS{42};
    inline constexpr std::size_t SAG{51};
    inline constexpr std::size_t BRIGHTNESS{52};

    inline constexpr std::size_t USB_GAIN{16};
    inline constexpr std::size_t NAME{16};

    // save fields
    inline constexpr std::size_t SAVE_SLOT{4};
    inline constexpr std::size_t FXKNOB{3};
    }
}
