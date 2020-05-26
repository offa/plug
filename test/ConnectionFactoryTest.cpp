/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2020  offa
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

#include "com/ConnectionFactory.h"
#include "com/CommunicationException.h"
#include "mocks/LibUsbMocks.h"
#include <gmock/gmock.h>

using namespace plug::com;
using namespace testing;

class ConnectionFactoryTest : public testing::Test
{
protected:
    void SetUp() override
    {
        usbmock = mock::resetUsbMock();
        ignoreClose();
    }

    void TearDown() override
    {
        mock::clearUsbMock();
    }

    void ignoreClose()
    {
        EXPECT_CALL(*usbmock, release_interface(_, _)).Times(AnyNumber());
        EXPECT_CALL(*usbmock, attach_kernel_driver(_, _)).Times(AnyNumber());
        EXPECT_CALL(*usbmock, close(_)).Times(AnyNumber());
        EXPECT_CALL(*usbmock, exit(_)).Times(AnyNumber());
    }


    mock::UsbMock* usbmock = nullptr;
    libusb_device_handle handle{};
    static inline constexpr std::uint16_t vid{0x1ed8};
};

TEST_F(ConnectionFactoryTest, createUsbConnectionOpensDevice)
{

    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, vid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0));

    auto conn = createUsbConnection();
    EXPECT_TRUE(conn->isOpen());
}

TEST_F(ConnectionFactoryTest, createUsbConnectionOpensFirstMatchedDevice)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, vid, _))
        .Times(2)
        .WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, vid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0));

    auto conn = createUsbConnection();
    EXPECT_TRUE(conn->isOpen());
}

TEST_F(ConnectionFactoryTest, createUsbConnectionThrowsIfNoMatchingDeviceFound)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, vid, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(nullptr));

    EXPECT_THROW(createUsbConnection(), CommunicationException);
}
