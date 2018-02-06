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
#include "LibUsbMocks.h"
#include <gmock/gmock.h>

using plug::UsbComm;
using namespace testing;

class UsbCommTest : public testing::Test
{
protected:
    void SetUp() override
    {
        comm = std::make_unique<UsbComm>();
        usbmock = mock::resetUsbMock();
    }

    void TearDown() override
    {
        mock::clearUsbMock();
    }

    void expectOpen()
    {
        EXPECT_CALL(*usbmock, open_device_with_vid_pid(_, _, _)).WillOnce(Return(&handle));
    }

    std::unique_ptr<UsbComm> comm;
    mock::UsbMock* usbmock;
    libusb_device_handle handle;
};

TEST_F(UsbCommTest, closeClosesConnection)
{
    expectOpen();
    comm->open(0, 0);

    InSequence s;
    EXPECT_CALL(*usbmock, release_interface(&handle, 0));
    EXPECT_CALL(*usbmock, attach_kernel_driver(&handle, 0));
    EXPECT_CALL(*usbmock, close(&handle));
    EXPECT_CALL(*usbmock, exit(nullptr));

    comm->close();
}

TEST_F(UsbCommTest, closeDoesNotReattachDriverIfNoDevice)
{
    expectOpen();
    comm->open(0, 0);

    InSequence s;
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(LIBUSB_ERROR_NO_DEVICE));
    EXPECT_CALL(*usbmock, close(&handle));
    EXPECT_CALL(*usbmock, exit(nullptr));

    comm->close();
}
