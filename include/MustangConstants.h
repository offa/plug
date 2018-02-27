/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2018  offa
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

namespace plug
{
    // amp's VID and PID
    constexpr std::uint16_t USB_VID{0x1ed8};
    constexpr std::uint16_t SMALL_AMPS_USB_PID{0x0004};    //Mustang I and II
    constexpr std::uint16_t BIG_AMPS_USB_PID{0x0005};      //Mustang III, IV and V
    constexpr std::uint16_t MINI_USB_PID{0x0010};          //Mustang Mini
    constexpr std::uint16_t FLOOR_USB_PID{0x0012};         //Mustang Floor
    constexpr std::uint16_t SMALL_AMPS_V2_USB_PID{0x0014}; //Mustang II (and I?) V2
    constexpr std::uint16_t BIG_AMPS_V2_USB_PID{0x0016};   //Mustang III+ V2

    // amp's VID and PID while in update mode
    constexpr std::uint16_t USB_UPDATE_VID{0x1ed8};
    constexpr std::uint16_t SMALL_AMPS_USB_UPDATE_PID{0x0006};    //Mustang I and II
    constexpr std::uint16_t BIG_AMPS_USB_UPDATE_PID{0x0007};      //Mustang III, IV, V
    constexpr std::uint16_t MINI_USB_UPDATE_PID{0x0011};          //Mustang Mini
    constexpr std::uint16_t FLOOR_USB_UPDATE_PID{0x0013};         //Mustang Floor
    constexpr std::uint16_t SMALL_AMPS_V2_USB_UPDATE_PID{0x0015}; //Mustang I & II V2
    constexpr std::uint16_t BIG_AMPS_V2_USB_UPDATE_PID{0x0017};   //Mustang III+ V2

    // for USB communication
    constexpr std::size_t TMOUT{500};
    constexpr std::size_t LENGTH{64};
    //#define NANO_SEC_SLEEP 10000000

    // effect array fields
    constexpr std::size_t DSP{2};
    constexpr std::size_t EFFECT{16};
    constexpr std::size_t FXSLOT{18};
    constexpr std::size_t KNOB1{32};
    constexpr std::size_t KNOB2{33};
    constexpr std::size_t KNOB3{34};
    constexpr std::size_t KNOB4{35};
    constexpr std::size_t KNOB5{36};
    constexpr std::size_t KNOB6{37};

    // amp array fields
    constexpr std::size_t AMPLIFIER{16};
    constexpr std::size_t VOLUME{32};
    constexpr std::size_t GAIN{33};
    constexpr std::size_t TREBLE{36};
    constexpr std::size_t MIDDLE{37};
    constexpr std::size_t BASS{38};
    constexpr std::size_t CABINET{49};
    constexpr std::size_t NOISE_GATE{47};
    constexpr std::size_t THRESHOLD{48};
    constexpr std::size_t MASTER_VOL{35};
    constexpr std::size_t GAIN2{34};
    constexpr std::size_t PRESENCE{39};
    constexpr std::size_t DEPTH{41};
    constexpr std::size_t BIAS{42};
    constexpr std::size_t SAG{51};
    constexpr std::size_t BRIGHTNESS{52};

    // save fields
    constexpr std::size_t SAVE_SLOT{4};
    constexpr std::size_t FXKNOB{3};
}
