/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2019  offa
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

namespace test::constants
{
    inline constexpr std::size_t nameLength{32};

    // Effects
    inline constexpr std::size_t posEffect{16};
    inline constexpr std::size_t posFxSlot{18};
    inline constexpr std::size_t posKnob1{32};
    inline constexpr std::size_t posKnob2{33};
    inline constexpr std::size_t posKnob3{34};
    inline constexpr std::size_t posKnob4{35};
    inline constexpr std::size_t posKnob5{36};
    inline constexpr std::size_t posKnob6{37};

    // Amp
    inline constexpr std::size_t ampPos{16};
    inline constexpr std::size_t volumePos{32};
    inline constexpr std::size_t gainPos{33};
    inline constexpr std::size_t treblePos{36};
    inline constexpr std::size_t middlePos{37};
    inline constexpr std::size_t bassPos{38};
    inline constexpr std::size_t cabinetPos{49};
    inline constexpr std::size_t noiseGatePos{47};
    inline constexpr std::size_t thresholdPos{48};
    inline constexpr std::size_t masterVolPos{35};
    inline constexpr std::size_t gain2Pos{34};
    inline constexpr std::size_t presencePos{39};
    inline constexpr std::size_t depthPos{41};
    inline constexpr std::size_t biasPos{42};
    inline constexpr std::size_t sagPos{51};
    inline constexpr std::size_t brightnessPos{52};
    inline constexpr std::size_t usbGainPos{16};

    // USB Data
    inline constexpr std::uint8_t endpointSend{0x01};
    inline constexpr std::uint8_t endpointReceive{0x81};
    inline constexpr std::uint16_t usbVid{0x1ed8};
    inline constexpr std::uint16_t pidMustangI_II{0x0004};
    inline constexpr std::uint16_t pidMustangIII_IV_V{0x0005};
    inline constexpr std::uint16_t pidMustangMini{0x0010};
    inline constexpr std::uint16_t pidMustangFloor{0x0012};
    inline constexpr std::uint16_t pidMustangI_II_v2{0x0014};
    inline constexpr std::uint16_t pidMustangIII_IV_V_v2{0x0016};

    // Control Data
    inline constexpr std::size_t posDsp{2};
    inline constexpr std::uint8_t posFxKnob{3};
    inline constexpr std::uint8_t posSaveField{4};
    inline constexpr std::size_t posSlot{4};
}
