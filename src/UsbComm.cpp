/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2018  offa
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

#include "UsbComm.h"
#include <algorithm>
#include <chrono>
#include <libusb-1.0/libusb.h>

namespace plug
{
    namespace
    {
        constexpr std::chrono::milliseconds timeout{500};

        void checked(int rtnValue, const std::string& msg)
        {
            if (rtnValue != LIBUSB_SUCCESS)
            {
                throw UsbException{msg};
            }
        }
    }

    void UsbComm::open(std::uint16_t vid, std::uint16_t pid)
    {
        openFirst(vid, {pid});
    }

    void UsbComm::openFirst(std::uint16_t vid, std::initializer_list<std::uint16_t> pids)
    {
        libusb_init(nullptr);

        std::find_if(pids.begin(), pids.end(), [this, vid](const auto& pid) {
            handle = libusb_open_device_with_vid_pid(nullptr, vid, pid);
            return (handle != nullptr);
        });

        if (handle == nullptr)
        {
            throw UsbException{"Failed to open usb device"};
        }

        initInterface();
    }

    void UsbComm::close()
    {
        const auto result = libusb_release_interface(handle, 0);

        if (result != LIBUSB_ERROR_NO_DEVICE)
        {
            libusb_attach_kernel_driver(handle, 0);
        }

        libusb_close(handle);
        handle = nullptr;
        libusb_exit(nullptr);
    }

    bool UsbComm::isOpen() const
    {
        return ( handle != nullptr );
    }

    std::int32_t UsbComm::interruptWrite(std::uint8_t endpoint, std::vector<std::uint8_t> data)
    {
        int actualTransfered{0};
        libusb_interrupt_transfer(handle, endpoint, data.data(), data.size(), &actualTransfered, timeout.count());
        return std::int32_t{actualTransfered};
    }

    std::vector<std::uint8_t> UsbComm::interruptReceive(std::uint8_t endpoint, std::size_t recvSize)
    {
        int actualTransfered{0};
        std::vector<std::uint8_t> buffer(recvSize);
        libusb_interrupt_transfer(handle, endpoint, buffer.data(), buffer.size(), &actualTransfered, timeout.count());
        buffer.resize(actualTransfered);

        return buffer;
    }

    void UsbComm::initInterface()
    {
        if (libusb_kernel_driver_active(handle, 0) != LIBUSB_SUCCESS)
        {
            checked(libusb_detach_kernel_driver(handle, 0), "Detaching kernel driver failed");
        }

        checked(libusb_claim_interface(handle, 0), "Claiming interface failed");
    }
}
