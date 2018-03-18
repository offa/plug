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

#include <algorithm>
#include <array>

namespace test::helper
{
    using BinData = std::array<std::uint8_t, 64>;


    inline BinData createEmptyPacket()
    {
        BinData data{{0}};
        data.fill(0x00);
        return data;
    }

    inline BinData createInitCmdPacket()
    {
        auto data = createEmptyPacket();
        data[0] = 0xff;
        data[1] = 0xc1;
        return data;
    }

    inline BinData createInitializedPacket(std::initializer_list<std::uint8_t> init)
    {
        auto data = createEmptyPacket();
        std::copy(init.begin(), init.end(), data.begin());
        return data;
    }

    inline BinData createEmptyNamedPacket(const std::string_view name)
    {
        constexpr std::size_t nameFieldOffset{16};
        auto data = createEmptyPacket();
        std::copy(name.cbegin(), name.cend(), std::next(data.begin(), nameFieldOffset));
        return data;
    }
}
