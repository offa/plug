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

#include "LibUsbMocks.h"
#include "com/LibUsbCompat.h"
#include <stdexcept>

namespace mock
{
    static std::unique_ptr<UsbMock> usbmock;


    UsbMock* getUsbMock()
    {
        if (usbmock == nullptr)
        {
            throw std::logic_error{"Usb Mock not initialized."};
        }
        return usbmock.get();
    }

    UsbMock* resetUsbMock()
    {
        usbmock = std::make_unique<UsbMock>();
        return getUsbMock();
    }


    void clearUsbMock()
    {
        usbmock.reset();
    }
}


extern "C"
{
    int libusb_init(libusb_context** ctx)
    {
        return mock::getUsbMock()->init(ctx);
    }

    void libusb_exit(libusb_context* ctx)
    {
        mock::getUsbMock()->exit(ctx);
    }

    libusb_device_handle* libusb_open_device_with_vid_pid(libusb_context* ctx, uint16_t vendor_id, uint16_t product_id)
    {
        return mock::getUsbMock()->open_device_with_vid_pid(ctx, vendor_id, product_id);
    }

    int libusb_interrupt_transfer(libusb_device_handle* dev_handle, unsigned char endpoint,
                                  unsigned char* data, int length, int* actual_length, unsigned int timeout)
    {
        return mock::getUsbMock()->interrupt_transfer(dev_handle, endpoint, data, length, actual_length, timeout);
    }

    int libusb_claim_interface(libusb_device_handle* dev_handle, int interface_number)
    {
        return mock::getUsbMock()->claim_interface(dev_handle, interface_number);
    }

    int libusb_detach_kernel_driver(libusb_device_handle* dev_handle, int interface_number)
    {
        return mock::getUsbMock()->detach_kernel_driver(dev_handle, interface_number);
    }

    int libusb_kernel_driver_active(libusb_device_handle* dev_handle, int interface_number)
    {
        return mock::getUsbMock()->kernel_driver_active(dev_handle, interface_number);
    }

    int libusb_attach_kernel_driver(libusb_device_handle* dev_handle, int interface_number)
    {
        return mock::getUsbMock()->attach_kernel_driver(dev_handle, interface_number);
    }

    int libusb_set_auto_detach_kernel_driver(libusb_device_handle* dev_handle, int enable)
    {
        return mock::getUsbMock()->set_auto_detach_kernel_driver(dev_handle, enable);
    }

    void libusb_close(libusb_device_handle* dev_handle)
    {
        mock::getUsbMock()->close(dev_handle);
    }

    int libusb_release_interface(libusb_device_handle* dev_handle, int interface_number)
    {
        return mock::getUsbMock()->release_interface(dev_handle, interface_number);
    }

    const char* libusb_error_name(int error_code)
    {
        return mock::getUsbMock()->error_name(error_code);
    }

    ssize_t libusb_get_device_list(libusb_context* ctx, libusb_device*** list)
    {
        return mock::getUsbMock()->get_device_list(ctx, list);
    }

    int libusb_get_device_descriptor(libusb_device* dev, libusb_device_descriptor* desc)
    {
        return mock::getUsbMock()->get_device_descriptor(dev, desc);
    }

    void libusb_free_device_list(libusb_device** list, int unref_devices)
    {
        mock::getUsbMock()->free_device_list(list, unref_devices);
    }

    libusb_device* libusb_ref_device(libusb_device* dev)
    {
        return mock::getUsbMock()->ref_device(dev);
    }

    void libusb_unref_device(libusb_device* dev)
    {
        return mock::getUsbMock()->unref_device(dev);
    }

    int libusb_open(libusb_device* dev, libusb_device_handle** dev_handle)
    {
        return mock::getUsbMock()->open(dev, dev_handle);
    }

    int libusb_get_string_descriptor_ascii(libusb_device_handle* dev_handle, uint8_t desc_index, unsigned char* data, int length)
    {
        return mock::getUsbMock()->get_string_descriptor_ascii(dev_handle, desc_index, data, length);
    }
}


namespace plug::com::usb::libusb
{
    const char* strerror(ErrorCodeAdapter errorCode)
    {
        return mock::getUsbMock()->strerror(errorCode);
    }
}
