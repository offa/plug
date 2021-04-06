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
#include "mocks/LibUsbMocks.h"
#include <libusb-1.0/libusb.h>
#include <gmock/gmock.h>

using namespace testing;
using namespace plug::com::usb;

class UsbTest : public testing::Test
{
protected:
    void SetUp() override
    {
        usbmock = mock::resetUsbMock();
    }

    void TearDown() override
    {
        mock::clearUsbMock();
    }

    mock::UsbMock* usbmock = nullptr;
};

TEST_F(UsbTest, contextCtorInitializesDefaultContext)
{
    EXPECT_CALL(*usbmock, init(nullptr)).WillOnce(Return(LIBUSB_SUCCESS));
    EXPECT_CALL(*usbmock, exit(_));

    Context context;
}

TEST_F(UsbTest, contextCtorThrowsOnInitError)
{
    EXPECT_CALL(*usbmock, init(nullptr)).WillOnce(Return(LIBUSB_ERROR_OTHER));
    EXPECT_CALL(*usbmock, error_name(LIBUSB_ERROR_OTHER)).WillOnce(Return("ignore_name"));
    EXPECT_CALL(*usbmock, strerror(LIBUSB_ERROR_OTHER)).WillOnce(Return("ignore_message"));

    EXPECT_THROW(Context{}, UsbException);
}

TEST_F(UsbTest, contextDtorDeinitializesDefaultContext)
{
    EXPECT_CALL(*usbmock, init(_)).WillOnce(Return(LIBUSB_SUCCESS));
    EXPECT_CALL(*usbmock, exit(nullptr));

    Context ctx{};
}

TEST_F(UsbTest, exceptionContainsErrorInformation)
{
    EXPECT_CALL(*usbmock, error_name(_)).WillOnce(Return("name___LIBUSB_ERROR_NO_DEVICE"));
    EXPECT_CALL(*usbmock, strerror(_)).WillOnce(Return("message___LIBUSB_ERROR_NO_DEVICE"));

    const UsbException ex{LIBUSB_ERROR_NO_DEVICE};
    EXPECT_THAT(ex.code(), Eq(LIBUSB_ERROR_NO_DEVICE));
    EXPECT_THAT(ex.name(), StrEq("name___LIBUSB_ERROR_NO_DEVICE"));
    EXPECT_THAT(ex.message(), StrEq("message___LIBUSB_ERROR_NO_DEVICE"));
    EXPECT_THAT(ex.what(), StrEq("message___LIBUSB_ERROR_NO_DEVICE"));
}

TEST_F(UsbTest, listDevicesEmptyIfNoDevices)
{
    EXPECT_CALL(*usbmock, get_device_list(nullptr, _)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, free_device_list(_, _));

    const auto devices = listDevices();
    EXPECT_THAT(devices, IsEmpty());
}

TEST_F(UsbTest, listDevicesThrowsOnDeviceListError)
{
    EXPECT_CALL(*usbmock, error_name(_)).WillOnce(Return("ignore_name"));
    EXPECT_CALL(*usbmock, strerror(_)).WillOnce(Return("ignore_message"));

    EXPECT_CALL(*usbmock, get_device_list(_, _)).WillOnce(Return(LIBUSB_ERROR_OTHER));
    EXPECT_THROW(listDevices(), UsbException);
}

TEST_F(UsbTest, listDevicesReturnsDevices)
{
    EXPECT_CALL(*usbmock, ref_device(_)).Times(2);
    EXPECT_CALL(*usbmock, unref_device(_)).Times(2);

    libusb_device device0;
    libusb_device device1;
    std::array<libusb_device*, 2> deviceList{&device0, &device1};
    EXPECT_CALL(*usbmock, get_device_list(nullptr, NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(deviceList.data()),
                        Return(deviceList.size())));
    libusb_device_descriptor descr0;
    descr0.idVendor = 0xabcd;
    descr0.idProduct = 0x1221;
    libusb_device_descriptor descr1;
    descr1.idVendor = 0xeeff;
    descr1.idProduct = 0x0202;
    EXPECT_CALL(*usbmock, get_device_descriptor(NotNull(), NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(descr0), Return(LIBUSB_SUCCESS)))
        .WillOnce(DoAll(SetArgPointee<1>(descr1), Return(LIBUSB_SUCCESS)));
    EXPECT_CALL(*usbmock, free_device_list(_, _));

    const auto devices = listDevices();
    EXPECT_THAT(devices, SizeIs(2));
    EXPECT_THAT(devices[0].vendorId(), Eq(0xabcd));
    EXPECT_THAT(devices[0].productId(), Eq(0x1221));
    EXPECT_THAT(devices[1].vendorId(), Eq(0xeeff));
    EXPECT_THAT(devices[1].productId(), Eq(0x0202));
}

TEST_F(UsbTest, listDevicesUnrefsList)
{
    EXPECT_CALL(*usbmock, ref_device(_));
    EXPECT_CALL(*usbmock, unref_device(_));

    libusb_device device0;
    std::array<libusb_device*, 1> deviceList{&device0};
    EXPECT_CALL(*usbmock, get_device_list(nullptr, NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(deviceList.data()),
                        Return(deviceList.size())));
    EXPECT_CALL(*usbmock, get_device_descriptor(NotNull(), NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(libusb_device_descriptor{}), Return(LIBUSB_SUCCESS)));
    EXPECT_CALL(*usbmock, free_device_list(NotNull(), 1));

    listDevices();
}

TEST_F(UsbTest, listDevicesSkipsDeviceOnFailingDescriptor)
{
    EXPECT_CALL(*usbmock, ref_device(_)).Times(2);
    EXPECT_CALL(*usbmock, unref_device(_)).Times(2);

    libusb_device device0;
    libusb_device device1;
    std::array<libusb_device*, 2> deviceList{&device0, &device1};
    EXPECT_CALL(*usbmock, get_device_list(nullptr, NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(deviceList.data()),
                        Return(deviceList.size())));
    libusb_device_descriptor descr0;
    descr0.idVendor = 0x1234;
    descr0.idProduct = 0x5678;
    libusb_device_descriptor descr1;
    descr1.idVendor = 0x0ff0;
    descr1.idProduct = 0x0ee0;
    EXPECT_CALL(*usbmock, get_device_descriptor(NotNull(), NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(descr0), Return(0)))
        .WillOnce(DoAll(SetArgPointee<1>(descr1), Return(LIBUSB_ERROR_ACCESS)));
    EXPECT_CALL(*usbmock, free_device_list(_, _));
    EXPECT_CALL(*usbmock, error_name(LIBUSB_ERROR_ACCESS)).WillOnce(Return("ignore_name"));
    EXPECT_CALL(*usbmock, strerror(LIBUSB_ERROR_ACCESS)).WillOnce(Return("ignore_message"));

    const auto devices = listDevices();
    EXPECT_THAT(devices, SizeIs(1));
    EXPECT_THAT(devices[0].vendorId(), Eq(0x1234));
}

TEST_F(UsbTest, deviceRefsDevice)
{
    libusb_device dev;
    EXPECT_CALL(*usbmock, ref_device(&dev)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));

    Device device{&dev, 0xaaaa, 0xbbbb, 0x01};
}

TEST_F(UsbTest, deviceUnRefsDeviceOnDestruction)
{
    libusb_device dev;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(&dev));

    Device device{&dev, 0xaaaa, 0xbbbb, 0x01};
}

TEST_F(UsbTest, deviceOpen)
{
    libusb_device dev;
    libusb_device_handle dummy;
    libusb_device_handle* handle = &dummy;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(&dev, NotNull()))
        .WillOnce(DoAll(SetArgPointee<1>(handle), Return(LIBUSB_SUCCESS)));
    EXPECT_CALL(*usbmock, close(_));

    Device device{&dev, 0x1111, 0x2222, 0x01};
    device.open();
    EXPECT_THAT(device.isOpen(), IsTrue());
}

TEST_F(UsbTest, deviceOpenThrowsOnError)
{
    libusb_device dev;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(&dev, NotNull())).WillOnce(Return(LIBUSB_ERROR_ACCESS));
    EXPECT_CALL(*usbmock, error_name(LIBUSB_ERROR_ACCESS)).WillOnce(Return("ignore_name"));
    EXPECT_CALL(*usbmock, strerror(LIBUSB_ERROR_ACCESS)).WillOnce(Return("ignore_message"));

    Device device{&dev, 0x1111, 0x2222, 0x01};
    EXPECT_THROW(device.open(), UsbException);
}

TEST_F(UsbTest, deviceIsOpenReturnsFalseIfNotOpen)
{
    libusb_device dev;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));

    Device device{&dev, 0x1111, 0x2222, 0x01};
    EXPECT_THAT(device.isOpen(), IsFalse());
}

TEST_F(UsbTest, deviceCloseClosesOpenDevice)
{
    libusb_device dev;
    libusb_device_handle dummy;
    libusb_device_handle* handle = &dummy;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(handle), Return(LIBUSB_SUCCESS)));
    EXPECT_CALL(*usbmock, close(handle));

    Device device{&dev, 0x1111, 0x2222, 0x01};
    device.open();
    EXPECT_THAT(device.isOpen(), IsTrue());
    device.close();
    EXPECT_THAT(device.isOpen(), IsFalse());
}

TEST_F(UsbTest, deviceCloseIgnoresNotOpen)
{
    libusb_device dev;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));

    Device device{&dev, 0x1111, 0x2222, 0x01};
    device.close();
    EXPECT_THAT(device.isOpen(), IsFalse());
}

TEST_F(UsbTest, deviceIsClosedOnDestruction)
{
    libusb_device dev;
    libusb_device_handle dummy;
    libusb_device_handle* handle = &dummy;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(handle), Return(LIBUSB_SUCCESS)));
    EXPECT_CALL(*usbmock, close(handle));

    Device device{&dev, 0x1111, 0x2222, 0x01};
    device.open();
}

TEST_F(UsbTest, deviceNameReturnsNameIfOpen)
{
    libusb_device dev;
    libusb_device_handle dummy;
    libusb_device_handle* handle = &dummy;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(handle), Return(LIBUSB_SUCCESS)));
    std::string nameBuffer = "usb-device-0";
    EXPECT_CALL(*usbmock, get_string_descriptor_ascii(handle, 0xce, NotNull(), 256))
        .WillOnce(DoAll(SetArrayArgument<2>(nameBuffer.data(), nameBuffer.data() + nameBuffer.size()), Return(nameBuffer.size())));
    EXPECT_CALL(*usbmock, close(_));

    Device device{&dev, 0xabcd, 0xeeff, 0xce};
    device.open();
    EXPECT_THAT(device.name(), StrEq("usb-device-0"));
}

TEST_F(UsbTest, deviceNameReturnsEmptyStringOfZeroLengthName)
{
    libusb_device dev;
    libusb_device_handle dummy;
    libusb_device_handle* handle = &dummy;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(handle), Return(LIBUSB_SUCCESS)));
    std::string nameBuffer = "usb-device-0";
    EXPECT_CALL(*usbmock, get_string_descriptor_ascii(handle, 0xee, NotNull(), 256))
        .WillOnce(DoAll(SetArrayArgument<2>(nameBuffer.data(), nameBuffer.data() + nameBuffer.size()), Return(0)));
    EXPECT_CALL(*usbmock, close(_));

    Device device{&dev, 0x3333, 0xaaaa, 0xee};
    device.open();
    EXPECT_THAT(device.name(), IsEmpty());
}

TEST_F(UsbTest, deviceNameThrowsOnError)
{
    libusb_device dev;
    libusb_device_handle dummy;
    libusb_device_handle* handle = &dummy;
    EXPECT_CALL(*usbmock, ref_device(_)).WillOnce(Return(&dev));
    EXPECT_CALL(*usbmock, unref_device(_));
    EXPECT_CALL(*usbmock, open(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(handle), Return(LIBUSB_SUCCESS)));
    std::string nameBuffer = "usb-device-0";
    EXPECT_CALL(*usbmock, get_string_descriptor_ascii(handle, 0xde, NotNull(), 256))
        .WillOnce(DoAll(SetArrayArgument<2>(nameBuffer.data(), nameBuffer.data() + nameBuffer.size()), Return(LIBUSB_ERROR_TIMEOUT)));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, error_name(LIBUSB_ERROR_TIMEOUT)).WillOnce(Return("ignore_name"));
    EXPECT_CALL(*usbmock, strerror(LIBUSB_ERROR_TIMEOUT)).WillOnce(Return("ignore_message"));

    Device device{&dev, 0x0011, 0x2222, 0xde};
    device.open();
    EXPECT_THROW(device.name(), UsbException);
}
