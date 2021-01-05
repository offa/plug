/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2021  offa
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
#include "effects_enum.h"

namespace plug
{

    enum class Position
    {
        input,
        effectsLoop
    };

    struct amp_settings
    {
        amps amp_num;
        std::uint8_t gain;
        std::uint8_t volume;
        std::uint8_t treble;
        std::uint8_t middle;
        std::uint8_t bass;
        cabinets cabinet;
        std::uint8_t noise_gate;
        std::uint8_t master_vol;
        std::uint8_t gain2;
        std::uint8_t presence;
        std::uint8_t threshold;
        std::uint8_t depth;
        std::uint8_t bias;
        std::uint8_t sag;
        bool brightness;
        std::uint8_t usb_gain;
    };

    struct fx_pedal_settings
    {
        std::uint8_t fx_slot;
        effects effect_num;
        std::uint8_t knob1;
        std::uint8_t knob2;
        std::uint8_t knob3;
        std::uint8_t knob4;
        std::uint8_t knob5;
        std::uint8_t knob6;
        Position position;
        bool enabled{true};
    };
}
