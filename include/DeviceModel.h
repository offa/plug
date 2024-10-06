/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2024  offa
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

#include <string>

namespace plug
{
    class DeviceModel
    {
    public:
        enum class Category
        {
            MustangV1,
            MustangV2,

            // Mustang LT 25, LT 40S, LT 50, Rumble LT 25
            // All of these are interoperable with Windows/macOS FenderTone
            MustangV3_LT,

            // Mustang Micro
            // Not documented as interoperable with any FenderTone,
            // but it has a USB interface, so why not see if it 
            // can be controlled
            MustangV3_Micro,

            // Mustang GT 40
            // Not sure whether the FenderTone for this is desktop or mobile
            // Keep it separate for now can,  merge if it turns out to be
            // sufficiently similar to LT
            MustangV3_GT,


            Other
        };

        DeviceModel(const std::string& name, Category category, std::size_t numberPresets)
            : name_(name), category_(category), numberPresets_(numberPresets)
        {
        }

        std::string name() const
        {
            return name_;
        }
        Category category() const
        {
            return category_;
        }
        std::size_t numberOfPresets() const
        {
            return numberPresets_;
        }

    private:
        std::string name_;
        Category category_;
        std::size_t numberPresets_;
    };
}
