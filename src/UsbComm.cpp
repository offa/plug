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
#include <libusb-1.0/libusb.h>

namespace plug
{
    namespace
    {
        void checked(int rtnValue, const std::string& msg)
        {
            if( rtnValue != LIBUSB_SUCCESS )
            {
                throw UsbException{msg};
            }
        }
    }

    void UsbComm::open(std::uint16_t vid, std::uint16_t pid)
    {
        libusb_init(nullptr);
        handle = libusb_open_device_with_vid_pid(nullptr, vid, pid);

        if( handle == nullptr )
        {
            throw UsbException{"Failed to open usb device"};
        }

        if( libusb_kernel_driver_active(handle, 0) != LIBUSB_SUCCESS )
        {
            checked(libusb_detach_kernel_driver(handle, 0), "Detaching kernel driver failed");
        }

        checked(libusb_claim_interface(handle, 0), "Claiming interface failed");
    }

    void UsbComm::close()
    {
        const auto result = libusb_release_interface(handle, 0);

        if (result != LIBUSB_ERROR_NO_DEVICE)
        {
            libusb_attach_kernel_driver(handle, 0);
        }

        libusb_close(handle);
        libusb_exit(nullptr);
    }

}
