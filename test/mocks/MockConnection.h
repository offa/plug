/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2025  offa
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

namespace plug::test::mock
{
    class MockConnection : public plug::com::Connection
    {
    public:
        virtual ~MockConnection()
        {
        }

        MOCK_METHOD(void, open, (std::uint16_t, std::uint16_t) );
        MOCK_METHOD(void, openFirst, (std::uint16_t, std::initializer_list<std::uint16_t>) );
        MOCK_METHOD(void, close, ());
        MOCK_METHOD(bool, isOpen, (), (const));
        MOCK_METHOD(std::vector<std::uint8_t>, receive, (std::size_t) );
        MOCK_METHOD(std::size_t, sendImpl, (std::uint8_t*, std::size_t) );
        MOCK_METHOD(std::string, name, (), (const));
    };
}
