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

#include "com/UsbContext.h"
#include "com/UsbException.h"
#include <algorithm>
#include <libusb-1.0/libusb.h>

namespace plug::com::usb
{
    Context::Context()
    {
        init();
    }

    Context::~Context()
    {
        deinit();
    }

    void Context::init()
    {
        if (const int status = libusb_init(nullptr); status != LIBUSB_SUCCESS)
        {
            throw UsbException{status};
        }
    }

    void Context::deinit()
    {
        libusb_exit(nullptr);
    }


    std::vector<Device> listDevices()
    {
        libusb_device** devices;
        const auto n = libusb_get_device_list(nullptr, &devices);

        if (n < 0)
        {
            throw UsbException(n);
        }

        std::vector<Device> devicesFound;
        devicesFound.reserve(n);

        std::for_each(devices, std::next(devices, n), [&devicesFound](auto* dev) {
            try
            {
                devicesFound.emplace_back(dev);
            }
            catch (const UsbException&)
            {
            }
        });

        libusb_free_device_list(devices, 1);
        return devicesFound;
    }
}
