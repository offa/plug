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

#include <vector>
#include <cstdint>

namespace plug::com
{

    class Connection
    {
    public:
        virtual ~Connection() = default;

        virtual void close() = 0;
        virtual bool isOpen() const = 0;

        template <class Container>
        std::size_t send(Container c)
        {
            return sendImpl(c.data(), c.size());
        }

        virtual std::vector<std::uint8_t> receive(std::size_t recvSize) = 0;

    private:
        virtual std::size_t sendImpl(std::uint8_t* data, std::size_t size) = 0;
    };
}
