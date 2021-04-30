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

#include "UsbDeviceMock.h"
#include <memory>

namespace mock
{
    static std::unique_ptr<UsbContextMock> usbContextMock;
    static std::unique_ptr<UsbDeviceMock> usbDeviceMock;


    UsbContextMock* resetUsbContextMock()
    {
        usbContextMock = std::make_unique<UsbContextMock>();
        return getUsbContextMock();
    }

    UsbContextMock* getUsbContextMock()
    {
        if (usbContextMock == nullptr)
        {
            throw std::logic_error{"Usb Device Mock not initialized."};
        }
        return usbContextMock.get();
    }

    void clearUsbContextMock()
    {
        usbContextMock = nullptr;
    }

    UsbDeviceMock* resetUsbDeviceMock()
    {
        usbDeviceMock = std::make_unique<UsbDeviceMock>();
        return getUsbDeviceMock();
    }

    UsbDeviceMock* getUsbDeviceMock()
    {
        if (usbDeviceMock == nullptr)
        {
            throw std::logic_error{"Usb Device Mock not initialized."};
        }
        return usbDeviceMock.get();
    }

    void clearUsbDeviceMock()
    {
        usbDeviceMock.reset();
    }
}


namespace plug::com::usb
{
    namespace detail
    {
        void releaseDevice([[maybe_unused]] libusb_device* device)
        {
        }

        void releaseHandle([[maybe_unused]] libusb_device_handle* handle)
        {
        }
    }

    std::vector<Device> listDevices()
    {
        return mock::usbContextMock->listDevices();
    }


    Device::Device(libusb_device* device)
        : device_(device), handle_(nullptr), descriptor_({})
    {
    }

    void Device::open()
    {
        mock::usbDeviceMock->open();
    }

    void Device::close()
    {
        mock::usbDeviceMock->close();
    }

    bool Device::isOpen() const noexcept
    {
        return mock::usbDeviceMock->isOpen();
    }

    std::uint16_t Device::vendorId() const noexcept
    {
        return mock::usbDeviceMock->vendorId();
    }

    std::uint16_t Device::productId() const noexcept
    {
        return mock::usbDeviceMock->productId();
    }

    std::size_t Device::write(std::uint8_t endpoint, std::uint8_t* data, std::size_t dataSize)
    {
        return mock::usbDeviceMock->write(endpoint, data, dataSize);
    }

    std::vector<std::uint8_t> Device::receive(std::uint8_t endpoint, std::size_t dataSize)
    {
        return mock::usbDeviceMock->receive(endpoint, dataSize);
    }

}
