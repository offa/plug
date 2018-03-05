/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2018  offa
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
    constexpr std::size_t packetSize{64};
    constexpr std::size_t nameLength{32};

    // Effects
    constexpr std::size_t posEffect{16};
    constexpr std::size_t posFxSlot{18};
    constexpr std::size_t posKnob1{32};
    constexpr std::size_t posKnob2{33};
    constexpr std::size_t posKnob3{34};
    constexpr std::size_t posKnob4{35};
    constexpr std::size_t posKnob5{36};
    constexpr std::size_t posKnob6{37};

    // Amp
    constexpr std::size_t ampPos{16};
    constexpr std::size_t volumePos{32};
    constexpr std::size_t gainPos{33};
    constexpr std::size_t treblePos{36};
    constexpr std::size_t middlePos{37};
    constexpr std::size_t bassPos{38};
    constexpr std::size_t cabinetPos{49};
    constexpr std::size_t noiseGatePos{47};
    constexpr std::size_t thresholdPos{48};
    constexpr std::size_t masterVolPos{35};
    constexpr std::size_t gain2Pos{34};
    constexpr std::size_t presencePos{39};
    constexpr std::size_t depthPos{41};
    constexpr std::size_t biasPos{42};
    constexpr std::size_t sagPos{51};
    constexpr std::size_t brightnessPos{52};
    constexpr std::size_t usbGainPos{16};

    // USB Data
    constexpr std::uint8_t endpointSend{0x01};
    constexpr std::uint8_t endpointReceive{0x81};
    constexpr std::uint16_t usbVid{0x1ed8};
    constexpr std::uint16_t pidMustangI_II{0x0004};
    constexpr std::uint16_t pidMustangIII_IV_V{0x0005};
    constexpr std::uint16_t pidMustangMini{0x0010};
    constexpr std::uint16_t pidMustangFloor{0x0012};
    constexpr std::uint16_t pidMustangI_II_v2{0x0014};
    constexpr std::uint16_t pidMustangIII_IV_V_v2{0x0016};

    // Control Data
    constexpr std::size_t posDsp{2};
    constexpr std::uint8_t posFxKnob{3};
    constexpr std::uint8_t posSaveField{4};
    constexpr std::size_t posSlot{4};

}
