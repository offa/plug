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

#include "com/Connection.h"
#include <gmock/gmock.h>

namespace mock
{
    class MockConnection : public plug::com::Connection
    {
    public:
        virtual ~MockConnection()
        {
        }

        MOCK_METHOD2(open, void(std::uint16_t, std::uint16_t));
        MOCK_METHOD2(openFirst, void(std::uint16_t, std::initializer_list<std::uint16_t>));
        MOCK_METHOD0(close, void());
        MOCK_CONST_METHOD0(isOpen, bool());
        MOCK_METHOD1(receive, std::vector<std::uint8_t>(std::size_t));
        MOCK_METHOD2(sendImpl, std::size_t(std::uint8_t*, std::size_t));
    };
}
