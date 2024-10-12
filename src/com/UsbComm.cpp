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

#include "com/UsbComm.h"
#include "com/CommunicationException.h"
#include <algorithm>
#include <chrono>

#include <iostream>
#include <iomanip>

static void debug_dump(const char* label, std::vector<std::uint8_t> bytes, int *pRetval = NULL)
{
#ifndef NDEBUG
    std::cout << label << ": {";
    std::cout << std::setfill('0') << std::resetiosflags(std::ios::dec) << std::setiosflags(std::ios::hex);
    for (
        std::vector<std::uint8_t>::const_iterator pByte = bytes.begin();
        pByte != bytes.end();
        ++pByte)
    {
        std::cout << " " << std::setw(2) << static_cast<unsigned int>(*pByte);
    }
    std::cout << std::setw(0) << std::setfill(' ') << std::resetiosflags(std::ios::hex) << std::setiosflags(std::ios::dec);
    std::cout << " }";
    if (pRetval != NULL)
    {
        std::cout << "returning " << *pRetval;
    }
    std::cout << std::endl;
#endif
}

namespace plug::com
{
    namespace
    {
        inline constexpr std::uint8_t endpointSend{0x01};
        inline constexpr std::uint8_t endpointRecv{0x81};

        usb::Device openDevice(usb::Device&& device)
        {
            device.open();
            return std::move(device);
        }
    }

    UsbComm::UsbComm(usb::Device device)
        : device_(openDevice(std::move(device))), name_(device_.name())
    {
    }

    void UsbComm::close()
    {
        device_.close();
    }

    bool UsbComm::isOpen() const
    {
        return device_.isOpen();
    }

    std::vector<std::uint8_t> UsbComm::receive(std::size_t recvSize)
    {
        auto retval = device_.receive(endpointRecv, recvSize);

        debug_dump("UsbComm::receive", retval);

        return retval;
    }

    std::string UsbComm::name() const
    {
        return name_;
    }

    std::size_t UsbComm::sendImpl(std::uint8_t* data, std::size_t size)
    {
        auto retval = device_.write(endpointSend, data, size);

        debug_dump("UsbComm::sendImpl", std::vector<uint8_t>(data, data + size));

        return retval;
    }
}
