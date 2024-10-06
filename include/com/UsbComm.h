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

#include "com/Connection.h"
#include <com/UsbDevice.h>


namespace plug::com
{

    class UsbComm : public Connection
    {
    public:
        UsbComm(usb::Device device);

        void close() override;
        bool isOpen() const override;

        std::vector<std::uint8_t> receive(std::size_t recvSize) override;

        std::string name() const override;

    private:
        std::size_t sendImpl(std::uint8_t* data, std::size_t size) override;

        usb::Device device_;
        const std::string name_;
    };
}
