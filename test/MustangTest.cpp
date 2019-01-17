/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2019  offa
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
#include "com/Packet.h"
#include "com/PacketSerializer.h"
#include "com/CommunicationException.h"
#include "helper/PacketConstants.h"
#include "helper/PacketHelper.h"
#include "mocks/MockConnection.h"
#include "matcher/Matcher.h"
#include <array>
#include <gmock/gmock.h>

using namespace plug;
using namespace plug::com;
using namespace test;
using namespace test::matcher;
using namespace test::constants;
using namespace testing;

class MustangTest : public testing::Test
{
protected:
    void SetUp() override
    {
        conn = std::make_shared<mock::MockConnection>();
        m = std::make_unique<com::Mustang>(conn);
    }

    void TearDown() override
    {
    }

    void ignoreClose()
    {
        EXPECT_CALL(*conn, close());
    }

    std::vector<std::uint8_t> createEmptyPacketData() const
    {
        return std::vector<std::uint8_t>(packetSize, 0x00);
    }

    auto createEffectData(std::uint8_t slotValue, std::uint8_t effect, std::array<std::uint8_t, 6> values) const
    {
        std::vector<std::uint8_t> data(packetSize, 0x00);
        data[posDsp] = 0x08;
        data[posEffect] = effect;
        data[posFxSlot] = slotValue;
        std::copy(values.cbegin(), values.cend(), std::next(data.begin(), posKnob1));
        return data;
    }

    template<class Container>
    auto asBuffer(const Container& c) const
    {
        return std::vector<std::uint8_t>{std::cbegin(c), std::cend(c)};
    }


    std::shared_ptr<mock::MockConnection> conn;
    std::unique_ptr<com::Mustang> m;
    const std::vector<std::uint8_t> noData{};
    const std::vector<std::uint8_t> ignoreData = std::vector<std::uint8_t>(packetSize);
    const std::vector<std::uint8_t> ignoreAmpData = []() { std::vector<std::uint8_t> d(packetSize, 0x00); d[ampPos] = 0x5e; return d; }();
    const Packet loadCmd = serializeLoadCommand();
    static inline constexpr int slot{5};
};

TEST_F(MustangTest, startInitializesUsb)
{
    const auto [initCmd1, initCmd2] = serializeInitCommand();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{48};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    m->start_amp();

    ignoreClose();
}

TEST_F(MustangTest, startPropagatesErrorOnInitFailure)
{
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _)).WillOnce(Throw(plug::com::CommunicationException{"expected"}));

    EXPECT_THROW(m->start_amp(), plug::com::CommunicationException);

    ignoreClose();
}

TEST_F(MustangTest, startRequestsCurrentPresetName)
{
    const auto [initCmd1, initCmd2] = serializeInitCommand();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{48};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    const std::string actualName{"abc"};
    const auto nameData = asBuffer(serializeName(0, actualName));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(nameData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto [bank, presets] = m->start_amp();

    const auto name = std::get<0>(bank);
    EXPECT_THAT(name, StrEq(actualName));

    ignoreClose();
    static_cast<void>(presets);
}

TEST_F(MustangTest, startRequestsCurrentAmp)
{
    auto recvData = createEmptyPacketData();
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
    auto extendedData = createEmptyPacketData();
    extendedData[usbGainPos] = 0x44;

    const auto [initCmd1, initCmd2] = serializeInitCommand();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{48};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(extendedData))
        .WillOnce(Return(noData));



    const auto [bank, presets] = m->start_amp();
    const auto settings = std::get<1>(bank);
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
    static_cast<void>(presets);
}

TEST_F(MustangTest, startRequestsCurrentEffects)
{
    const auto recvData0 = createEffectData(0x04, 0x19, {{10, 20, 30, 40, 50, 60}});
    const auto recvData1 = createEffectData(0x01, 0x13, {{0, 0, 0, 1, 1, 1}});
    const auto recvData2 = createEffectData(0x02, 0x00, {{0, 0, 0, 0, 0, 0}});
    const auto recvData3 = createEffectData(0x07, 0x2b, {{1, 2, 3, 4, 5, 6}});

    const auto [initCmd1, initCmd2] = serializeInitCommand();


    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{48};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(recvData0))
        .WillOnce(Return(recvData1))
        .WillOnce(Return(recvData2))
        .WillOnce(Return(recvData3))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto [bank, presets] = m->start_amp();
    std::array<fx_pedal_settings, 4> settings = std::get<2>(bank);

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
    static_cast<void>(presets);
}

TEST_F(MustangTest, startRequestsAmpPresetList)
{
    const auto [initCmd1, initCmd2] = serializeInitCommand();
    const auto recvData0 = asBuffer(serializeName(0, "abc"));
    const auto recvData1 = asBuffer(serializeName(0, "def"));
    const auto recvData2 = asBuffer(serializeName(0, "ghi"));


    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{48};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(recvData0))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData1))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData2))
        .WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive - 6).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto [bank, presetList] = m->start_amp();

    EXPECT_THAT(presetList.size(), Eq(maxToReceive / 2));
    EXPECT_THAT(presetList[0], StrEq("abc"));
    EXPECT_THAT(presetList[1], StrEq("def"));
    EXPECT_THAT(presetList[2], StrEq("ghi"));

    ignoreClose();
    static_cast<void>(bank);
}

TEST_F(MustangTest, startUsesFullInitialTransmissionSizeIfOverThreshold)
{
    const auto [initCmd1, initCmd2] = serializeInitCommand();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{200};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    m->start_amp();

    ignoreClose();
}

TEST_F(MustangTest, startDoesNotInitializeUsbIfCalledMultipleTimes)
{
    const auto [initCmd1, initCmd2] = serializeInitCommand();

    InSequence s;
    // #1
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_CALL(*conn, openFirst(_, _));

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    constexpr size_t maxToReceive{200};
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    // #2
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));
    EXPECT_CALL(*conn, openFirst(_, _)).Times(0);

    // Init commands
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).Times(maxToReceive).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));



    m->start_amp();
    m->start_amp();

    ignoreClose();
}

TEST_F(MustangTest, DISABLED_stopAmpDoesNothingIfNotStartedYet)
{
    // FIXME: Update test (#152)
    m->stop_amp();
}

TEST_F(MustangTest, stopAmpClosesConnection)
{
    EXPECT_CALL(*conn, close());
    m->stop_amp();

    ignoreClose();
}

TEST_F(MustangTest, loadMemoryBankSendsBankSelectionCommandAndReceivesPacket)
{
    const auto loadCmd = serializeLoadSlotCommand(slot);

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    m->load_memory_bank(slot);
    ignoreClose();
}

TEST_F(MustangTest, loadMemoryBankReceivesName)
{
    const auto recvData = asBuffer(serializeName(0, "abc"));

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, _, _)).WillOnce(Return(packetSize));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(recvData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));

    const auto [name, amp, effects] = m->load_memory_bank(slot);
    EXPECT_THAT(name, StrEq("abc"));
    static_cast<void>(amp);
    static_cast<void>(effects);
    ignoreClose();
}

TEST_F(MustangTest, loadMemoryBankReceivesAmpValues)
{
    auto recvData = createEmptyPacketData();
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
    auto extendedData = createEmptyPacketData();
    extendedData[usbGainPos] = 0xab;

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, _, _)).WillOnce(Return(packetSize));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(extendedData))
        .WillOnce(Return(noData));


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
    ignoreClose();
}

TEST_F(MustangTest, loadMemoryBankReceivesEffectValues)
{
    const auto recvData0 = createEffectData(0x04, 0x4f, {{11, 22, 33, 44, 55, 66}});
    const auto recvData1 = createEffectData(0x01, 0x13, {{0, 0, 0, 1, 1, 1}});
    const auto recvData2 = createEffectData(0x02, 0x00, {{0, 0, 0, 0, 0, 0}});
    const auto recvData3 = createEffectData(0x07, 0x2b, {{1, 2, 3, 4, 5, 6}});


    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, _, _)).WillOnce(Return(packetSize));

    // Data
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(recvData0))
        .WillOnce(Return(recvData1))
        .WillOnce(Return(recvData2))
        .WillOnce(Return(recvData3))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


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
    ignoreClose();
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
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_amplifier(settings);
    ignoreClose();
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
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_amplifier(settings);
    ignoreClose();
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
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_amplifier(settings);
    ignoreClose();
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
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    m->set_amplifier(settings);
    ignoreClose();
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


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
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
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
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


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
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


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(cmdExecute.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
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
    // Save effect name cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataName), dataName.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Effect #0
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataEffect0), dataEffect0.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Effect #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataEffect1), dataEffect1.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));


    m->save_effects(slot, name, settings);
    ignoreClose();
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
    // Save effect cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataName), dataName.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Effect #0
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataEffect0), dataEffect0.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    m->save_effects(slot, name, settings);
    ignoreClose();
}

TEST_F(MustangTest, saveEffectsDoesNothingOnInvalidEffect)
{
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::COMPRESSOR, 0, 1, 2, 3, 4, 5, Position::input}};

    EXPECT_THROW(m->save_effects(slot, "abcd", settings), std::invalid_argument);
    ignoreClose();
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
    // Save effect cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataName), dataName.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Effect #0
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataEffect0), dataEffect0.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));


    m->save_effects(slot, name, settings);
    ignoreClose();
}

TEST_F(MustangTest, saveEffectsEnsuresNameStringFormat)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::SINE_CHORUS, 0, 1, 2, 3, 4, 5, Position::input}};
    constexpr int fxKnob{0x01};
    constexpr std::size_t nameSize{24};
    const std::string name(26, 'x');
    const std::string nameExpected{name.cbegin(), std::next(name.cbegin(), nameSize - 1)};
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03, 0x00});
    cmdExecute[posFxKnob] = fxKnob;

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
    std::copy(name.cbegin(), std::next(name.cbegin(), nameSize - 1), std::next(dataName.begin(), 16));

    InSequence s;
    // Save effect cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataName), dataName.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));

    // Effect #0
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, _)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));


    m->save_effects(slot, nameExpected, settings);
    ignoreClose();
}

TEST_F(MustangTest, saveEffectsHandlesEffectsWithMoreControls)
{
    std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::TAPE_DELAY, 0, 1, 2, 3, 4, 5, Position::input}};
    constexpr int fxKnob{0x02};
    auto cmdExecute = helper::createInitializedPacket({0x1c, 0x03, 0x00});
    cmdExecute[posFxKnob] = fxKnob;
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


    InSequence s;
    // Save effect cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, _)).WillOnce(Return(noData));

    // Effect #0
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(dataEffect0), dataEffect0.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, _)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));


    m->save_effects(slot, name, settings);
    ignoreClose();
}

TEST_F(MustangTest, saveOnAmp)
{
    const std::string name(30, 'x');
    const auto saveNamePacket = serializeName(slot, name);
    const auto loadSlotCmd = serializeLoadSlotCommand(slot);

    InSequence s;
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(saveNamePacket), saveNamePacket.size())).WillOnce(Return(saveNamePacket.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(noData));
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadSlotCmd), loadSlotCmd.size())).WillOnce(Return(0));


    m->save_on_amp(name, slot);
    ignoreClose();
}

