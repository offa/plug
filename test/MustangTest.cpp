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
    const Packet applyCmd = serializeApplyCommand();
    const Packet clearCmd = serializeClearEffectSettings();
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
    const auto loadSlotCmd = serializeLoadSlotCommand(slot);

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(loadSlotCmd), loadSlotCmd.size())).WillOnce(Return(loadSlotCmd.size()));

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

    const amp_settings as{amps::BRITISH_80S, 2, 1, 3, 4, 5,
                            cabinets::cab4x12M, 0, 9, 10, 11,
                            0, 0x80, 13, 1, false, 0xab};

    const auto recvData = asBuffer(serializeAmpSettings(as));
    const auto extendedData = asBuffer(serializeAmpSettingsUsbGain(as));

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
    EXPECT_THAT(settings.amp_num, Eq(as.amp_num));
    EXPECT_THAT(settings.volume, Eq(as.volume));
    EXPECT_THAT(settings.gain, Eq(as.gain));
    EXPECT_THAT(settings.treble, Eq(as.treble));
    EXPECT_THAT(settings.middle, Eq(as.middle));
    EXPECT_THAT(settings.bass, Eq(as.bass));
    EXPECT_THAT(settings.cabinet, Eq(as.cabinet));
    EXPECT_THAT(settings.noise_gate, Eq(as.noise_gate));
    EXPECT_THAT(settings.threshold, Eq(as.threshold));
    EXPECT_THAT(settings.master_vol, Eq(as.master_vol));
    EXPECT_THAT(settings.gain2, Eq(as.gain2));
    EXPECT_THAT(settings.presence, Eq(as.presence));
    EXPECT_THAT(settings.depth, Eq(as.depth));
    EXPECT_THAT(settings.bias, Eq(as.bias));
    EXPECT_THAT(settings.sag, Eq(as.sag));
    EXPECT_THAT(settings.brightness, Eq(as.brightness));
    EXPECT_THAT(settings.usb_gain, Eq(as.usb_gain));
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

    const auto data = serializeAmpSettings(settings);
    const auto data2 = serializeAmpSettingsUsbGain(settings);


    InSequence s;
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
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

    const auto data = serializeAmpSettings(settings);
    const auto data2 = serializeAmpSettingsUsbGain(settings);


    InSequence s;
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
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

    const auto data = serializeAmpSettings(settings);
    const auto data2 = serializeAmpSettingsUsbGain(settings);


    InSequence s;
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_amplifier(settings);
    ignoreClose();
}

TEST_F(MustangTest, setAmpHandlesOutOfRangeNoiseGate)
{
    constexpr int outOfRange{0x06};
    const amp_settings settings{amps::BRITISH_60S, 8, 9, 1, 2, 3,
                                cabinets::cab57DLX, outOfRange, 5,
                                3, 2, 7, 4, 1, 5, true, 4};

    const auto data = serializeAmpSettings(settings);
    const auto data2 = serializeAmpSettingsUsbGain(settings);


    InSequence s;
    // Data #1
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    m->set_amplifier(settings);
    ignoreClose();
}

TEST_F(MustangTest, setEffectSendsValue)
{
    const fx_pedal_settings settings{3, effects::OVERDRIVE, 8, 7, 6, 5, 4, 3, Position::input};
    const auto data = serializeEffectSettings(settings);


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
}

TEST_F(MustangTest, setEffectClearsEffectIfEmptyEffect)
{
    const fx_pedal_settings settings{2, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
}

TEST_F(MustangTest, setEffectHandlesEffectPosition)
{
    const fx_pedal_settings settings{2, effects::OVERDRIVE, 1, 2, 3, 4, 5, 6, Position::effectsLoop};
    const auto data = serializeEffectSettings(settings);


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
}

TEST_F(MustangTest, setEffectHandlesEffectsWithMoreControls)
{
    const fx_pedal_settings settings{3, effects::STEREO_TAPE_DELAY, 1, 1, 1, 1, 1, 7, Position::input};
    const auto data = serializeEffectSettings(settings);


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, interruptWriteImpl(endpointSend, BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, interruptReceive(endpointReceive, packetSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
    ignoreClose();
}

TEST_F(MustangTest, saveEffectsSendsValues)
{
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::MONO_DELAY, 0, 1, 2, 3, 4, 5, Position::input},
                                                fx_pedal_settings{2, effects::SINE_FLANGER, 6, 7, 8, 0, 0, 0, Position::effectsLoop}};
    const std::string name = "abcd";
    const auto dataName = serializeSaveEffectName(slot, name, settings);
    const auto cmdExecute = serializeApplyCommand(settings[0]);
    const auto packets = serializeSaveEffectPacket(slot, settings);
    const auto dataEffect0 = packets[0];
    const auto dataEffect1 = packets[1];


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
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::MONO_DELAY, 0, 1, 2, 3, 4, 5, Position::input},
                                                fx_pedal_settings{2, effects::SINE_FLANGER, 6, 7, 8, 0, 0, 0, Position::effectsLoop},
                                                fx_pedal_settings{3, effects::SINE_FLANGER, 1, 2, 2, 1, 0, 4, Position::effectsLoop}};
    const std::string name = "abcd";
    const auto dataName = serializeSaveEffectName(slot, name, settings);
    const auto cmdExecute = serializeApplyCommand(settings[0]);
    const auto packets = serializeSaveEffectPacket(slot, settings);
    const auto dataEffect0 = packets[0];


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
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::SINE_CHORUS, 0, 1, 2, 3, 4, 5, Position::input}};
    const std::string name = "abcd";
    const auto dataName = serializeSaveEffectName(slot, name, settings);
    const auto cmdExecute = serializeApplyCommand(settings[0]);
    const auto packets = serializeSaveEffectPacket(slot, settings);
    const auto dataEffect0 = packets[0];


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
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::SINE_CHORUS, 0, 1, 2, 3, 4, 5, Position::input}};
    constexpr std::size_t nameSize{24};
    const std::string name(26, 'x');
    const std::string nameExpected{name.cbegin(), std::next(name.cbegin(), nameSize)};
    const auto cmdExecute = serializeApplyCommand(settings[0]);
    const auto dataName = serializeSaveEffectName(slot, name, settings);


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
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::TAPE_DELAY, 0, 1, 2, 3, 4, 5, Position::input}};
    const auto cmdExecute = serializeApplyCommand(settings[0]);
    const auto packets = serializeSaveEffectPacket(slot, settings);
    const auto dataEffect0 = packets[0];


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


    m->save_effects(slot, "abcd", settings);
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

