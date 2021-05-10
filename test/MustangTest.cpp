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

#include "com/Mustang.h"
#include "com/Packet.h"
#include "com/PacketSerializer.h"
#include "com/CommunicationException.h"
#include "mocks/MockConnection.h"
#include "matcher/Matcher.h"
#include "matcher/TypeMatcher.h"
#include <array>
#include <gmock/gmock.h>

using namespace plug;
using namespace plug::com;
using namespace test;
using namespace test::matcher;
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

    [[nodiscard]] std::vector<std::uint8_t> createEmptyPacketData() const
    {
        return std::vector<std::uint8_t>(packetRawTypeSize, 0x00);
    }

    template <class Container>
    [[nodiscard]] auto asBuffer(const Container& c) const
    {
        return std::vector<std::uint8_t>{std::cbegin(c), std::cend(c)};
    }


    std::shared_ptr<mock::MockConnection> conn;
    std::unique_ptr<com::Mustang> m;
    const std::vector<std::uint8_t> noData{};
    const std::vector<std::uint8_t> ignoreData = std::vector<std::uint8_t>(packetRawTypeSize);
    const std::vector<std::uint8_t> ignoreAmpData = [] { std::vector<std::uint8_t> d(packetRawTypeSize, 0x00); d[16] = 0x5e; return d; }();
    const PacketRawType loadCmd = serializeLoadCommand().getBytes();
    const PacketRawType applyCmd = serializeApplyCommand().getBytes();
    static inline constexpr std::size_t presetPacketCountShort{48};
    static inline constexpr std::size_t presetPacketCountFull{200};
    static inline constexpr int slot{5};
};

TEST_F(MustangTest, startInitializesDevice)
{
    const auto [initPacket1, initPacket2] = serializeInitCommand();
    const auto initCmd1 = initPacket1.getBytes();
    const auto initCmd2 = initPacket2.getBytes();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));

    // Init commands
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).Times(presetPacketCountShort).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    m->start_amp();
}

TEST_F(MustangTest, startThrowsIfConnectionNotReady)
{
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(false));
    EXPECT_THROW(m->start_amp(), plug::com::CommunicationException);
}

TEST_F(MustangTest, startRequestsCurrentPresetName)
{
    const auto [initPacket1, initPacket2] = serializeInitCommand();
    const auto initCmd1 = initPacket1.getBytes();
    const auto initCmd2 = initPacket2.getBytes();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));

    // Init commands
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).Times(presetPacketCountFull).WillRepeatedly(Return(ignoreData));

    const std::string actualName{"abc"};
    const auto nameData = asBuffer(serializeName(0, actualName).getBytes());

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(nameData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto [signalChain, presets] = m->start_amp();
    EXPECT_THAT(signalChain.name(), StrEq(actualName));

    static_cast<void>(presets);
}

TEST_F(MustangTest, startRequestsCurrentAmp)
{
    constexpr amp_settings amp{amps::BRITISH_60S, 4, 8, 5, 9, 1,
                               cabinets::cabBSSMN, 5, 3, 4, 7, 4, 2, 6, 1,
                               true, 17};
    const auto recvData = asBuffer(serializeAmpSettings(amp).getBytes());
    const auto extendedData = asBuffer(serializeAmpSettingsUsbGain(amp).getBytes());
    const auto [initPacket1, initPacket2] = serializeInitCommand();
    const auto initCmd1 = initPacket1.getBytes();
    const auto initCmd2 = initPacket2.getBytes();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));

    // Init commands
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).Times(presetPacketCountShort).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(extendedData))
        .WillOnce(Return(noData));


    const auto [signalChain, presets] = m->start_amp();
    EXPECT_THAT(signalChain.amp(), AmpIs(amp));

    static_cast<void>(presets);
}

TEST_F(MustangTest, startRequestsCurrentEffects)
{
    constexpr fx_pedal_settings e0{0x00, effects::TRIANGLE_FLANGER, 10, 20, 30, 40, 50, 0, Position::effectsLoop};
    constexpr fx_pedal_settings e1{0x01, effects::TRIANGLE_CHORUS, 0, 0, 0, 1, 1, 1, Position::input};
    constexpr fx_pedal_settings e2{0x02, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};
    constexpr fx_pedal_settings e3{0x03, effects::TAPE_DELAY, 1, 2, 3, 4, 5, 6, Position::effectsLoop};
    const auto recvData0 = asBuffer(serializeEffectSettings(e0).getBytes());
    const auto recvData1 = asBuffer(serializeEffectSettings(e1).getBytes());
    const auto recvData2 = asBuffer(serializeEffectSettings(e2).getBytes());
    const auto recvData3 = asBuffer(serializeEffectSettings(e3).getBytes());
    const auto [initPacket1, initPacket2] = serializeInitCommand();
    const auto initCmd1 = initPacket1.getBytes();
    const auto initCmd2 = initPacket2.getBytes();


    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));

    // Init commands
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).Times(presetPacketCountShort).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(recvData0))
        .WillOnce(Return(recvData1))
        .WillOnce(Return(recvData2))
        .WillOnce(Return(recvData3))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto [signalChain, presets] = m->start_amp();

    EXPECT_THAT(signalChain.effects()[0], EffectIs(e0));

    static_cast<void>(presets);
}

TEST_F(MustangTest, startRequestsAmpPresetList)
{
    const auto [initPacket1, initPacket2] = serializeInitCommand();
    const auto initCmd1 = initPacket1.getBytes();
    const auto initCmd2 = initPacket2.getBytes();
    const auto recvData0 = asBuffer(serializeName(0, "abc").getBytes());
    const auto recvData1 = asBuffer(serializeName(0, "def").getBytes());
    const auto recvData2 = asBuffer(serializeName(0, "ghi").getBytes());


    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));

    // Init commands
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(recvData0))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData1))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData2))
        .WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).Times(presetPacketCountShort - 6).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto [signalChain, presetList] = m->start_amp();

    EXPECT_THAT(presetList.size(), Eq(presetPacketCountShort / 2));
    EXPECT_THAT(presetList[0], StrEq("abc"));
    EXPECT_THAT(presetList[1], StrEq("def"));
    EXPECT_THAT(presetList[2], StrEq("ghi"));

    static_cast<void>(signalChain);
}

TEST_F(MustangTest, startUsesFullInitialTransmissionSizeIfOverThreshold)
{
    const auto [initPacket1, initPacket2] = serializeInitCommand();
    const auto initCmd1 = initPacket1.getBytes();
    const auto initCmd2 = initPacket2.getBytes();

    InSequence s;
    EXPECT_CALL(*conn, isOpen()).WillOnce(Return(true));

    // Init commands
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd1), initCmd1.size())).WillOnce(Return(initCmd1.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(initCmd2), initCmd2.size())).WillOnce(Return(initCmd2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadCmd), loadCmd.size())).WillOnce(Return(loadCmd.size()));

    // Preset names data
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).Times(presetPacketCountFull).WillRepeatedly(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    m->start_amp();
}

TEST_F(MustangTest, stopAmpClosesConnection)
{
    EXPECT_CALL(*conn, close());
    m->stop_amp();
}

TEST_F(MustangTest, loadMemoryBankSendsBankSelectionCommandAndReceivesPacket)
{
    const auto loadSlotCmd = serializeLoadSlotCommand(slot).getBytes();

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadSlotCmd), loadSlotCmd.size())).WillOnce(Return(loadSlotCmd.size()));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    m->load_memory_bank(slot);
}

TEST_F(MustangTest, loadMemoryBankReceivesName)
{
    const auto recvData = asBuffer(serializeName(0, "abc").getBytes());

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, sendImpl(_, _)).WillOnce(Return(packetRawTypeSize));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(recvData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));

    const auto signalChain = m->load_memory_bank(slot);
    EXPECT_THAT(signalChain.name(), StrEq("abc"));
}

TEST_F(MustangTest, loadMemoryBankReceivesAmpValues)
{

    constexpr amp_settings as{amps::BRITISH_80S, 2, 1, 3, 4, 5,
                              cabinets::cab4x12M, 0, 9, 10, 11,
                              0, 0x80, 13, 1, false, 0xab};

    const auto recvData = asBuffer(serializeAmpSettings(as).getBytes());
    const auto extendedData = asBuffer(serializeAmpSettingsUsbGain(as).getBytes());

    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, sendImpl(_, _)).WillOnce(Return(packetRawTypeSize));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(recvData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(extendedData))
        .WillOnce(Return(noData));


    const auto signalChain = m->load_memory_bank(slot);
    EXPECT_THAT(signalChain.amp(), AmpIs(as));
}

TEST_F(MustangTest, loadMemoryBankReceivesEffectValues)
{
    constexpr fx_pedal_settings e0{0x00, effects::TRIANGLE_FLANGER, 10, 20, 30, 40, 50, 0, Position::effectsLoop};
    constexpr fx_pedal_settings e1{0x01, effects::TRIANGLE_CHORUS, 0, 0, 0, 1, 1, 0, Position::input};
    constexpr fx_pedal_settings e2{0x02, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};
    constexpr fx_pedal_settings e3{0x03, effects::TAPE_DELAY, 1, 2, 3, 4, 5, 6, Position::effectsLoop};
    const auto recvData0 = asBuffer(serializeEffectSettings(e0).getBytes());
    const auto recvData1 = asBuffer(serializeEffectSettings(e1).getBytes());
    const auto recvData2 = asBuffer(serializeEffectSettings(e2).getBytes());
    const auto recvData3 = asBuffer(serializeEffectSettings(e3).getBytes());


    InSequence s;
    // Load cmd
    EXPECT_CALL(*conn, sendImpl(_, _)).WillOnce(Return(packetRawTypeSize));

    // Data
    EXPECT_CALL(*conn, receive(packetRawTypeSize))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(ignoreAmpData))
        .WillOnce(Return(recvData0))
        .WillOnce(Return(recvData1))
        .WillOnce(Return(recvData2))
        .WillOnce(Return(recvData3))
        .WillOnce(Return(ignoreData))
        .WillOnce(Return(noData));


    const auto signalChain = m->load_memory_bank(slot);

    EXPECT_THAT(signalChain.effects(), ElementsAre(EffectIs(e0), EffectIs(e1), EffectIs(e2), EffectIs(e3)));
}

TEST_F(MustangTest, setAmpSendsValues)
{
    constexpr amp_settings settings{amps::BRITISH_70S, 8, 9, 1, 2, 3,
                                    cabinets::cab4x12G, 3, 5, 3, 2, 1,
                                    4, 1, 5, true, 4};

    const auto data = serializeAmpSettings(settings).getBytes();
    const auto data2 = serializeAmpSettingsUsbGain(settings).getBytes();


    InSequence s;
    // Data #1
    EXPECT_CALL(*conn, sendImpl(BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, sendImpl(BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Data #2
    EXPECT_CALL(*conn, sendImpl(BufferIs(data2), data2.size())).WillOnce(Return(data2.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, sendImpl(BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));


    m->set_amplifier(settings);
}

TEST_F(MustangTest, setEffectSendsValue)
{
    constexpr fx_pedal_settings settings{3, effects::OVERDRIVE, 8, 7, 6, 5, 4, 3, Position::input};
    const auto data = serializeEffectSettings(settings).getBytes();
    const PacketRawType clearEffect = serializeClearEffectSettings(settings).getBytes();

    InSequence s;

    // Clear effect command
    EXPECT_CALL(*conn, sendImpl(BufferIs(clearEffect), clearEffect.size())).WillOnce(Return(clearEffect.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, sendImpl(BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Data
    EXPECT_CALL(*conn, sendImpl(BufferIs(data), data.size())).WillOnce(Return(data.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, sendImpl(BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    m->set_effect(settings);
}

TEST_F(MustangTest, setEffectDoesNotSendValueIfDisabled)
{
    constexpr fx_pedal_settings settings{3, effects::OVERDRIVE, 8, 7, 6, 5, 4, 3, Position::input, false};
    const PacketRawType clearEffect = serializeClearEffectSettings(settings).getBytes();

    InSequence s;

    // Clear effect command
    EXPECT_CALL(*conn, sendImpl(BufferIs(clearEffect), clearEffect.size())).WillOnce(Return(clearEffect.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, sendImpl(BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    m->set_effect(settings);
}

TEST_F(MustangTest, setEffectClearsEffectIfEmptyEffect)
{
    constexpr fx_pedal_settings settings{2, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};
    const PacketRawType clearCmd = serializeClearEffectSettings(settings).getBytes();


    InSequence s;
    // Clear command
    EXPECT_CALL(*conn, sendImpl(BufferIs(clearCmd), clearCmd.size())).WillOnce(Return(clearCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));

    // Apply command
    EXPECT_CALL(*conn, sendImpl(BufferIs(applyCmd), applyCmd.size())).WillOnce(Return(applyCmd.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(ignoreData));


    m->set_effect(settings);
}

TEST_F(MustangTest, saveEffectsSendsValues)
{
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::MONO_DELAY, 0, 1, 2, 3, 4, 5, Position::input},
                                                  fx_pedal_settings{2, effects::SINE_FLANGER, 6, 7, 8, 0, 0, 0, Position::effectsLoop}};
    const std::string name = "abcd";
    const auto dataName = serializeSaveEffectName(slot, name, settings).getBytes();
    const auto cmdExecute = serializeApplyCommand(settings[0]).getBytes();
    const auto packets = serializeSaveEffectPacket(slot, settings);


    InSequence s;
    // Save effect name cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(dataName), dataName.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));

    // Effect #0
    const auto effect0 = packets[0].getBytes();
    EXPECT_CALL(*conn, sendImpl(BufferIs(effect0), effect0.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));

    // Effect #1
    const auto effect1 = packets[1].getBytes();
    EXPECT_CALL(*conn, sendImpl(BufferIs(effect1), effect1.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));


    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveEffectsLimitsNumberOfValues)
{
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::MONO_DELAY, 0, 1, 2, 3, 4, 5, Position::input},
                                                  fx_pedal_settings{2, effects::SINE_FLANGER, 6, 7, 8, 0, 0, 0, Position::effectsLoop},
                                                  fx_pedal_settings{3, effects::SINE_FLANGER, 1, 2, 2, 1, 0, 4, Position::effectsLoop}};
    const std::string name = "abcd";
    const auto dataName = serializeSaveEffectName(slot, name, settings).getBytes();
    const auto cmdExecute = serializeApplyCommand(settings[0]).getBytes();
    const auto packets = serializeSaveEffectPacket(slot, settings);


    InSequence s;
    // Save effect cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(dataName), dataName.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));

    // Effect #0
    const auto effect0 = packets[0].getBytes();
    EXPECT_CALL(*conn, sendImpl(BufferIs(effect0), effect0.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));

    // Apply cmd
    EXPECT_CALL(*conn, sendImpl(BufferIs(cmdExecute), cmdExecute.size())).WillOnce(Return(0));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));

    m->save_effects(slot, name, settings);
}

TEST_F(MustangTest, saveEffectsDoesNothingOnInvalidEffect)
{
    const std::vector<fx_pedal_settings> settings{fx_pedal_settings{1, effects::COMPRESSOR, 0, 1, 2, 3, 4, 5, Position::input}};

    EXPECT_THROW(m->save_effects(slot, "abcd", settings), std::invalid_argument);
}

TEST_F(MustangTest, saveOnAmp)
{
    const std::string name(30, 'x');
    const auto saveNamePacket = serializeName(slot, name).getBytes();
    const auto loadSlotCmd = serializeLoadSlotCommand(slot).getBytes();

    InSequence s;
    EXPECT_CALL(*conn, sendImpl(BufferIs(saveNamePacket), saveNamePacket.size())).WillOnce(Return(saveNamePacket.size()));
    EXPECT_CALL(*conn, receive(packetRawTypeSize)).WillOnce(Return(noData));
    EXPECT_CALL(*conn, sendImpl(BufferIs(loadSlotCmd), loadSlotCmd.size())).WillOnce(Return(0));

    m->save_on_amp(name, slot);
}
