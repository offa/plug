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

#include "data_structs.h"
#include "MustangConstants.h"
#include <string>
#include <array>
#include <cstdint>

namespace plug
{
    std::string decodeNameFromData(const unsigned char data[7][64]);
    amp_settings decodeAmpFromData(const unsigned char data[7][64]);
    void decodeEffectsFromData(unsigned char prev_array[4][packetSize], const unsigned char data[7][64], fx_pedal_settings* const& effects_set_out);

    std::array<std::uint8_t, packetSize> serializeAmpSettings(const amp_settings& value);
    std::array<std::uint8_t, packetSize> serializeAmpSettingsUsbGain(const amp_settings& value);
    std::array<std::uint8_t, packetSize> serializeName(std::uint8_t slot, std::string_view name);

}
