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
#include "LibUsbMocks.h"
#include "common.h"
#include "helper/Matcher.h"
#include "helper/PacketHelper.h"
#include <array>
#include <gmock/gmock.h>

using namespace plug;
using namespace test::matcher;
using namespace test;
using namespace testing;
using mock::UsbMock;


namespace
{
    constexpr std::size_t packetSize{64};
    constexpr std::size_t nameLength{32};

    // Effects
    constexpr std::size_t posEffect{16};
    constexpr std::size_t posFxSlot{18};
    constexpr std::size_t posKnob1{32};
    constexpr std::size_t posKnob2{33};
    constexpr std::size_t posKnob3{34};
    constexpr std::size_t posKnob4{35};
    constexpr std::size_t posKnob5{36};
    constexpr std::size_t posKnob6{37};

    // Amp
    constexpr std::size_t ampPos{16};
    constexpr std::size_t volumePos{32};
    constexpr std::size_t gainPos{33};
    constexpr std::size_t treblePos{36};
    constexpr std::size_t middlePos{37};
    constexpr std::size_t bassPos{38};
    constexpr std::size_t cabinetPos{49};
    constexpr std::size_t noiseGatePos{47};
    constexpr std::size_t thresholdPos{48};
    constexpr std::size_t masterVolPos{35};
    constexpr std::size_t gain2Pos{34};
    constexpr std::size_t presencePos{39};
    constexpr std::size_t depthPos{41};
    constexpr std::size_t biasPos{42};
    constexpr std::size_t sagPos{51};
    constexpr std::size_t brightnessPos{52};
    constexpr std::size_t usbGainPos{16};

    // USB Data
    constexpr std::uint8_t endpointSend{0x01};
    constexpr std::uint8_t endpointReceive{0x81};
    constexpr std::uint16_t usbVid{0x1ed8};
    constexpr std::uint16_t pidMustangI_II{0x0004};
    constexpr std::uint16_t pidMustangIII_IV_V{0x0005};
    constexpr std::uint16_t pidMustangMini{0x0010};
    constexpr std::uint16_t pidMustangFloor{0x0012};
    constexpr std::uint16_t pidMustangI_II_v2{0x0014};
    constexpr std::uint16_t pidMustangIII_IV_V_v2{0x0016};

    // Control Data
    constexpr std::size_t posDsp{2};
    constexpr std::uint8_t posFxKnob{3};
    constexpr std::uint8_t posSaveField{4};
    constexpr std::size_t posSlot{4};
}


class MustangTest : public testing::Test
{
protected:
    void SetUp() override
    {
        m = std::make_unique<Mustang>();
        usbmock = mock::resetUsbMock();
    }

    void TearDown() override
    {
        mock::clearUsbMock();
    }

    void expectStart()
    {
        EXPECT_CALL(*usbmock, init(_));
        EXPECT_CALL(*usbmock, open_device_with_vid_pid(_, _, _)).WillOnce(Return(&handle));
        EXPECT_CALL(*usbmock, kernel_driver_active(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*usbmock, claim_interface(_, _)).WillOnce(Return(0));
        EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, _, _, _)).Times(AnyNumber());
        m->start_amp(nullptr, nullptr, nullptr, nullptr);
    }

    void expectClose()
    {
        EXPECT_CALL(*usbmock, release_interface(_, _));
        EXPECT_CALL(*usbmock, attach_kernel_driver(_, _));
        EXPECT_CALL(*usbmock, close(_));
        EXPECT_CALL(*usbmock, exit(_));
    }

    void ignoreClose()
    {
        EXPECT_CALL(*usbmock, release_interface(_, _)).RetiresOnSaturation();
        EXPECT_CALL(*usbmock, attach_kernel_driver(_, _)).RetiresOnSaturation();
        EXPECT_CALL(*usbmock, close(_)).RetiresOnSaturation();
        EXPECT_CALL(*usbmock, exit(_)).RetiresOnSaturation();
        m = nullptr;
    }

    auto createEffectData(std::uint8_t slotValue, std::uint8_t effect, std::array<std::uint8_t, 6> values) const
    {
        auto data = helper::createEmptyPacket();
        data[posDsp] = 8;
        data[posEffect] = effect;
        data[posFxSlot] = slotValue;
        std::copy(values.cbegin(), values.cend(), std::next(data.begin(), posKnob1));
        return data;
    }


    std::unique_ptr<Mustang> m;
    mock::UsbMock* usbmock;
    libusb_device_handle handle;
    const helper::BinData dummy = helper::createEmptyPacket();
    const helper::BinData initCmd = helper::createInitCmdPacket();
    static constexpr int usbSuccess{LIBUSB_SUCCESS};
    static constexpr int usbError{LIBUSB_ERROR_NO_DEVICE};
    static constexpr int slot{5};
};

TEST_F(MustangTest, startInitializesUsb)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    constexpr int recvSize{0};
    auto initCmd1 = helper::createInitializedPacket({0x00, 0xc3});
    auto initCmd2 = helper::createInitializedPacket({0x1a, 0x03});

    // Init Step 1
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd1), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)));
    // Init Step 2
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)));

    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsSuccessful());

    ignoreClose();
}

TEST_F(MustangTest, startReturnsErrorOnInitFailure)
{
    EXPECT_CALL(*usbmock, init(nullptr)).WillOnce(Return(usbError));
    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, startDeterminesAmpType)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, pidMustangI_II)).WillOnce(Return(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, pidMustangIII_IV_V)).WillOnce(Return(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, pidMustangI_II_v2)).WillOnce(Return(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, pidMustangIII_IV_V_v2)).WillOnce(Return(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, pidMustangMini)).WillOnce(Return(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, pidMustangFloor)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0));
    EXPECT_CALL(*usbmock, claim_interface(_, 0));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, _, _, _)).Times(AnyNumber());

    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsSuccessful());

    ignoreClose();
}

TEST_F(MustangTest, startFailsIfNoDeviceFound)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*usbmock, exit(nullptr));

    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, startDetachesKernelDriverIfNotActive)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(usbError));
    EXPECT_CALL(*usbmock, detach_kernel_driver(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, _, _, _)).Times(AnyNumber());

    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsSuccessful());

    ignoreClose();
}

TEST_F(MustangTest, startFailsIfDriverFails)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(usbError));
    EXPECT_CALL(*usbmock, detach_kernel_driver(&handle, 0)).WillOnce(Return(usbError));
    expectClose();

    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, startFailsIfClaimFails)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(usbError));
    expectClose();

    const auto result = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, startRequestsCurrentPresetName)
{
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    auto recvData = helper::createEmptyNamedPacket("abc");
    constexpr int recvSize{0};
    constexpr int recvSizeResponse{1};

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .Times(2)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));

    Sequence s;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(200)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(200), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(0), Return(0)));

    char nameList[100][nameLength];
    std::array<char, nameLength> name;
    const auto result = m->start_amp(nameList, name.data(), nullptr, nullptr);
    EXPECT_THAT(result, IsSuccessful());
    EXPECT_THAT(name.data(), StrEq("abc"));

    ignoreClose();
}

TEST_F(MustangTest, startRequestsCurrentAmp)
{
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    auto recvData = helper::createEmptyPacket();
    recvData[ampPos] = 0x61;
    recvData[volumePos] = 8;
    recvData[gainPos] = 4;
    recvData[treblePos] = 5;
    recvData[middlePos] = 9;
    recvData[bassPos] = 1;
    recvData[cabinetPos] = 2;
    recvData[noiseGatePos] = 5;
    recvData[thresholdPos] = 4;
    recvData[masterVolPos] = 3;
    recvData[gain2Pos] = 4;
    recvData[presencePos] = 7;
    recvData[depthPos] = 2;
    recvData[biasPos] = 6;
    recvData[sagPos] = 5;
    recvData[brightnessPos] = 1;
    auto extendedData = helper::createEmptyPacket();
    extendedData[usbGainPos] = 0x44;

    constexpr int recvSize{0};
    constexpr int recvSizeResponse{1};

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));

    Sequence s;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(200)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(200), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .Times(5)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(recvSize - 1), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .Times(1)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(extendedData.cbegin(), extendedData.cend()), SetArgPointee<4>(0), Return(0)))
        .RetiresOnSaturation();

    char nameList[100][32];
    amp_settings settings;
    const auto result = m->start_amp(nameList, nullptr, &settings, nullptr);
    EXPECT_THAT(result, IsSuccessful());
    EXPECT_THAT(settings.amp_num, Eq(value(amps::BRITISH_60S)));
    EXPECT_THAT(settings.volume, Eq(recvData[volumePos]));
    EXPECT_THAT(settings.gain, Eq(recvData[gainPos]));
    EXPECT_THAT(settings.treble, Eq(recvData[treblePos]));
    EXPECT_THAT(settings.middle, Eq(recvData[middlePos]));
    EXPECT_THAT(settings.bass, Eq(recvData[bassPos]));
    EXPECT_THAT(settings.cabinet, Eq(recvData[cabinetPos]));
    EXPECT_THAT(settings.noise_gate, Eq(recvData[noiseGatePos]));
    EXPECT_THAT(settings.threshold, Eq(recvData[thresholdPos]));
    EXPECT_THAT(settings.master_vol, Eq(recvData[masterVolPos]));
    EXPECT_THAT(settings.gain2, Eq(recvData[gain2Pos]));
    EXPECT_THAT(settings.presence, Eq(recvData[presencePos]));
    EXPECT_THAT(settings.depth, Eq(recvData[depthPos]));
    EXPECT_THAT(settings.bias, Eq(recvData[biasPos]));
    EXPECT_THAT(settings.sag, Eq(recvData[sagPos]));
    EXPECT_THAT(settings.brightness, Eq(recvData[brightnessPos]));
    EXPECT_THAT(settings.usb_gain, Eq(0x44));

    ignoreClose();
}

TEST_F(MustangTest, startRequestsCurrentEffects)
{
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    auto recvData0 = createEffectData(0x04, 0x19, {{10, 20, 30, 40, 50, 60}});
    auto recvData1 = createEffectData(0x01, 0x13, {{0, 0, 0, 1, 1, 1}});
    auto recvData2 = createEffectData(0x02, 0x00, {{0, 0, 0, 0, 0, 0}});
    auto recvData3 = createEffectData(0x07, 0x2b, {{1, 2, 3, 4, 5, 6}});
    constexpr int recvSize{0};
    constexpr int recvSizeResponse{1};

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .Times(2)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));

    Sequence s;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(200)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(200), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(4), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData0.cbegin(), recvData0.cend()), SetArgPointee<4>(3), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData1.cbegin(), recvData1.cend()), SetArgPointee<4>(2), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData2.cbegin(), recvData2.cend()), SetArgPointee<4>(1), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData3.cbegin(), recvData3.cend()), SetArgPointee<4>(0), Return(0)));

    char nameList[100][32];
    std::array<fx_pedal_settings, 4> settings;
    const auto result = m->start_amp(nameList, nullptr, nullptr, settings.data());
    EXPECT_THAT(result, IsSuccessful());
    EXPECT_THAT(settings[0].fx_slot, Eq(0));
    EXPECT_THAT(settings[0].knob1, Eq(10));
    EXPECT_THAT(settings[0].knob2, Eq(20));
    EXPECT_THAT(settings[0].knob3, Eq(30));
    EXPECT_THAT(settings[0].knob4, Eq(40));
    EXPECT_THAT(settings[0].knob5, Eq(50));
    EXPECT_THAT(settings[0].knob6, Eq(60));
    EXPECT_THAT(settings[0].put_post_amp, Eq(true));
    EXPECT_THAT(settings[0].effect_num, Eq(value(effects::TRIANGLE_FLANGER)));

    ignoreClose();
}

TEST_F(MustangTest, startRequestsAmpPresetList)
{
    auto recvData0 = helper::createEmptyNamedPacket("abc");
    auto recvData1 = helper::createEmptyNamedPacket("def");
    auto recvData2 = helper::createEmptyNamedPacket("ghi");

    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    constexpr int recvSize{0};

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .Times(2)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData0.cbegin(), recvData0.cend()), SetArgPointee<4>(4), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(3), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData1.cbegin(), recvData1.cend()), SetArgPointee<4>(2), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(1), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData2.cbegin(), recvData2.cend()), SetArgPointee<4>(0), Return(0)));

    constexpr int recvSizeResponse{1};
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSizeResponse), Return(0)));

    constexpr std::size_t numberOfNames{100};
    char names[numberOfNames][nameLength];

    const auto result = m->start_amp(names, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsSuccessful());
    EXPECT_THAT(names[0], StrEq("abc"));
    EXPECT_THAT(names[1], StrEq("def"));
    EXPECT_THAT(names[2], StrEq("ghi"));

    ignoreClose();
}

TEST_F(MustangTest, startDoesNotInitializeUsbIfCalledMultipleTimes)
{
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    constexpr std::size_t numOfCalls{2};
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, _, _, _, _, _))
        .Times(numOfCalls * 4)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));

    EXPECT_THAT(m->start_amp(nullptr, nullptr, nullptr, nullptr), IsSuccessful());

    const auto result1 = m->start_amp(nullptr, nullptr, nullptr, nullptr);
    EXPECT_THAT(result1, IsSuccessful());

    ignoreClose();
}

TEST_F(MustangTest, stopAmpDoesNothingIfNotStartedYet)
{
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpClosesConnection)
{
    expectStart();
    InSequence s;
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(usbSuccess));
    EXPECT_CALL(*usbmock, attach_kernel_driver(&handle, 0));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, exit(nullptr));
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpClosesConnectionIfNoDevice)
{
    expectStart();
    InSequence s;
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(usbError));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, exit(nullptr));
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpTwiceDoesNothing)
{
    expectStart();
    InSequence s;
    EXPECT_CALL(*usbmock, release_interface(&handle, 0)).WillOnce(Return(usbSuccess));
    EXPECT_CALL(*usbmock, attach_kernel_driver(&handle, 0));
    EXPECT_CALL(*usbmock, close(_));
    EXPECT_CALL(*usbmock, exit(nullptr));
    m->stop_amp();
    m->stop_amp();
}

TEST_F(MustangTest, loadMemoryBankSendsBankSelectionCommandAndReceivesPacket)
{
    constexpr int recvSize{1};
    auto sendCmd = helper::createInitializedPacket({0x1c, 0x01, 0x01});
    sendCmd[posSlot] = slot;
    sendCmd[6] = 0x01;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(sendCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize - 1), Return(0)));

    const auto result = m->load_memory_bank(slot, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, loadMemoryBankReceivesName)
{
    constexpr int recvSize{1};
    auto recvData = helper::createEmptyNamedPacket("abc");

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(0), Return(0)));

    std::array<char, packetSize> name{{0}};
    m->load_memory_bank(slot, name.data(), nullptr, nullptr);
    EXPECT_THAT(name.data(), StrEq("abc"));
}

TEST_F(MustangTest, loadMemoryBankReceivesAmpValues)
{
    constexpr int recvSize{2};
    auto recvData = helper::createEmptyPacket();
    recvData[ampPos] = 0x5e;
    recvData[volumePos] = 1;
    recvData[gainPos] = 2;
    recvData[treblePos] = 3;
    recvData[middlePos] = 4;
    recvData[bassPos] = 5;
    recvData[cabinetPos] = 6;
    recvData[noiseGatePos] = 7;
    recvData[thresholdPos] = 8;
    recvData[masterVolPos] = 9;
    recvData[gain2Pos] = 10;
    recvData[presencePos] = 11;
    recvData[depthPos] = 12;
    recvData[biasPos] = 13;
    recvData[sagPos] = 14;
    recvData[brightnessPos] = 0;
    auto extendedData = helper::createEmptyPacket();
    extendedData[usbGainPos] = 0xab;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize - 1), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .Times(5)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(recvSize - 1), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .Times(1)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(extendedData.cbegin(), extendedData.cend()), SetArgPointee<4>(0), Return(0)))
        .RetiresOnSaturation();

    amp_settings settings{};
    m->load_memory_bank(slot, nullptr, &settings, nullptr);
    EXPECT_THAT(settings.amp_num, Eq(value(amps::BRITISH_80S)));
    EXPECT_THAT(settings.volume, Eq(recvData[volumePos]));
    EXPECT_THAT(settings.gain, Eq(recvData[gainPos]));
    EXPECT_THAT(settings.treble, Eq(recvData[treblePos]));
    EXPECT_THAT(settings.middle, Eq(recvData[middlePos]));
    EXPECT_THAT(settings.bass, Eq(recvData[bassPos]));
    EXPECT_THAT(settings.cabinet, Eq(recvData[cabinetPos]));
    EXPECT_THAT(settings.noise_gate, Eq(recvData[noiseGatePos]));
    EXPECT_THAT(settings.threshold, Eq(recvData[thresholdPos]));
    EXPECT_THAT(settings.master_vol, Eq(recvData[masterVolPos]));
    EXPECT_THAT(settings.gain2, Eq(recvData[gain2Pos]));
    EXPECT_THAT(settings.presence, Eq(recvData[presencePos]));
    EXPECT_THAT(settings.depth, Eq(recvData[depthPos]));
    EXPECT_THAT(settings.bias, Eq(recvData[biasPos]));
    EXPECT_THAT(settings.sag, Eq(recvData[sagPos]));
    EXPECT_THAT(settings.brightness, Eq(recvData[brightnessPos]));
    EXPECT_THAT(settings.usb_gain, Eq(0xab));
}

TEST_F(MustangTest, loadMemoryBankReceivesEffectValues)
{
    constexpr int recvSize{6};
    auto recvData0 = createEffectData(0x04, 0x4f, {{11, 22, 33, 44, 55, 66}});
    auto recvData1 = createEffectData(0x01, 0x13, {{0, 0, 0, 1, 1, 1}});
    auto recvData2 = createEffectData(0x02, 0x00, {{0, 0, 0, 0, 0, 0}});
    auto recvData3 = createEffectData(0x07, 0x2b, {{1, 2, 3, 4, 5, 6}});

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize - 1), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize - 2), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData0.cbegin(), recvData0.cend()), SetArgPointee<4>(recvSize - 3), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData1.cbegin(), recvData1.cend()), SetArgPointee<4>(recvSize - 4), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData2.cbegin(), recvData2.cend()), SetArgPointee<4>(recvSize - 5), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData3.cbegin(), recvData3.cend()), SetArgPointee<4>(recvSize - 6), Return(0)));

    std::array<fx_pedal_settings, 4> settings;
    m->load_memory_bank(slot, nullptr, nullptr, settings.data());

    EXPECT_THAT(settings[0].fx_slot, Eq(0));
    EXPECT_THAT(settings[0].knob1, Eq(11));
    EXPECT_THAT(settings[0].knob2, Eq(22));
    EXPECT_THAT(settings[0].knob3, Eq(33));
    EXPECT_THAT(settings[0].knob4, Eq(44));
    EXPECT_THAT(settings[0].knob5, Eq(55));
    EXPECT_THAT(settings[0].knob6, Eq(66));
    EXPECT_THAT(settings[0].put_post_amp, Eq(true));
    EXPECT_THAT(settings[0].effect_num, Eq(value(effects::PHASER)));

    EXPECT_THAT(settings[1].fx_slot, Eq(1));
    EXPECT_THAT(settings[1].knob1, Eq(0));
    EXPECT_THAT(settings[1].knob2, Eq(0));
    EXPECT_THAT(settings[1].knob3, Eq(0));
    EXPECT_THAT(settings[1].knob4, Eq(1));
    EXPECT_THAT(settings[1].knob5, Eq(1));
    EXPECT_THAT(settings[1].knob6, Eq(1));
    EXPECT_THAT(settings[1].put_post_amp, Eq(false));
    EXPECT_THAT(settings[1].effect_num, Eq(value(effects::TRIANGLE_CHORUS)));

    EXPECT_THAT(settings[2].fx_slot, Eq(2));
    EXPECT_THAT(settings[2].knob1, Eq(0));
    EXPECT_THAT(settings[2].knob2, Eq(0));
    EXPECT_THAT(settings[2].knob3, Eq(0));
    EXPECT_THAT(settings[2].knob4, Eq(0));
    EXPECT_THAT(settings[2].knob5, Eq(0));
    EXPECT_THAT(settings[2].knob6, Eq(0));
    EXPECT_THAT(settings[2].put_post_amp, Eq(false));
    EXPECT_THAT(settings[2].effect_num, Eq(value(effects::EMPTY)));

    EXPECT_THAT(settings[3].fx_slot, Eq(3));
    EXPECT_THAT(settings[3].knob1, Eq(1));
    EXPECT_THAT(settings[3].knob2, Eq(2));
    EXPECT_THAT(settings[3].knob3, Eq(3));
    EXPECT_THAT(settings[3].knob4, Eq(4));
    EXPECT_THAT(settings[3].knob5, Eq(5));
    EXPECT_THAT(settings[3].knob6, Eq(6));
    EXPECT_THAT(settings[3].put_post_amp, Eq(true));
    EXPECT_THAT(settings[3].effect_num, Eq(value(effects::TAPE_DELAY)));
}

TEST_F(MustangTest, loadMemoryBankReturnsErrorOnTransferError)
{
    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(usbError)));
    const auto result = m->load_memory_bank(0, nullptr, nullptr, nullptr);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, setAmpSendsValues)
{
    amp_settings settings;
    settings.amp_num = value(amps::BRITISH_70S);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = value(cabinets::cab4x12G);
    settings.noise_gate = 3;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = 1;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = 5;
    settings.brightness = true;
    settings.usb_gain = 4;

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                                               0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                                               0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posDsp] = 0x05;
    data[gainPos] = settings.gain;
    data[volumePos] = settings.volume;
    data[treblePos] = settings.treble;
    data[middlePos] = settings.middle;
    data[bassPos] = settings.bass;
    data[cabinetPos] = settings.cabinet;
    data[noiseGatePos] = settings.noise_gate;
    data[masterVolPos] = settings.master_vol;
    data[gain2Pos] = settings.gain2;
    data[presencePos] = settings.presence;
    data[biasPos] = settings.bias;
    data[sagPos] = 0x01;
    data[brightnessPos] = 1;
    data[ampPos] = 0x79;
    data[44] = 0x0b;
    data[45] = 0x0b;
    data[46] = 0x0b;
    data[50] = 0x0b;
    data[54] = 0x7c;
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    auto data2 = helper::createInitializedPacket({0x1c, 0x03, 0x0d});
    data2[6] = 0x01;
    data2[7] = 0x01;
    data2[usbGainPos] = settings.usb_gain;


    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setAmpHandlesOutOfRangeCabinet)
{
    constexpr int invalidCabinet{0x0d};
    amp_settings settings;
    settings.amp_num = value(amps::FENDER_57_CHAMP);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = invalidCabinet;
    settings.noise_gate = 3;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = 1;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = 5;
    settings.brightness = true;
    settings.usb_gain = 4;

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                                               0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                                               0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posDsp] = 0x05;
    data[gainPos] = settings.gain;
    data[volumePos] = settings.volume;
    data[treblePos] = settings.treble;
    data[middlePos] = settings.middle;
    data[bassPos] = settings.bass;
    data[cabinetPos] = 0x00;
    data[noiseGatePos] = settings.noise_gate;
    data[masterVolPos] = settings.master_vol;
    data[gain2Pos] = settings.gain2;
    data[presencePos] = settings.presence;
    data[biasPos] = settings.bias;
    data[sagPos] = 0x01;
    data[brightnessPos] = 1;
    data[ampPos] = 0x7c;
    data[44] = 0x0c;
    data[45] = 0x0c;
    data[46] = 0x0c;
    data[50] = 0x0c;
    data[54] = 0x00;
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    auto data2 = helper::createInitializedPacket({0x1c, 0x03, 0x0d});
    data2[6] = 0x01;
    data2[7] = 0x01;
    data2[usbGainPos] = settings.usb_gain;


    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setAmpHandlesNoiseGate)
{
    constexpr int limitValue{0x05};
    amp_settings settings;
    settings.amp_num = value(amps::FENDER_SUPER_SONIC);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = value(cabinets::cab57DLX);
    settings.noise_gate = limitValue;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = 7;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = 5;
    settings.brightness = true;
    settings.usb_gain = 4;

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                                               0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                                               0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posDsp] = 0x05;
    data[gainPos] = settings.gain;
    data[volumePos] = settings.volume;
    data[treblePos] = settings.treble;
    data[middlePos] = settings.middle;
    data[bassPos] = settings.bass;
    data[cabinetPos] = settings.cabinet;
    data[noiseGatePos] = settings.noise_gate;
    data[thresholdPos] = settings.threshold;
    data[depthPos] = settings.depth;
    data[masterVolPos] = settings.master_vol;
    data[gain2Pos] = settings.gain2;
    data[presencePos] = settings.presence;
    data[biasPos] = settings.bias;
    data[sagPos] = 0x01;
    data[brightnessPos] = 1;
    data[ampPos] = 0x72;
    data[44] = 0x06;
    data[45] = 0x06;
    data[46] = 0x06;
    data[50] = 0x06;
    data[54] = 0x79;
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    auto data2 = helper::createInitializedPacket({0x1c, 0x03, 0x0d});
    data2[6] = 0x01;
    data2[7] = 0x01;
    data2[usbGainPos] = settings.usb_gain;


    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setAmpHandlesNoiseGateAndOutOfRangeThreshold)
{
    constexpr int limitValue{0x05};
    constexpr int outOfRange{0x10};
    amp_settings settings;
    settings.amp_num = value(amps::FENDER_SUPER_SONIC);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = value(cabinets::cab57DLX);
    settings.noise_gate = limitValue;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = outOfRange;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = 5;
    settings.brightness = true;
    settings.usb_gain = 4;

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                                               0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                                               0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posDsp] = 0x05;
    data[gainPos] = settings.gain;
    data[volumePos] = settings.volume;
    data[treblePos] = settings.treble;
    data[middlePos] = settings.middle;
    data[bassPos] = settings.bass;
    data[cabinetPos] = settings.cabinet;
    data[noiseGatePos] = settings.noise_gate;
    data[thresholdPos] = 0x00;
    data[depthPos] = settings.depth;
    data[masterVolPos] = settings.master_vol;
    data[gain2Pos] = settings.gain2;
    data[presencePos] = settings.presence;
    data[biasPos] = settings.bias;
    data[sagPos] = 0x01;
    data[brightnessPos] = 1;
    data[ampPos] = 0x72;
    data[44] = 0x06;
    data[45] = 0x06;
    data[46] = 0x06;
    data[50] = 0x06;
    data[54] = 0x79;
    auto cmdExecute = helper::createInitializedPacket({0x01c, 0x03});
    auto data2 = helper::createInitializedPacket({0x1c, 0x03, 0x0d});
    data2[6] = 0x01;
    data2[7] = 0x01;
    data2[usbGainPos] = settings.usb_gain;


    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));


    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setAmpHandlesSagValue)
{
    constexpr int valueInRange{0x00};
    amp_settings settings;
    settings.amp_num = value(amps::FENDER_SUPER_SONIC);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = value(cabinets::cab57DLX);
    settings.noise_gate = 5;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = 7;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = valueInRange;
    settings.brightness = true;
    settings.usb_gain = 4;

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                                               0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                                               0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posDsp] = 0x05;
    data[gainPos] = settings.gain;
    data[volumePos] = settings.volume;
    data[treblePos] = settings.treble;
    data[middlePos] = settings.middle;
    data[bassPos] = settings.bass;
    data[cabinetPos] = settings.cabinet;
    data[noiseGatePos] = settings.noise_gate;
    data[thresholdPos] = settings.threshold;
    data[depthPos] = settings.depth;
    data[masterVolPos] = settings.master_vol;
    data[gain2Pos] = settings.gain2;
    data[presencePos] = settings.presence;
    data[biasPos] = settings.bias;
    data[sagPos] = settings.sag;
    data[brightnessPos] = 1;
    data[ampPos] = 0x72;
    data[44] = 0x06;
    data[45] = 0x06;
    data[46] = 0x06;
    data[50] = 0x06;
    data[54] = 0x79;
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    auto data2 = helper::createInitializedPacket({0x1c, 0x03, 0x0d});
    data2[6] = 0x01;
    data2[7] = 0x01;
    data2[usbGainPos] = settings.usb_gain;


    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setAmpHandlesOutOfRangeNoiseGate)
{
    constexpr int outOfRange{0x06};
    constexpr int expectedValue{0x00};
    amp_settings settings;
    settings.amp_num = value(amps::BRITISH_60S);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = value(cabinets::cab57DLX);
    settings.noise_gate = outOfRange;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = 7;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = 5;
    settings.brightness = true;
    settings.usb_gain = 4;

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                                               0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                                               0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posDsp] = 0x05;
    data[gainPos] = settings.gain;
    data[volumePos] = settings.volume;
    data[treblePos] = settings.treble;
    data[middlePos] = settings.middle;
    data[bassPos] = settings.bass;
    data[cabinetPos] = settings.cabinet;
    data[noiseGatePos] = expectedValue;
    data[masterVolPos] = settings.master_vol;
    data[gain2Pos] = settings.gain2;
    data[presencePos] = settings.presence;
    data[biasPos] = settings.bias;
    data[sagPos] = 0x01;
    data[brightnessPos] = 1;
    data[ampPos] = 0x61;
    data[44] = 0x07;
    data[45] = 0x07;
    data[46] = 0x07;
    data[50] = 0x07;
    data[54] = 0x5e;
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    auto data2 = helper::createInitializedPacket({0x1c, 0x03, 0x0d});
    data2[6] = 0x01;
    data2[7] = 0x01;
    data2[usbGainPos] = settings.usb_gain;


    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data.cbegin(), data.cend()), SetArgPointee<4>(data.size()), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data2), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(data2.cbegin(), data2.cend()), SetArgPointee<4>(data.size()), Return(0)));

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setAmpReturnsErrorOnFailure)
{
    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(6)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(usbError)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));

    amp_settings settings;
    settings.amp_num = value(amps::BRITISH_70S);
    settings.gain = 8;
    settings.volume = 9;
    settings.treble = 1;
    settings.middle = 2;
    settings.bass = 3;
    settings.cabinet = value(cabinets::cab4x12G);
    settings.noise_gate = 3;
    settings.master_vol = 5;
    settings.gain2 = 3;
    settings.presence = 2;
    settings.threshold = 1;
    settings.depth = 4;
    settings.bias = 1;
    settings.sag = 5;
    settings.brightness = true;
    settings.usb_gain = 4;

    const auto result = m->set_amplifier(settings);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, setEffectSendsValue)
{
    fx_pedal_settings settings;
    settings.fx_slot = 3;
    settings.effect_num = value(effects::OVERDRIVE);
    settings.knob1 = 8;
    settings.knob2 = 7;
    settings.knob3 = 6;
    settings.knob4 = 5;
    settings.knob5 = 4;
    settings.knob6 = 3;
    settings.put_post_amp = false;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    std::array<std::uint8_t, packetSize> clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    clearCmd[posEffect] = 0x00;
    clearCmd[posKnob1] = 0x00;
    clearCmd[posKnob2] = 0x00;
    clearCmd[posKnob3] = 0x00;
    clearCmd[posKnob4] = 0x00;
    clearCmd[posKnob5] = 0x00;
    clearCmd[posKnob6] = 0x00;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(clearCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posFxSlot] = settings.fx_slot;
    data[posKnob1] = settings.knob1;
    data[posKnob2] = settings.knob2;
    data[posKnob3] = settings.knob3;
    data[posKnob4] = settings.knob4;
    data[posKnob5] = settings.knob5;
    data[posDsp] = 0x06;
    data[posEffect] = 0x3c;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    const auto result = m->set_effect(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setEffectClearsEffectIfEmptyEffect)
{
    fx_pedal_settings settings;
    settings.fx_slot = 2;
    settings.effect_num = value(effects::EMPTY);

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    std::array<std::uint8_t, packetSize> clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    clearCmd[posEffect] = 0x00;
    clearCmd[posKnob1] = 0x00;
    clearCmd[posKnob2] = 0x00;
    clearCmd[posKnob3] = 0x00;
    clearCmd[posKnob4] = 0x00;
    clearCmd[posKnob5] = 0x00;
    clearCmd[posKnob6] = 0x00;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(clearCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    const auto result = m->set_effect(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setEffectHandlesEffectPosition)
{
    fx_pedal_settings settings;
    settings.fx_slot = 2;
    settings.effect_num = value(effects::OVERDRIVE);
    settings.knob1 = 1;
    settings.knob2 = 2;
    settings.knob3 = 3;
    settings.knob4 = 4;
    settings.knob5 = 5;
    settings.knob6 = 6;
    settings.put_post_amp = true;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    std::array<std::uint8_t, packetSize> clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    clearCmd[posEffect] = 0x00;
    clearCmd[posKnob1] = 0x00;
    clearCmd[posKnob2] = 0x00;
    clearCmd[posKnob3] = 0x00;
    clearCmd[posKnob4] = 0x00;
    clearCmd[posKnob5] = 0x00;
    clearCmd[posKnob6] = 0x00;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(clearCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    constexpr int fxLoopOffset{4};
    data[posFxSlot] = settings.fx_slot + fxLoopOffset;
    data[posKnob1] = settings.knob1;
    data[posKnob2] = settings.knob2;
    data[posKnob3] = settings.knob3;
    data[posKnob4] = settings.knob4;
    data[posKnob5] = settings.knob5;
    data[posDsp] = 0x06;
    data[posEffect] = 0x3c;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    const auto result = m->set_effect(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setEffectHandlesEffectsWithMoreControls)
{
    fx_pedal_settings settings;
    settings.fx_slot = 3;
    settings.effect_num = value(effects::STEREO_TAPE_DELAY);
    settings.knob1 = 1;
    settings.knob2 = 1;
    settings.knob3 = 1;
    settings.knob4 = 1;
    settings.knob5 = 1;
    settings.knob6 = 7;
    settings.put_post_amp = false;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    std::array<std::uint8_t, packetSize> clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    clearCmd[posEffect] = 0x00;
    clearCmd[posKnob1] = 0x00;
    clearCmd[posKnob2] = 0x00;
    clearCmd[posKnob3] = 0x00;
    clearCmd[posKnob4] = 0x00;
    clearCmd[posKnob5] = 0x00;
    clearCmd[posKnob6] = 0x00;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(clearCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posFxSlot] = settings.fx_slot;
    data[posKnob1] = settings.knob1;
    data[posKnob2] = settings.knob2;
    data[posKnob3] = settings.knob3;
    data[posKnob4] = settings.knob4;
    data[posKnob5] = settings.knob5;
    data[posKnob6] = settings.knob6;
    data[posDsp] = 0x08;
    data[posEffect] = 0x2a;
    data[19] = 0x02;
    data[20] = 0x01;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(data), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    const auto result = m->set_effect(settings);
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, setEffectReturnsErrorOnFailure)
{
    fx_pedal_settings settings;
    settings.fx_slot = 3;
    settings.effect_num = value(effects::OVERDRIVE);
    settings.knob1 = 8;
    settings.knob2 = 7;
    settings.knob3 = 6;
    settings.knob4 = 5;
    settings.knob5 = 4;
    settings.knob6 = 3;
    settings.put_post_amp = false;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    std::array<std::uint8_t, packetSize> clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    clearCmd[posEffect] = 0x00;
    clearCmd[posKnob1] = 0x00;
    clearCmd[posKnob2] = 0x00;
    clearCmd[posKnob3] = 0x00;
    clearCmd[posKnob4] = 0x00;
    clearCmd[posKnob5] = 0x00;
    clearCmd[posKnob6] = 0x00;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(4)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));

    std::array<std::uint8_t, packetSize> data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    data[posFxSlot] = settings.fx_slot;
    data[posKnob1] = settings.knob1;
    data[posKnob2] = settings.knob2;
    data[posKnob3] = settings.knob3;
    data[posKnob4] = settings.knob4;
    data[posKnob5] = settings.knob5;
    data[posDsp] = 0x06;
    data[posEffect] = 0x3c;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(2)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(usbError)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));


    const auto result = m->set_effect(settings);
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, saveEffectsSendsValues)
{
    std::array<fx_pedal_settings, 2> settings{{fx_pedal_settings{1, value(effects::MONO_DELAY), 0, 1, 2, 3, 4, 5, false},
                                               fx_pedal_settings{2, value(effects::SINE_FLANGER), 6, 7, 8, 0, 0, 0, true}}};
    constexpr int numOfEffects = settings.size();
    constexpr int fxKnob{0x02};
    constexpr int postAmpOffset{4};
    std::array<char, 24> name{{'a', 'b', 'c', 'd', '\0'}};
    std::array<std::uint8_t, packetSize> dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    std::array<std::uint8_t, packetSize> dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    dataName[posFxKnob] = fxKnob;
    dataName[posSaveField] = slot;
    std::copy(name.cbegin(), std::next(name.cbegin(), 4), std::next(dataName.begin(), 16));

    dataValues[posSaveField] = slot;
    dataValues[1] = 0x03;
    dataValues[6] = 0x00;

    std::fill(std::next(dataValues.begin(), 16), dataValues.end(), 0x00);
    dataValues[19] = 0x00;
    dataValues[20] = 0x08;
    dataValues[21] = 0x01;
    dataValues[posKnob6] = 0x00;

    auto dataEffect0 = dataValues;
    dataEffect0[posFxKnob] = fxKnob;
    dataEffect0[posFxSlot] = settings[0].fx_slot;
    dataEffect0[posKnob1] = settings[0].knob1;
    dataEffect0[posKnob2] = settings[0].knob2;
    dataEffect0[posKnob3] = settings[0].knob3;
    dataEffect0[posKnob4] = settings[0].knob4;
    dataEffect0[posKnob5] = settings[0].knob5;
    dataEffect0[posDsp] = 0x08;
    dataEffect0[posEffect] = 0x16;
    dataEffect0[19] = 0x02;
    dataEffect0[20] = 0x01;
    auto dataEffect1 = dataValues;
    dataEffect1[posFxKnob] = fxKnob;
    dataEffect1[posFxSlot] = settings[1].fx_slot + postAmpOffset;
    dataEffect1[posKnob1] = settings[1].knob1;
    dataEffect1[posKnob2] = settings[1].knob2;
    dataEffect1[posKnob3] = settings[1].knob3;
    dataEffect1[posKnob4] = settings[1].knob4;
    dataEffect1[posKnob5] = settings[1].knob5;
    dataEffect1[posDsp] = 0x07;
    dataEffect1[posEffect] = 0x18;
    dataEffect1[19] = 0x01;
    dataEffect1[20] = 0x01;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03, 0x00});
    cmdExecute[posFxKnob] = fxKnob;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataName), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    // Effect #0
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataEffect0), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    // Effect #1
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataEffect1), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    // Execute Cmd
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));


    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, saveEffectsLimitsNumberOfValues)
{
    std::array<fx_pedal_settings, 3> settings{{fx_pedal_settings{1, value(effects::MONO_DELAY), 0, 1, 2, 3, 4, 5, false},
                                               fx_pedal_settings{2, value(effects::SINE_FLANGER), 6, 7, 8, 0, 0, 0, true},
                                               fx_pedal_settings{3, value(effects::SINE_FLANGER), 1, 2, 2, 1, 0, 4, true}}};
    constexpr int numOfEffects = settings.size();
    constexpr int fxKnob{0x02};
    std::array<char, 24> name{{'a', 'b', 'c', 'd', '\0'}};
    std::array<std::uint8_t, packetSize> dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    std::array<std::uint8_t, packetSize> dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    dataName[posFxKnob] = fxKnob;
    dataName[posSaveField] = slot;
    std::copy(name.cbegin(), std::next(name.cbegin(), 4), std::next(dataName.begin(), 16));

    dataValues[posSaveField] = slot;
    dataValues[1] = 0x03;
    dataValues[6] = 0x00;

    std::fill(std::next(dataValues.begin(), 16), dataValues.end(), 0x00);
    dataValues[19] = 0x00;
    dataValues[20] = 0x08;
    dataValues[21] = 0x01;
    dataValues[posKnob6] = 0x00;

    auto dataEffect0 = dataValues;
    dataEffect0[posFxKnob] = fxKnob;
    dataEffect0[posFxSlot] = settings[0].fx_slot;
    dataEffect0[posKnob1] = settings[0].knob1;
    dataEffect0[posKnob2] = settings[0].knob2;
    dataEffect0[posKnob3] = settings[0].knob3;
    dataEffect0[posKnob4] = settings[0].knob4;
    dataEffect0[posKnob5] = settings[0].knob5;
    dataEffect0[posDsp] = 0x08;
    dataEffect0[posEffect] = 0x16;
    dataEffect0[19] = 0x02;
    dataEffect0[20] = 0x01;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03, 0x00});
    cmdExecute[posFxKnob] = fxKnob;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataName), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    // Effect #0
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataEffect0), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    // Execute Cmd
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));


    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, saveEffectsReturnsErrorOnInvalidEffect)
{
    std::array<fx_pedal_settings, 1> settings{{fx_pedal_settings{1, value(effects::COMPRESSOR), 0, 1, 2, 3, 4, 5, false}}};
    constexpr int numOfEffects = settings.size();
    std::array<char, 24> name{{'a', 'b', 'c', 'd', '\0'}};

    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, saveEffectsHandlesEffectsWithDifferentFxKnobs)
{
    std::array<fx_pedal_settings, 1> settings{{fx_pedal_settings{1, value(effects::SINE_CHORUS), 0, 1, 2, 3, 4, 5, false}}};
    constexpr int numOfEffects = settings.size();
    constexpr int fxKnob{0x01};
    std::array<char, 24> name{{'a', 'b', 'c', 'd', '\0'}};
    std::array<std::uint8_t, packetSize> dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    std::array<std::uint8_t, packetSize> dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    dataName[posFxKnob] = fxKnob;
    dataName[posSaveField] = slot;
    std::copy(name.cbegin(), std::next(name.cbegin(), 4), std::next(dataName.begin(), 16));

    dataValues[posSaveField] = slot;
    dataValues[1] = 0x03;
    dataValues[6] = 0x00;

    std::fill(std::next(dataValues.begin(), 16), dataValues.end(), 0x00);
    dataValues[19] = 0x00;
    dataValues[20] = 0x08;
    dataValues[21] = 0x01;
    dataValues[posKnob6] = 0x00;

    auto dataEffect0 = dataValues;
    dataEffect0[posFxKnob] = fxKnob;
    dataEffect0[posFxSlot] = settings[0].fx_slot;
    dataEffect0[posKnob1] = settings[0].knob1;
    dataEffect0[posKnob2] = settings[0].knob2;
    dataEffect0[posKnob3] = settings[0].knob3;
    dataEffect0[posKnob4] = settings[0].knob4;
    dataEffect0[posKnob5] = settings[0].knob5;
    dataEffect0[posDsp] = 0x07;
    dataEffect0[posEffect] = 0x12;
    dataEffect0[19] = 0x01;
    dataEffect0[20] = 0x01;

    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03, 0x00});
    cmdExecute[posFxKnob] = fxKnob;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataName), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    // Effect #0
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataEffect0), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    // Execute Cmd
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(cmdExecute), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));


    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, saveEffectsEnsuresNameStringFormat)
{
    std::array<fx_pedal_settings, 1> settings{{fx_pedal_settings{1, value(effects::SINE_CHORUS), 0, 1, 2, 3, 4, 5, false}}};
    constexpr int numOfEffects = settings.size();
    constexpr int fxKnob{0x01};
    std::array<char, 24> name;
    name.fill('x');
    std::array<std::uint8_t, packetSize> dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    dataName[posFxKnob] = fxKnob;
    dataName[posSaveField] = slot;
    std::copy(name.cbegin(), name.cend(), std::next(dataName.begin(), 16));
    dataName[16 + name.size() - 1] = '\0';


    Sequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataName), packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(4)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));


    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, saveEffectsHandlesEffectsWithMoreControls)
{
    std::array<fx_pedal_settings, 1> settings{{fx_pedal_settings{1, value(effects::TAPE_DELAY), 0, 1, 2, 3, 4, 5, false}}};
    constexpr int numOfEffects = settings.size();
    constexpr int fxKnob{0x02};
    std::array<char, 24> name{{'a', 'b', 'c', 'd', '\0'}};
    std::array<std::uint8_t, packetSize> dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    dataValues[posSaveField] = slot;
    dataValues[1] = 0x03;
    dataValues[6] = 0x00;

    std::fill(std::next(dataValues.begin(), 16), dataValues.end(), 0x00);
    dataValues[19] = 0x00;
    dataValues[20] = 0x08;
    dataValues[21] = 0x01;
    dataValues[posKnob6] = 0x00;

    auto dataEffect0 = dataValues;
    dataEffect0[posFxKnob] = fxKnob;
    dataEffect0[posFxSlot] = settings[0].fx_slot;
    dataEffect0[posKnob1] = settings[0].knob1;
    dataEffect0[posKnob2] = settings[0].knob2;
    dataEffect0[posKnob3] = settings[0].knob3;
    dataEffect0[posKnob4] = settings[0].knob4;
    dataEffect0[posKnob5] = settings[0].knob5;
    dataEffect0[posKnob6] = settings[0].knob6;
    dataEffect0[posDsp] = 0x08;
    dataEffect0[posEffect] = 0x2b;
    dataEffect0[19] = 0x02;
    dataEffect0[20] = 0x01;


    Sequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));

    // Effect #0
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(dataEffect0), packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(dummy.size()), Return(0)));
    // Execute Cmd
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));


    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsSuccessful());
}

TEST_F(MustangTest, saveEffectsReturnsErrorOnFailure)
{
    std::array<fx_pedal_settings, 2> settings{{fx_pedal_settings{1, value(effects::MONO_DELAY), 0, 1, 2, 3, 4, 5, false},
                                               fx_pedal_settings{2, value(effects::SINE_FLANGER), 6, 7, 8, 0, 0, 0, true}

    }};
    constexpr int numOfEffects = settings.size();
    std::array<char, 24> name{{'a', 'b', 'c', 'd', '\0'}};

    Sequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, _, _, packetSize, _, _))
        .Times(6)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(usbError)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));


    const auto result = m->save_effects(slot, name.data(), numOfEffects, settings.data());
    EXPECT_THAT(result, IsFailure());
}

TEST_F(MustangTest, saveOnAmp)
{
    auto sendCmd = helper::createInitializedPacket({0x1c, 0x01, 0x03});
    sendCmd[posSlot] = slot;
    sendCmd[6] = 0x01;
    sendCmd[7] = 0x01;
    std::array<char, 34> nameOversized;
    nameOversized.fill('a');
    std::copy_n(nameOversized.cbegin(), 31, std::next(sendCmd.begin(), 16));
    nameOversized[31] = char{0x0f};
    nameOversized[32] = 'b';
    nameOversized[33] = '\0';

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(sendCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));

    auto memBank = helper::createInitializedPacket({0x1c, 0x01, 0x01});
    memBank[posSlot] = slot;
    memBank[6] = 0x01;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(memBank), _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(0), Return(0)));

    const auto result = m->save_on_amp(nameOversized.data(), slot);
    EXPECT_THAT(result, IsSuccessful());
}
