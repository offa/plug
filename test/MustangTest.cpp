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

#include <gmock/gmock.h>
#include "mustang.h"

template<class T>
void unused(T&&) { }


namespace
{
    class UsbMock
    {
    public:

        MOCK_METHOD1(close, void(libusb_device_handle*));
    };
}


// Stubs
extern "C"
{
    int libusb_init(libusb_context **ctx)
    {
        unused(ctx);
        return 0;
    }

    void libusb_exit(libusb_context *ctx)
    {
        unused(ctx);
    }

    libusb_device_handle*  libusb_open_device_with_vid_pid(libusb_context *ctx, uint16_t vendor_id, uint16_t product_id)
    {
        unused(ctx);
        unused(vendor_id);
        unused(product_id);
        return nullptr;
    }

    int libusb_interrupt_transfer(libusb_device_handle *dev_handle, unsigned char endpoint,
                                unsigned char *data, int length, int *actual_length, unsigned int timeout)
    {
        unused(dev_handle);
        unused(endpoint);
        unused(data);
        unused(length);
        unused(actual_length);
        unused(timeout);
        return 0;
    }

    int libusb_claim_interface(libusb_device_handle *dev_handle, int interface_number)
    {
        unused(dev_handle);
        unused(interface_number);
        return 0;
    }

    int libusb_detach_kernel_driver(libusb_device_handle *dev_handle, int interface_number)
    {
        unused(dev_handle);
        unused(interface_number);
        return 0;
    }

    int libusb_kernel_driver_active(libusb_device_handle *dev_handle, int interface_number)
    {
        unused(dev_handle);
        unused(interface_number);
        return 0;
    }

    int libusb_release_interface(libusb_device_handle *dev_handle, int interface_number)
    {
        unused(dev_handle);
        unused(interface_number);
        return 0;
    }

    int libusb_attach_kernel_driver(libusb_device_handle *dev_handle, int interface_number)
    {
        unused(dev_handle);
        unused(interface_number);
        return 0;
    }

    void libusb_close(libusb_device_handle *dev_handle)
    {
        unused(dev_handle);
    }

}



class MustangTest : public testing::Test
{
};

TEST_F(MustangTest, stoppingAmpCleansUpConnection)
{
    Mustang m;
}

