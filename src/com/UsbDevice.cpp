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

#include "com/UsbDevice.h"
#include "com/UsbException.h"
#include <array>
#include <libusb-1.0/libusb.h>

namespace plug::com::usb
{
    Device::Device(libusb_device* device)
        : device_(libusb_ref_device(device)), handle_(nullptr), descriptor_(getDeviceDescriptor(device))
    {
    }

    Device::~Device()
    {
        close();
        libusb_unref_device(device_);
    }

    void Device::open()
    {
        if (const int result = libusb_open(device_, &handle_); result != LIBUSB_SUCCESS)
        {
            throw UsbException{result};
        }
    }

    void Device::close()
    {
        if (handle_ != nullptr)
        {
            libusb_close(handle_);
            handle_ = nullptr;
        }
    }

    bool Device::isOpen() const noexcept
    {
        return handle_ != nullptr;
    }

    std::uint16_t Device::vendorId() const noexcept
    {
        return descriptor_.vid;
    }

    std::uint16_t Device::productId() const noexcept
    {
        return descriptor_.pid;
    }

    std::string Device::name() const
    {
        std::array<std::uint8_t, 256> buffer;
        const int n = libusb_get_string_descriptor_ascii(handle_, descriptor_.stringDescriptorIndex, buffer.data(), buffer.size());

        if (n < 0)
        {
            throw UsbException{n};
        }
        return std::string{buffer.cbegin(), std::next(buffer.cbegin(), n)};
    }

    Device::Descriptor Device::getDeviceDescriptor(libusb_device* device) const
    {
        libusb_device_descriptor descriptor;

        if (const auto result = libusb_get_device_descriptor(device, &descriptor); result != LIBUSB_SUCCESS)
        {
            libusb_unref_device(device_);
            throw UsbException{result};
        }
        return {descriptor.idVendor, descriptor.idProduct, descriptor.iProduct};
    }

}
