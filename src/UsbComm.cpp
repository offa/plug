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
    void UsbComm::open(std::uint16_t vid, std::uint16_t pid)
    {
        handle = libusb_open_device_with_vid_pid(nullptr, vid, pid);
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
