/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2019  offa
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

        inline constexpr std::chrono::milliseconds timeout{500};

        inline constexpr std::uint8_t endpointSend{0x01};
        inline constexpr std::uint8_t endpointRecv{0x81};


        void checked(int rtnValue, const std::string& msg)
        {
            if (rtnValue != LIBUSB_SUCCESS)
            {
                throw CommunicationException{msg};
            }
        }
    }

    UsbComm::UsbComm()
        : handle(nullptr)
    {
    }

    UsbComm::~UsbComm()
    {
        this->close();
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
            throw CommunicationException{"Failed to open usb device"};
        }

        initInterface();
    }

    void UsbComm::close()
    {
        if (handle != nullptr)
        {
            if (libusb_release_interface(handle, 0) != LIBUSB_ERROR_NO_DEVICE)
            {
                libusb_attach_kernel_driver(handle, 0);
            }

            libusb_close(handle);
            libusb_exit(nullptr);
            handle = nullptr;
        }
    }

    bool UsbComm::isOpen() const
    {
        return (handle != nullptr);
    }

    std::vector<std::uint8_t> UsbComm::receive(std::size_t recvSize)
    {
        int actualTransfered{0};
        std::vector<std::uint8_t> buffer(recvSize);
        const auto rtn = libusb_interrupt_transfer(handle, endpointRecv, buffer.data(), static_cast<int>(buffer.size()), &actualTransfered, timeout.count());

        if (rtn != LIBUSB_ERROR_TIMEOUT)
        {
            checked(rtn, "Interrupt receive failed");
        }

        buffer.resize(static_cast<std::size_t>(actualTransfered));

        return buffer;
    }

    std::size_t UsbComm::sendImpl(std::uint8_t* data, std::size_t size)
    {
        int actualTransfered{0};
        const auto rtn = libusb_interrupt_transfer(handle, endpointSend, data, static_cast<int>(size), &actualTransfered, timeout.count());
        checked(rtn, "Interrupt write failed");

        return static_cast<std::size_t>(actualTransfered);
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
