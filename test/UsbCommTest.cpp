/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2024  offa
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

#include "com/UsbComm.h"
#include "com/CommunicationException.h"
#include "mocks/UsbDeviceMock.h"
#include "matcher/Matcher.h"
#include <array>
#include <gmock/gmock.h>

namespace plug::test
{
    using plug::com::UsbComm;
    using plug::com::usb::Device;
    using namespace plug::test::matcher;
    using namespace testing;

    class UsbCommTest : public testing::Test
    {
    protected:
        void SetUp() override
        {
            deviceMock = mock::resetUsbDeviceMock();
        }

        void TearDown() override
        {
            mock::clearUsbDeviceMock();
        }

        UsbComm create() const
        {
            return UsbComm{Device{nullptr}};
        }

        mock::UsbDeviceMock* deviceMock{nullptr};
    };

    TEST_F(UsbCommTest, ctorOpensDevice)
    {
        EXPECT_CALL(*deviceMock, open());
        EXPECT_CALL(*deviceMock, name());
        UsbComm com{Device{nullptr}};
    }

    TEST_F(UsbCommTest, closeClosesDevice)
    {
        EXPECT_CALL(*deviceMock, open());
        EXPECT_CALL(*deviceMock, name());
        EXPECT_CALL(*deviceMock, close());

        UsbComm com = create();
        com.close();
    }

    TEST_F(UsbCommTest, isOpenReturnDeviceState)
    {
        EXPECT_CALL(*deviceMock, open());
        EXPECT_CALL(*deviceMock, name());

        UsbComm com = create();
        EXPECT_CALL(*deviceMock, isOpen()).WillOnce(Return(false));
        EXPECT_THAT(com.isOpen(), IsFalse());
        EXPECT_CALL(*deviceMock, isOpen()).WillOnce(Return(true));
        EXPECT_THAT(com.isOpen(), IsTrue());
    }

    TEST_F(UsbCommTest, sendSendsData)
    {
        EXPECT_CALL(*deviceMock, open());
        EXPECT_CALL(*deviceMock, name());

        const std::array<std::uint8_t, 4> data{{0x00, 0xa1, 0xb2, 0xb3}};
        EXPECT_CALL(*deviceMock, write(0x01, BufferIs(data), data.size())).WillOnce(Return(data.size()));

        UsbComm com = create();
        const auto n = com.send(data);
        EXPECT_THAT(n, Eq(4));
    }

    TEST_F(UsbCommTest, receiveReceivesData)
    {
        EXPECT_CALL(*deviceMock, open());
        EXPECT_CALL(*deviceMock, name());

        std::vector<std::uint8_t> data{{0x00, 0xa1, 0xb2, 0xb3, 0xc4}};
        EXPECT_CALL(*deviceMock, receive(0x81, data.size())).WillOnce(Return(data));

        UsbComm com = create();
        const auto received = com.receive(data.size());
        EXPECT_THAT(received, Eq(data));
    }

    TEST_F(UsbCommTest, modelName)
    {
        EXPECT_CALL(*deviceMock, open());
        EXPECT_CALL(*deviceMock, name()).WillOnce(Return("USB Device Name"));

        UsbComm com = create();
        EXPECT_THAT(com.name(), Eq("USB Device Name"));
    }

}
