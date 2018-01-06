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

#include "mustang.h"
#include "common.h"
#include <array>
#include <gmock/gmock.h>

using namespace plug;
using namespace testing;

namespace
{
    class UsbMock
    {
    public:

        MOCK_METHOD1(close, void(libusb_device_handle*));
        MOCK_METHOD3(open_device_with_vid_pid, libusb_device_handle*(libusb_context*, uint16_t, uint16_t));
        MOCK_METHOD1(exit, void(libusb_context*));
        MOCK_METHOD2(release_interface, int(libusb_device_handle*, int));
        MOCK_METHOD2(attach_kernel_driver, int(libusb_device_handle*, int));
        MOCK_METHOD6(interrupt_transfer, int(libusb_device_handle*, unsigned char, unsigned char*, int, int*, unsigned int));

    };


    std::unique_ptr<UsbMock> usbmock;
}


extern "C"
{
    int libusb_init(libusb_context** ctx)
    {
        unused(ctx);
        return 0;
    }

    void libusb_exit(libusb_context* ctx)
    {
        usbmock->exit(ctx);
    }

    libusb_device_handle* libusb_open_device_with_vid_pid(libusb_context *ctx, uint16_t vendor_id, uint16_t product_id)
    {
        return usbmock->open_device_with_vid_pid(ctx, vendor_id, product_id);
    }

    int libusb_interrupt_transfer(libusb_device_handle* dev_handle, unsigned char endpoint,
                                unsigned char* data, int length, int* actual_length, unsigned int timeout)
    {
        return usbmock->interrupt_transfer(dev_handle, endpoint, data, length, actual_length, timeout);
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

    int libusb_release_interface(libusb_device_handle* dev_handle, int interface_number)
    {
        return usbmock->release_interface(dev_handle, interface_number);
    }

    int libusb_attach_kernel_driver(libusb_device_handle* dev_handle, int interface_number)
    {
        return usbmock->attach_kernel_driver(dev_handle, interface_number);
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


MATCHER_P(BufferIs, expected, "")
{
    return std::equal(expected.cbegin(), expected.cend(), arg);
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

    void expectStart()
    {
        EXPECT_CALL(*usbmock, open_device_with_vid_pid(_, _, _)).WillOnce(Return(&handle));
        EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, _, _, _)).Times(AnyNumber());
        m->start_amp(nullptr, nullptr, nullptr, nullptr);
    }


    std::unique_ptr<Mustang> m;
    libusb_device_handle handle;
    static constexpr std::size_t packetSize{64};
};

TEST_F(MustangTest, stopAmpDoesNothingIfNotStartedYet)
{
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpClosesConnection)
{
    expectStart();
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(LIBUSB_SUCCESS));
    EXPECT_CALL(*usbmock, attach_kernel_driver(&handle, 0));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, exit(nullptr));
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpClosesConnectionIfNoDevice)
{
    expectStart();
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(LIBUSB_ERROR_NO_DEVICE));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, exit(nullptr));
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpTwiceDoesNothing)
{
    expectStart();
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(LIBUSB_SUCCESS));
    EXPECT_CALL(*usbmock, attach_kernel_driver(&handle, 0));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, exit(nullptr));
    m->stop_amp();
    m->stop_amp();
}

TEST_F(MustangTest, loadMemoryBankSendsBankSelectionCommandAndReceivesPacket)
{
    constexpr int recvSize{1};
    constexpr int slot{8};
    constexpr int slotPos{4};
    std::array<std::uint8_t, packetSize> sendCmd;
    sendCmd.fill(0x00);
    sendCmd[0] = 0x1c;
    sendCmd[1] = 0x01;
    sendCmd[2] = 0x01;
    sendCmd[slotPos] = slot;
    sendCmd[6] = 0x01;
    std::array<std::uint8_t, packetSize> dummy{{0}};

    EXPECT_CALL(*usbmock, interrupt_transfer(_, 0x01, BufferIs(sendCmd), packetSize, _, _)).WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, 0x81, _, packetSize, _, _)).WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize-1), Return(0)));

    const auto result = m->load_memory_bank(slot, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, Eq(0));
}

TEST_F(MustangTest, loadMemoryBankReceivesName)
{
    constexpr int recvSize{1};
    constexpr int slot{8};
    std::array<std::uint8_t, packetSize> recvData;
    recvData.fill(0x00);
    recvData[16] = 'a';
    recvData[17] = 'b';
    recvData[18] = 'c';


    EXPECT_CALL(*usbmock, interrupt_transfer(_, 0x01, _, packetSize, _, _)).WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, 0x81, _, packetSize, _, _)).WillOnce(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(0), Return(0)));

    std::array<char, packetSize> name{{0}};
    m->load_memory_bank(slot, name.data(), nullptr, nullptr);
    EXPECT_THAT(name.data(), StrEq("abc"));
}

TEST_F(MustangTest, loadMemoryBankReturnsErrorOnTransferError)
{
    constexpr int errorCode{1};
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, _, _, _)).WillOnce(DoAll(SetArgPointee<4>(0), Return(errorCode)));
    const auto result = m->load_memory_bank(0, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, Eq(errorCode));
}


