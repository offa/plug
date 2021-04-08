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

#include <gmock/gmock.h>
#include <libusb-1.0/libusb.h>

namespace mock
{
    class UsbMock
    {
    public:
        MOCK_METHOD(int, init, (libusb_context**));
        MOCK_METHOD(void, close, (libusb_device_handle*));
        MOCK_METHOD(libusb_device_handle*, open_device_with_vid_pid, (libusb_context*, uint16_t, uint16_t));
        MOCK_METHOD(void, exit, (libusb_context*));
        MOCK_METHOD(int, kernel_driver_active, (libusb_device_handle*, int));
        MOCK_METHOD(int, detach_kernel_driver, (libusb_device_handle*, int));
        MOCK_METHOD(int, attach_kernel_driver, (libusb_device_handle*, int));
        MOCK_METHOD(int, set_auto_detach_kernel_driver, (libusb_device_handle*, int));
        MOCK_METHOD(int, release_interface, (libusb_device_handle*, int));
        MOCK_METHOD(int, claim_interface, (libusb_device_handle*, int));
        MOCK_METHOD(int, interrupt_transfer, (libusb_device_handle*, unsigned char, unsigned char*, int, int*, unsigned int));
        MOCK_METHOD(const char*, error_name, (int));
        MOCK_METHOD(const char*, strerror, (int));
        MOCK_METHOD(ssize_t, get_device_list, (libusb_context*, libusb_device***));
        MOCK_METHOD(int, get_device_descriptor, (libusb_device*, libusb_device_descriptor*));
        MOCK_METHOD(void, free_device_list, (libusb_device**, int));
        MOCK_METHOD(libusb_device*, ref_device, (libusb_device*));
        MOCK_METHOD(void, unref_device, (libusb_device*));
        MOCK_METHOD(int, open, (libusb_device*, libusb_device_handle**));
        MOCK_METHOD(int, get_string_descriptor_ascii, (libusb_device_handle*, uint8_t, unsigned char*, int));
    };

    UsbMock* getUsbMock();
    UsbMock* resetUsbMock();
    void clearUsbMock();
}


extern "C"
{
    struct libusb_device
    {
        uint16_t idVendor;
        uint16_t idProduct;
        uint8_t iProduct;
    };


    struct libusb_device_handle
    {
        char dummy;
    };

}
