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

#include <string>
#include <cstdint>

struct libusb_device;
struct libusb_device_handle;

namespace plug::com::usb
{

    class Device
    {
    public:
        explicit Device(libusb_device* device);
        Device(Device&&) = default;

        ~Device();

        void open();
        void close();
        bool isOpen() const noexcept;

        std::uint16_t vendorId() const noexcept;
        std::uint16_t productId() const noexcept;
        std::string name();

        Device& operator=(Device&&) = default;


    private:
        libusb_device* device_;
        libusb_device_handle* handle_;
        std::uint16_t vid_;
        std::uint16_t pid_;
        std::uint8_t stringDescriptorIndex_;
    };
}
