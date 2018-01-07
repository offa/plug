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

#include "effects_enum.h"
#include <cstdint>
#include <string>
#include <stdexcept>

namespace plug
{

    constexpr amps lookupAmpById(std::uint8_t id)
    {
        switch(id)
        {
            case 0x67:
                return amps::FENDER_57_DELUXE;
            case 0x64:
                return amps::FENDER_59_BASSMAN;
            case 0x7c:
                return amps::FENDER_57_CHAMP;
            case 0x53:
                return amps::FENDER_65_DELUXE_REVERB;
            case 0x6a:
                return amps::FENDER_65_PRINCETON;
            case 0x75:
                return amps::FENDER_65_TWIN_REVERB;
            case 0x72:
                return amps::FENDER_SUPER_SONIC;
            case 0x61:
                return amps::BRITISH_60S;
            case 0x79:
                return amps::BRITISH_70S;
            case 0x5e:
                return amps::BRITISH_80S;
            case 0x5d:
                return amps::AMERICAN_90S;
            case 0x6d:
                return amps::METAL_2000;
            default:
                throw std::invalid_argument{"Invalid id: " + std::to_string(id)};
        }
    }

}
