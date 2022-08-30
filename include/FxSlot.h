/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2022  offa
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

#include <stdexcept>
#include <cstdint>

namespace plug
{
    class FxSlot
    {
    public:
        constexpr explicit FxSlot(std::uint8_t id)
            : id_(checkRange(id))
        {
        }

        constexpr std::uint8_t id() const
        {
            return id_;
        }

        constexpr bool isFxLoop() const
        {
            return id_ >= 4;
        }

    private:
        constexpr std::uint8_t checkRange(std::uint8_t value) const
        {
            if (value > 7)
            {
                throw std::invalid_argument{"Slot ID out of range: " + std::to_string(value)};
            }
            return value;
        }

        std::uint8_t id_;
    };

}
