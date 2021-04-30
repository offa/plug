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

#include "com/ConnectionFactory.h"
#include "com/CommunicationException.h"
#include "mocks/UsbDeviceMock.h"
#include <gmock/gmock-spec-builders.h>
#include <gmock/gmock.h>
#include <memory>

using namespace plug::com;
using namespace testing;


class ConnectionFactoryTest : public testing::Test
{
protected:
    void SetUp() override
    {
        contextMock = mock::resetUsbContextMock();
        deviceMock = mock::resetUsbDeviceMock();
    }

    void TearDown() override
    {
        mock::clearUsbContextMock();
        mock::clearUsbDeviceMock();
    }

    mock::UsbContextMock* contextMock{nullptr};
    mock::UsbDeviceMock* deviceMock{nullptr};
};


TEST_F(ConnectionFactoryTest, createUsbConnectionThrowsIfNoDeviceFound)
{
    EXPECT_CALL(*contextMock, listDevices).WillOnce(Return(ByMove(std::vector<usb::Device>{})));

    EXPECT_THROW(createUsbConnection(), CommunicationException);
}

TEST_F(ConnectionFactoryTest, createUsbConnectionReturnsFirstDeviceFound)
{
    std::vector<usb::Device> devices{};
    devices.emplace_back(nullptr);
    devices.emplace_back(nullptr);
    devices.emplace_back(nullptr);
    EXPECT_CALL(*contextMock, listDevices).WillOnce(Return(ByMove(std::move(devices))));
    EXPECT_CALL(*deviceMock, open());
    EXPECT_CALL(*deviceMock, vendorId())
        .WillOnce(Return(0xf0f0))
        .WillOnce(Return(0x1ed8));
    EXPECT_CALL(*deviceMock, productId())
        .WillOnce(Return(0xff04))
        .WillOnce(Return(0x0005));

    auto device = createUsbConnection();
    EXPECT_THAT(device, NotNull());
}
