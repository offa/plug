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

#include "com/UsbContext.h"
#include "com/UsbDevice.h"
#include <gmock/gmock.h>

namespace plug::test::mock
{
    struct UsbContextMock
    {
        MOCK_METHOD(std::vector<plug::com::usb::Device>, listDevices, ());
    };

    UsbContextMock* resetUsbContextMock();
    UsbContextMock* getUsbContextMock();
    void clearUsbContextMock();


    struct UsbDeviceMock
    {
        MOCK_METHOD(void, open, ());
        MOCK_METHOD(void, close, ());
        MOCK_METHOD(bool, isOpen, (), (const, noexcept));
        MOCK_METHOD(std::uint16_t, vendorId, (), (const noexcept));
        MOCK_METHOD(std::uint16_t, productId, (), (const noexcept));
        MOCK_METHOD(std::size_t, write, (std::uint8_t, std::uint8_t*, std::size_t));
        MOCK_METHOD(std::vector<std::uint8_t>, receive, (std::uint8_t, std::size_t));
        MOCK_METHOD(std::string, name, ());
    };


    UsbDeviceMock* resetUsbDeviceMock();
    UsbDeviceMock* getUsbDeviceMock();
    void clearUsbDeviceMock();

}
