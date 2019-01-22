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

#include "data_structs.h"
#include <gmock/gmock.h>

namespace test::matcher
{
    MATCHER_P(EffectIs, value, "")
    {
        return std::tie(value.fx_slot, value.effect_num, value.knob1, value.knob2, value.knob3,
                        value.knob4, value.knob5, value.knob6, value.position)
            == std::tie(arg.fx_slot, arg.effect_num, arg.knob1, arg.knob2, arg.knob3,
                        arg.knob4, arg.knob5, arg.knob6, arg.position);
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
}


