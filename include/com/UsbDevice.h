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
#include <vector>
#include <cstdint>
#include <memory>

struct libusb_device;
struct libusb_device_handle;

namespace plug::com::usb
{
    template <auto Fn>
    struct ReleaseFunction
    {
        template <class T>
        void operator()(T* ptr)
        {
            Fn(ptr);
        }
    };

    template <class T, auto Fn>
    using Ressource = std::unique_ptr<T, ReleaseFunction<Fn>>;


    namespace detail
    {
        void releaseDevice(libusb_device* device);
        void releaseHandle(libusb_device_handle* handle);
    }


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
        std::string name() const;

        std::size_t write(std::uint8_t endpoint, std::uint8_t* data, std::size_t dataSize);
        std::vector<std::uint8_t> receive(std::uint8_t endpoint, std::size_t dataSize);

        Device& operator=(Device&&) = default;


    private:
        struct Descriptor
        {
            std::uint16_t vid;
            std::uint16_t pid;
            std::uint8_t stringDescriptorIndex;
        };

        Descriptor getDeviceDescriptor(libusb_device* device) const;

        Ressource<libusb_device, detail::releaseDevice> device_;
        Ressource<libusb_device_handle, detail::releaseHandle> handle_;
        Descriptor descriptor_;
    };
}
