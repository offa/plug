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

using namespace testing;


template<class T>
void unused(T&&) { }


namespace
{
    class UsbMock
    {
    public:

        MOCK_METHOD1(close, void(libusb_device_handle*));
        MOCK_METHOD3(open_device_with_vid_pid, libusb_device_handle*(libusb_context*, uint16_t, uint16_t));
    };


    std::unique_ptr<UsbMock> usbmock;
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
        return usbmock->open_device_with_vid_pid(ctx, vendor_id, product_id);
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

    void libusb_close(libusb_device_handle* dev_handle)
    {
        usbmock->close(dev_handle);
    }


    struct libusb_device_handle
    {
        char dummy;
    };
}



class MustangTest : public testing::Test
{
protected:

    void SetUp() override
    {
        m = std::make_unique<Mustang>();
        usbmock = std::make_unique<UsbMock>();
    }

    void TearDown() override
    {
        usbmock = nullptr;
    }

    std::unique_ptr<Mustang> m;
    libusb_device_handle handle;
};

TEST_F(MustangTest, stoppingAmpDoesNothingIfNotStartedYet)
{
    m->stop_amp();
}


TEST_F(MustangTest, stoppingAmpClosesConnection)
{
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(_, _, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, close(_));

    char dontCare{'x'};
    m->start_amp(nullptr, &dontCare, nullptr, nullptr);

    m->stop_amp();
}
