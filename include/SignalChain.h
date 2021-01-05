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
#include "effects_enum.h"
#include <string>
#include <tuple>
#include <array>

namespace plug
{

    class SignalChain
    {
    public:
        SignalChain()
            : name_(""), amp_(), effects_()
        {
        }
        SignalChain(const std::string& name, amp_settings amp, std::array<fx_pedal_settings, 4> effects)
            : name_(name), amp_(amp), effects_(effects)
        {
        }


        std::string name() const
        {
            return name_;
        }

        void setName(const std::string& name)
        {
            name_ = name;
        }

        amp_settings amp() const
        {
            return amp_;
        }

        void setAmp(amp_settings amp)
        {
            amp_ = amp;
        }

        std::array<fx_pedal_settings, 4> effects() const
        {
            return effects_;
        }

        void setEffects(std::array<fx_pedal_settings, 4> effects)
        {
            effects_ = effects;
        }


    private:
        std::string name_;
        amp_settings amp_;
        std::array<fx_pedal_settings, 4> effects_;
    };

}
