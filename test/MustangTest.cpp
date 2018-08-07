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

#include "com/Mustang.h"
#include "com/CommunicationException.h"
#include "mocks/LibUsbMocks.h"
#include "helper/PacketConstants.h"
#include "helper/Matcher.h"
#include "helper/PacketHelper.h"
#include <array>
#include <gmock/gmock.h>

using namespace plug;
using namespace plug::com;
using namespace test;
using namespace test::matcher;
using namespace test::constants;
using namespace testing;
using mock::UsbMock;

class MustangTest : public testing::Test
{
protected:
    void SetUp() override
    {
        m = std::make_unique<com::Mustang>();
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

    void expectReceiveBankData(std::array<Packet, 7>& data)
    {
        EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
            .WillOnce(DoAll(SetArrayArgument<2>(data[0].cbegin(), data[0].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(data[1].cbegin(), data[1].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(data[2].cbegin(), data[2].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(data[3].cbegin(), data[3].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(data[4].cbegin(), data[4].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(data[5].cbegin(), data[5].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(data[6].cbegin(), data[6].cend()), SetArgPointee<4>(packetSize), Return(0)))
            .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(0), Return(0)));
    }


    std::unique_ptr<com::Mustang> m;
    mock::UsbMock* usbmock;
    libusb_device_handle handle{};
    const Packet dummy = helper::createEmptyPacket();
    const Packet initCmd = helper::createInitCmdPacket();
    static inline constexpr int usbSuccess{LIBUSB_SUCCESS};
    static inline constexpr int usbError{LIBUSB_ERROR_NO_DEVICE};
    static inline constexpr int slot{5};
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

    m->start_amp(nullptr, nullptr, nullptr, nullptr);

    ignoreClose();
}

TEST_F(MustangTest, startHandlesErrorOnInitFailure)
{
    EXPECT_CALL(*usbmock, init(nullptr)).WillOnce(Return(usbSuccess));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _))
        .WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(_, _)).WillOnce(Return(usbSuccess));
    EXPECT_CALL(*usbmock, claim_interface(_, _)).WillOnce(Return(usbError));

    EXPECT_THROW(m->start_amp(nullptr, nullptr, nullptr, nullptr), plug::com::CommunicationException);

    ignoreClose();
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

    m->start_amp(nullptr, nullptr, nullptr, nullptr);

    ignoreClose();
}

TEST_F(MustangTest, startFailsIfNoDeviceFound)
{
    InSequence s;
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, _, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(nullptr));

    EXPECT_THROW(m->start_amp(nullptr, nullptr, nullptr, nullptr), plug::com::CommunicationException);
}

TEST_F(MustangTest, startRequestsCurrentPresetName)
{
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    auto recvData = helper::createEmptyNamedPacket("abc");
    const int recvSize = recvData.size();
    const int recvSizeResponse = recvSize;

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
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(0), Return(0)));

    char nameList[100][nameLength];
    std::array<char, nameLength> name{};
    m->start_amp(nameList, name.data(), nullptr, nullptr);
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
    const auto recvSize = recvData.size();

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    Sequence s;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(200)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .Times(5)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(recvData.cbegin(), recvData.cend()), SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArrayArgument<2>(extendedData.cbegin(), extendedData.cend()), SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(0), Return(0)));

    char nameList[100][32];
    amp_settings settings{};
    m->start_amp(nameList, nullptr, &settings, nullptr);
    EXPECT_THAT(settings.amp_num, Eq(amps::BRITISH_60S));
    EXPECT_THAT(settings.volume, Eq(recvData[volumePos]));
    EXPECT_THAT(settings.gain, Eq(recvData[gainPos]));
    EXPECT_THAT(settings.treble, Eq(recvData[treblePos]));
    EXPECT_THAT(settings.middle, Eq(recvData[middlePos]));
    EXPECT_THAT(settings.bass, Eq(recvData[bassPos]));
    EXPECT_THAT(settings.cabinet, Eq(cabinets::cabBSSMN));
    EXPECT_THAT(settings.noise_gate, Eq(recvData[noiseGatePos]));
    EXPECT_THAT(settings.threshold, Eq(recvData[thresholdPos]));
    EXPECT_THAT(settings.master_vol, Eq(recvData[masterVolPos]));
    EXPECT_THAT(settings.gain2, Eq(recvData[gain2Pos]));
    EXPECT_THAT(settings.presence, Eq(recvData[presencePos]));
    EXPECT_THAT(settings.depth, Eq(recvData[depthPos]));
    EXPECT_THAT(settings.bias, Eq(recvData[biasPos]));
    EXPECT_THAT(settings.sag, Eq(recvData[sagPos]));
    EXPECT_THAT(settings.brightness, Eq(recvData[brightnessPos]));
    EXPECT_THAT(settings.usb_gain, Eq(extendedData[usbGainPos]));

    ignoreClose();
}

TEST_F(MustangTest, startRequestsCurrentEffects)
{
    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    const auto recvData0 = createEffectData(0x04, 0x19, {{10, 20, 30, 40, 50, 60}});
    const auto recvData1 = createEffectData(0x01, 0x13, {{0, 0, 0, 1, 1, 1}});
    const auto recvData2 = createEffectData(0x02, 0x00, {{0, 0, 0, 0, 0, 0}});
    const auto recvData3 = createEffectData(0x07, 0x2b, {{1, 2, 3, 4, 5, 6}});
    const int recvSize = recvData0.size();

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .Times(2)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    Sequence s;

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(2)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .Times(200)
        .InSequence(s)
        .WillRepeatedly(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, packetSize, _, _))
        .InSequence(s)
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData0.cbegin(), recvData0.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData1.cbegin(), recvData1.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData2.cbegin(), recvData2.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData3.cbegin(), recvData3.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(0), Return(0)));

    char nameList[100][32];
    std::array<fx_pedal_settings, 4> settings{};
    m->start_amp(nameList, nullptr, nullptr, settings.data());
    EXPECT_THAT(settings[0].fx_slot, Eq(0));
    EXPECT_THAT(settings[0].knob1, Eq(10));
    EXPECT_THAT(settings[0].knob2, Eq(20));
    EXPECT_THAT(settings[0].knob3, Eq(30));
    EXPECT_THAT(settings[0].knob4, Eq(40));
    EXPECT_THAT(settings[0].knob5, Eq(50));
    EXPECT_THAT(settings[0].knob6, Eq(60));
    EXPECT_THAT(settings[0].position, Eq(Position::effectsLoop));
    EXPECT_THAT(settings[0].effect_num, Eq(effects::TRIANGLE_FLANGER));

    ignoreClose();
}

TEST_F(MustangTest, startRequestsAmpPresetList)
{
    const auto recvData0 = helper::createEmptyNamedPacket("abc");
    const auto recvData1 = helper::createEmptyNamedPacket("def");
    const auto recvData2 = helper::createEmptyNamedPacket("ghi");

    EXPECT_CALL(*usbmock, init(nullptr));
    EXPECT_CALL(*usbmock, open_device_with_vid_pid(nullptr, usbVid, _)).WillOnce(Return(&handle));
    EXPECT_CALL(*usbmock, kernel_driver_active(&handle, 0)).WillOnce(Return(0));
    EXPECT_CALL(*usbmock, claim_interface(&handle, 0)).WillOnce(Return(0));

    const int recvSize = recvData0.size();

    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, _, _, _))
        .Times(2)
        .WillRepeatedly(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointReceive, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData0.cbegin(), recvData0.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData1.cbegin(), recvData1.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(recvData2.cbegin(), recvData2.cend()), SetArgPointee<4>(recvSize), Return(0)))
        .WillOnce(DoAll(SetArrayArgument<2>(dummy.cbegin(), dummy.cend()), SetArgPointee<4>(0), Return(0)));

    EXPECT_CALL(*usbmock, interrupt_transfer(&handle, endpointSend, BufferIs(initCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    constexpr std::size_t numberOfNames{100};
    char names[numberOfNames][nameLength];

    m->start_amp(names, nullptr, nullptr, nullptr);
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

    m->start_amp(nullptr, nullptr, nullptr, nullptr);
    m->start_amp(nullptr, nullptr, nullptr, nullptr);

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
    auto sendCmd = helper::createInitializedPacket({0x1c, 0x01, 0x01});
    const int recvSize = sendCmd.size();
    sendCmd[posSlot] = slot;
    sendCmd[6] = 0x01;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, BufferIs(sendCmd), packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    auto ampDummy = helper::createEmptyPacket();
    ampDummy[ampPos] = 0x5e;

    std::array<Packet, 7> data{{dummy, ampDummy, dummy, dummy, dummy, dummy, dummy}};
    expectReceiveBankData(data);

    m->load_memory_bank(slot);
}

TEST_F(MustangTest, loadMemoryBankReceivesName)
{
    auto recvData = helper::createEmptyNamedPacket("abc");
    const auto recvSize = recvData.size();

    auto ampDummy = helper::createEmptyPacket();
    ampDummy[ampPos] = 0x5e;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));
    std::array<Packet, 7> data{{recvData, ampDummy, dummy, dummy, dummy, dummy, dummy}};
    expectReceiveBankData(data);

    const auto [name, amp, effects] = m->load_memory_bank(slot);
    EXPECT_THAT(name, StrEq("abc"));
    static_cast<void>(amp);
    static_cast<void>(effects);
}

TEST_F(MustangTest, loadMemoryBankReceivesAmpValues)
{
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
    const auto recvSize = recvData.size();


    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    InSequence s;
    std::array<Packet, 7> data{{dummy, recvData, dummy, dummy, dummy, dummy, extendedData}};
    expectReceiveBankData(data);

    const auto [name, settings, effects] = m->load_memory_bank(slot);
    EXPECT_THAT(settings.amp_num, Eq(amps::BRITISH_80S));
    EXPECT_THAT(settings.volume, Eq(recvData[volumePos]));
    EXPECT_THAT(settings.gain, Eq(recvData[gainPos]));
    EXPECT_THAT(settings.treble, Eq(recvData[treblePos]));
    EXPECT_THAT(settings.middle, Eq(recvData[middlePos]));
    EXPECT_THAT(settings.bass, Eq(recvData[bassPos]));
    EXPECT_THAT(settings.cabinet, Eq(cabinets::cab4x12M));
    EXPECT_THAT(settings.noise_gate, Eq(recvData[noiseGatePos]));
    EXPECT_THAT(settings.threshold, Eq(recvData[thresholdPos]));
    EXPECT_THAT(settings.master_vol, Eq(recvData[masterVolPos]));
    EXPECT_THAT(settings.gain2, Eq(recvData[gain2Pos]));
    EXPECT_THAT(settings.presence, Eq(recvData[presencePos]));
    EXPECT_THAT(settings.depth, Eq(recvData[depthPos]));
    EXPECT_THAT(settings.bias, Eq(recvData[biasPos]));
    EXPECT_THAT(settings.sag, Eq(recvData[sagPos]));
    EXPECT_THAT(settings.brightness, Eq(recvData[brightnessPos]));
    EXPECT_THAT(settings.usb_gain, Eq(extendedData[usbGainPos]));
    static_cast<void>(name);
    static_cast<void>(effects);
}

TEST_F(MustangTest, loadMemoryBankReceivesEffectValues)
{
    const auto recvData0 = createEffectData(0x04, 0x4f, {{11, 22, 33, 44, 55, 66}});
    const auto recvData1 = createEffectData(0x01, 0x13, {{0, 0, 0, 1, 1, 1}});
    const auto recvData2 = createEffectData(0x02, 0x00, {{0, 0, 0, 0, 0, 0}});
    const auto recvData3 = createEffectData(0x07, 0x2b, {{1, 2, 3, 4, 5, 6}});
    const auto recvSize = recvData0.size();

    auto ampDummy = helper::createEmptyPacket();
    ampDummy[ampPos] = 0x5e;

    InSequence s;
    EXPECT_CALL(*usbmock, interrupt_transfer(_, endpointSend, _, packetSize, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(recvSize), Return(0)));

    std::array<Packet, 7> data{{dummy, ampDummy, recvData0, recvData1, recvData2, recvData3, dummy}};
    expectReceiveBankData(data);

    const auto [name, amp, settings] = m->load_memory_bank(slot);
    EXPECT_THAT(settings[0].fx_slot, Eq(0));
    EXPECT_THAT(settings[0].knob1, Eq(11));
    EXPECT_THAT(settings[0].knob2, Eq(22));
    EXPECT_THAT(settings[0].knob3, Eq(33));
    EXPECT_THAT(settings[0].knob4, Eq(44));
    EXPECT_THAT(settings[0].knob5, Eq(55));
    EXPECT_THAT(settings[0].knob6, Eq(66));
    EXPECT_THAT(settings[0].position, Eq(Position::effectsLoop));
    EXPECT_THAT(settings[0].effect_num, Eq(effects::PHASER));

    EXPECT_THAT(settings[1].fx_slot, Eq(1));
    EXPECT_THAT(settings[1].knob1, Eq(0));
    EXPECT_THAT(settings[1].knob2, Eq(0));
    EXPECT_THAT(settings[1].knob3, Eq(0));
    EXPECT_THAT(settings[1].knob4, Eq(1));
    EXPECT_THAT(settings[1].knob5, Eq(1));
    EXPECT_THAT(settings[1].knob6, Eq(1));
    EXPECT_THAT(settings[1].position, Eq(Position::input));
    EXPECT_THAT(settings[1].effect_num, Eq(effects::TRIANGLE_CHORUS));

    EXPECT_THAT(settings[2].fx_slot, Eq(2));
    EXPECT_THAT(settings[2].knob1, Eq(0));
    EXPECT_THAT(settings[2].knob2, Eq(0));
    EXPECT_THAT(settings[2].knob3, Eq(0));
    EXPECT_THAT(settings[2].knob4, Eq(0));
    EXPECT_THAT(settings[2].knob5, Eq(0));
    EXPECT_THAT(settings[2].knob6, Eq(0));
    EXPECT_THAT(settings[2].position, Eq(Position::input));
    EXPECT_THAT(settings[2].effect_num, Eq(effects::EMPTY));

    EXPECT_THAT(settings[3].fx_slot, Eq(3));
    EXPECT_THAT(settings[3].knob1, Eq(1));
    EXPECT_THAT(settings[3].knob2, Eq(2));
    EXPECT_THAT(settings[3].knob3, Eq(3));
    EXPECT_THAT(settings[3].knob4, Eq(4));
    EXPECT_THAT(settings[3].knob5, Eq(5));
    EXPECT_THAT(settings[3].knob6, Eq(6));
    EXPECT_THAT(settings[3].position, Eq(Position::effectsLoop));
    EXPECT_THAT(settings[3].effect_num, Eq(effects::TAPE_DELAY));
    static_cast<void>(name);
    static_cast<void>(amp);
}

TEST_F(MustangTest, setAmpSendsValues)
{
    const amp_settings settings{amps::BRITISH_70S, 8, 9, 1, 2, 3,
                                cabinets::cab4x12G, 3, 5, 3, 2, 1,
                                4, 1, 5, true, 4};

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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
    data[cabinetPos] = value(settings.cabinet);
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
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
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

    m->set_amplifier(settings);
}

TEST_F(MustangTest, setAmpHandlesOutOfRangeCabinet)
{
    constexpr int invalidCabinet{0x0d};
    const amp_settings settings{amps::FENDER_57_CHAMP, 8, 9, 1, 2, 3, static_cast<cabinets>(invalidCabinet),
                                3, 5, 3, 2, 1, 4, 1, 5, true, 4};

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
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

    m->set_amplifier(settings);
}

TEST_F(MustangTest, setAmpHandlesNoiseGate)
{
    constexpr int limitValue{0x05};
    const amp_settings settings{amps::FENDER_SUPER_SONIC, 8, 9, 1, 2, 3,
                                cabinets::cab57DLX, limitValue, 5, 3, 2,
                                7, 4, 1, 5, true, 4};

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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
    data[cabinetPos] = value(settings.cabinet);
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
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
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

    m->set_amplifier(settings);
}

TEST_F(MustangTest, setAmpHandlesNoiseGateAndOutOfRangeThreshold)
{
    constexpr int limitValue{0x05};
    constexpr int outOfRange{0x10};
    const amp_settings settings{amps::FENDER_SUPER_SONIC, 8, 9, 1, 2, 3,
                                cabinets::cab57DLX, limitValue, 5, 3, 2,
                                outOfRange, 4, 1, 5, true, 4};

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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
    data[cabinetPos] = value(settings.cabinet);
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
    const auto cmdExecute = helper::createInitializedPacket({0x01c, 0x03});
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

    m->set_amplifier(settings);
}

TEST_F(MustangTest, setAmpHandlesSagValue)
{
    constexpr int valueInRange{0x00};
    const amp_settings settings{amps::FENDER_SUPER_SONIC, 8, 9, 1, 2, 3,
                                cabinets::cab57DLX, 5, 5,
                                3, 2, 7, 4, 1, valueInRange, true, 4};

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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
    data[cabinetPos] = value(settings.cabinet);
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
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
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

    m->set_amplifier(settings);
}

TEST_F(MustangTest, setAmpHandlesOutOfRangeNoiseGate)
{
    constexpr int outOfRange{0x06};
    constexpr int expectedValue{0x00};
    const amp_settings settings{amps::BRITISH_60S, 8, 9, 1, 2, 3,
                                cabinets::cab57DLX, outOfRange, 5,
                                3, 2, 7, 4, 1, 5, true, 4};

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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
    data[cabinetPos] = value(settings.cabinet);
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
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
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

    m->set_amplifier(settings);
}

TEST_F(MustangTest, setEffectSendsValue)
{
    const fx_pedal_settings settings{3, effects::OVERDRIVE, 8, 7, 6, 5, 4, 3, Position::input};
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    Packet clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    m->set_effect(settings);
}

TEST_F(MustangTest, setEffectClearsEffectIfEmptyEffect)
{
    const fx_pedal_settings settings{2, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    Packet clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    m->set_effect(settings);
}

TEST_F(MustangTest, setEffectHandlesEffectPosition)
{
    const fx_pedal_settings settings{2, effects::OVERDRIVE, 1, 2, 3, 4, 5, 6, Position::effectsLoop};
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    Packet clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    m->set_effect(settings);
}

TEST_F(MustangTest, setEffectHandlesEffectsWithMoreControls)
{
    const fx_pedal_settings settings{3, effects::STEREO_TAPE_DELAY, 1, 1, 1, 1, 1, 7, Position::input};
    const auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03});
    Packet clearCmd{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

    m->set_effect(settings);
}

TEST_F(MustangTest, saveEffectsSendsValues)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::MONO_DELAY, 0, 1, 2, 3, 4, 5, Position::input},
                                            fx_pedal_settings{2, effects::SINE_FLANGER, 6, 7, 8, 0, 0, 0, Position::effectsLoop}};
    constexpr int fxKnob{0x02};
    constexpr int postAmpOffset{4};
    const std::string name = "abcd";
    Packet dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    Packet dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
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


    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveEffectsLimitsNumberOfValues)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::MONO_DELAY, 0, 1, 2, 3, 4, 5, Position::input},
                                            fx_pedal_settings{2, effects::SINE_FLANGER, 6, 7, 8, 0, 0, 0, Position::effectsLoop},
                                            fx_pedal_settings{3, effects::SINE_FLANGER, 1, 2, 2, 1, 0, 4, Position::effectsLoop}};
    constexpr int fxKnob{0x02};
    const std::string name = "abcd";
    Packet dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    Packet dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
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


    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveEffectsDoesNothingOnInvalidEffect)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::COMPRESSOR, 0, 1, 2, 3, 4, 5, Position::input}};
    const std::string name = "abcd";

    EXPECT_THROW(m->save_effects(slot, name, settings), std::invalid_argument);
}

TEST_F(MustangTest, saveEffectsHandlesEffectsWithDifferentFxKnobs)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::SINE_CHORUS, 0, 1, 2, 3, 4, 5, Position::input}};
    constexpr int fxKnob{0x01};
    const std::string name = "abcd";
    Packet dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    Packet dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
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


    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveEffectsEnsuresNameStringFormat)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::SINE_CHORUS, 0, 1, 2, 3, 4, 5, Position::input}};
    constexpr int fxKnob{0x01};
    constexpr std::size_t nameSize{22};
    const std::string name(24, 'x');
    Packet dataName{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    dataName[posFxKnob] = fxKnob;
    dataName[posSaveField] = slot;
    std::copy(name.cbegin(), std::next(name.cbegin(), nameSize), std::next(dataName.begin(), 16));


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


    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveEffectsHandlesEffectsWithMoreControls)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::TAPE_DELAY, 0, 1, 2, 3, 4, 5, Position::input}};
    constexpr int fxKnob{0x02};
    const std::string name = "abcd";
    Packet dataValues{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
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


    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveOnAmp)
{
    constexpr std::size_t length{30};
    auto sendCmd = helper::createInitializedPacket({0x1c, 0x01, 0x03});
    sendCmd[posSlot] = slot;
    sendCmd[6] = 0x01;
    sendCmd[7] = 0x01;

    const std::string name(length, 'x');
    std::copy(name.cbegin(), name.cend(), std::next(sendCmd.begin(), 16));

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

    m->save_on_amp(name, slot);
}

TEST_F(MustangTest, saveOnAmpFillsShortName)
{
    constexpr std::size_t length{16};
    auto sendCmd = helper::createInitializedPacket({0x1c, 0x01, 0x03});
    sendCmd[posSlot] = slot;
    sendCmd[6] = 0x01;
    sendCmd[7] = 0x01;

    const std::string name(length, 'x');
    std::copy(name.cbegin(), name.cend(), std::next(sendCmd.begin(), 16));

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

    m->save_on_amp(name, slot);
}

TEST_F(MustangTest, saveOnAmpLimitsOversizedName)
{
    auto sendCmd = helper::createInitializedPacket({0x1c, 0x01, 0x03});
    sendCmd[posSlot] = slot;
    sendCmd[6] = 0x01;
    sendCmd[7] = 0x01;
    std::string nameOversized(34, 'a');
    nameOversized[31] = char{0x0f};
    nameOversized[32] = 'b';
    nameOversized[33] = '\0';
    std::copy_n(nameOversized.cbegin(), 31, std::next(sendCmd.begin(), 16));

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

    m->save_on_amp(nameOversized, slot);
}
