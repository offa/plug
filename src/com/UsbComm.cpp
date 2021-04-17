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

#include "com/UsbComm.h"
#include "com/CommunicationException.h"
#include <algorithm>
#include <chrono>
#include <libusb-1.0/libusb.h>

namespace plug::com
{
    namespace
    {
        inline constexpr std::uint8_t endpointSend{0x01};
        inline constexpr std::uint8_t endpointRecv{0x81};
    }

    UsbComm::UsbComm(usb::Device device)
        : device_(std::move(device))
    {
        device_.open();
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
        return device_.receive(endpointRecv, recvSize);
    }

    std::size_t UsbComm::sendImpl(std::uint8_t* data, std::size_t size)
    {
        return device_.write(endpointSend, data, size);
    }
}
