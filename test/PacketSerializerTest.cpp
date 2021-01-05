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

#include "com/PacketSerializer.h"
#include "data_structs.h"
#include "matcher/PacketMatcher.h"
#include "helper/MustangConstants.h"
#include <gmock/gmock.h>

using namespace plug;
using namespace plug::com;
using namespace testing;
using namespace test::matcher;


class PacketSerializerTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    [[nodiscard]]
    std::array<PacketRawType, 7> filledPackage(std::uint8_t value) const
    {
        PacketRawType packet{};
        std::fill(packet.begin(), packet.end(), value);
        std::array<PacketRawType, 7> data{};
        std::fill(data.begin(), data.end(), packet);
        return data;
    };

    [[nodiscard]]
    Packet<AmpPayload> ampPackage(std::uint8_t ampId) const
    {
        AmpPayload payload{};
        payload.setModel(ampId);
        payload.setCabinet(static_cast<std::uint8_t>(cabinets::OFF));

        Packet<AmpPayload> packet{};
        packet.setPayload(payload);
        return packet;
    };

    [[nodiscard]]
    Packet<AmpPayload> cabinetPackage(std::uint8_t cabinetId) const
    {
        AmpPayload payload{};
        payload.setModel(0x67);
        payload.setCabinet(cabinetId);

        Packet<AmpPayload> packet{};
        packet.setPayload(payload);
        return packet;
    };

    [[nodiscard]]
    std::array<Packet<EffectPayload>, 4> effectPackage(std::uint8_t effectId) const
    {
        EffectPayload payload{};
        payload.setSlot(0x01);
        payload.setModel(effectId);

        Packet<EffectPayload> packet{};
        packet.setPayload(payload);
        return {{packet, emptyEffectPayload, emptyEffectPayload, emptyEffectPayload}};
    };

    [[nodiscard]]
    Packet<NamePayload> presetNameEmptyPacket() const
    {
        NamePayload payload{};
        Packet<NamePayload> packet{};
        packet.setPayload(payload);
        return packet;
    }

    [[nodiscard]]
    Packet<NamePayload> presetNamePacket(std::string_view name) const
    {
        NamePayload payload{};
        payload.setName(name);

        Packet<NamePayload> packet{};
        packet.setPayload(payload);
        return packet;
    }

    const Packet<EffectPayload> emptyEffectPayload{};
    const Packet<AmpPayload> emptyAmpPayload{};
};

TEST_F(PacketSerializerTest, serializeInitCommand)
{
    PacketRawType packet1{};
    packet1[1] = 0xc3;
    PacketRawType packet2{};
    packet2[0] = 0x1a;
    packet2[1] = 0x03;

    const auto packets = serializeInitCommand();
    EXPECT_THAT(packets, SizeIs(2));
    EXPECT_THAT(packets[0].getBytes(), ContainerEq(packet1));
    EXPECT_THAT(packets[1].getBytes(), ContainerEq(packet2));
}

TEST_F(PacketSerializerTest, serializeApplyCommand)
{
    PacketRawType expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;

    const auto packet = serializeApplyCommand();
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeApplyCommandWithFxKnob)
{
    constexpr std::uint8_t fxKnob{0x02};
    const fx_pedal_settings effect{fxKnob, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};
    PacketRawType expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;
    expected[v1::FXKNOB] = fxKnob;

    const auto packet = serializeApplyCommand(effect);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeLoadCommand)
{
    PacketRawType expected{};
    expected[0] = 0xff;
    expected[1] = 0xc1;

    const auto packet = serializeLoadCommand();
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeLoadSlotCommand)
{
    constexpr std::uint8_t slot{15};
    PacketRawType expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x01;
    expected[v1::SAVE_SLOT] = slot;
    expected[6] = 0x01;

    const auto packet = serializeLoadSlotCommand(slot);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsData)
{
    constexpr amp_settings settings{amps::METAL_2000, 11, 22, 33, 44, 55, cabinets::cab2x12C, 1, 2, 3, 4, 5, 6, 7, 8, true, 0};

    PacketRawType expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                            0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                            0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    expected[v1::DSP] = 0x05;
    expected[v1::GAIN] = 11;
    expected[v1::VOLUME] = 22;
    expected[v1::TREBLE] = 33;
    expected[v1::MIDDLE] = 44;
    expected[v1::BASS] = 55;
    expected[v1::CABINET] = 0x07;
    expected[v1::NOISE_GATE] = 1;
    expected[v1::MASTER_VOL] = 2;
    expected[v1::GAIN2] = 3;
    expected[v1::PRESENCE] = 4;
    expected[v1::THRESHOLD] = 0;
    expected[v1::DEPTH] = 0x80;
    expected[v1::BIAS] = 7;
    expected[v1::SAG] = 0x02;
    expected[v1::BRIGHTNESS] = 1;
    expected[v1::AMPLIFIER] = 0x6d;
    expected[44] = 0x08;
    expected[45] = 0x08;
    expected[46] = 0x08;
    expected[50] = 0x08;
    expected[54] = 0x75;

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsWithEmptyData)
{
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, false, 0};

    PacketRawType expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                            0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                            0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    expected[v1::DSP] = 0x05;
    expected[v1::GAIN] = 0;
    expected[v1::VOLUME] = 0;
    expected[v1::TREBLE] = 0;
    expected[v1::MIDDLE] = 0;
    expected[v1::BASS] = 0;
    expected[v1::CABINET] = 0x00;
    expected[v1::NOISE_GATE] = 0;
    expected[v1::MASTER_VOL] = 0;
    expected[v1::GAIN2] = 0;
    expected[v1::PRESENCE] = 0;
    expected[v1::THRESHOLD] = 0;
    expected[v1::DEPTH] = 0x80;
    expected[v1::BIAS] = 0;
    expected[v1::SAG] = 0;
    expected[v1::BRIGHTNESS] = 0;
    expected[v1::AMPLIFIER] = 0x61;
    expected[44] = 0x07;
    expected[45] = 0x07;
    expected[46] = 0x07;
    expected[50] = 0x07;
    expected[54] = 0x5e;

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsDspData)
{
    constexpr amp_settings settings{amps::METAL_2000, 123, 101, 93, 30, 61, cabinets::cab2x12C, 3, 10, 15, 40, 0, 0, 100, 1, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::DSP], Eq(0x05));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsAmpControllsData)
{
    constexpr amp_settings settings{amps::METAL_2000, 123, 101, 93, 30, 61, cabinets::cab2x12C, 3, 10, 15, 40, 0, 0, 100, 1, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::GAIN], Eq(123));
    EXPECT_THAT(packet[v1::GAIN2], Eq(15));
    EXPECT_THAT(packet[v1::VOLUME], Eq(101));
    EXPECT_THAT(packet[v1::TREBLE], Eq(93));
    EXPECT_THAT(packet[v1::MIDDLE], Eq(30));
    EXPECT_THAT(packet[v1::BASS], Eq(61));
    EXPECT_THAT(packet[v1::MASTER_VOL], Eq(10));
    EXPECT_THAT(packet[v1::PRESENCE], Eq(40));
    EXPECT_THAT(packet[v1::NOISE_GATE], Eq(3));
    EXPECT_THAT(packet[v1::BIAS], Eq(100));
    EXPECT_THAT(packet[v1::BRIGHTNESS], Eq(0));
    EXPECT_THAT(packet[v1::SAG], Eq(1));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsAmpData)
{
    auto serializeWithType = [](amps a) {
        return serializeAmpSettings(amp_settings{a, 0, 0, 0, 0, 0, cabinets::cab2x12C, 0, 0, 0, 0, 0, 0, 0, 0, false, 0}).getBytes();
    };

    EXPECT_THAT(serializeWithType(amps::FENDER_57_DELUXE), AmpDataIs(0x67, 0x80, 0x01, 0x53));
    EXPECT_THAT(serializeWithType(amps::FENDER_59_BASSMAN), AmpDataIs(0x64, 0x80, 0x02, 0x67));
    EXPECT_THAT(serializeWithType(amps::FENDER_57_CHAMP), AmpDataIs(0x7c, 0x80, 0x0c, 0x00));
    EXPECT_THAT(serializeWithType(amps::FENDER_65_DELUXE_REVERB), AmpDataIs(0x53, 0x00, 0x03, 0x6a));
    EXPECT_THAT(serializeWithType(amps::FENDER_65_PRINCETON), AmpDataIs(0x6a, 0x80, 0x04, 0x61));
    EXPECT_THAT(serializeWithType(amps::FENDER_65_TWIN_REVERB), AmpDataIs(0x75, 0x80, 0x05, 0x72));
    EXPECT_THAT(serializeWithType(amps::FENDER_SUPER_SONIC), AmpDataIs(0x72, 0x80, 0x06, 0x79));
    EXPECT_THAT(serializeWithType(amps::BRITISH_60S), AmpDataIs(0x61, 0x80, 0x07, 0x5e));
    EXPECT_THAT(serializeWithType(amps::BRITISH_70S), AmpDataIs(0x79, 0x80, 0x0b, 0x7c));
    EXPECT_THAT(serializeWithType(amps::BRITISH_80S), AmpDataIs(0x5e, 0x80, 0x09, 0x5d));
    EXPECT_THAT(serializeWithType(amps::AMERICAN_90S), AmpDataIs(0x5d, 0x80, 0x0a, 0x6d));
    EXPECT_THAT(serializeWithType(amps::METAL_2000), AmpDataIs(0x6d, 0x80, 0x08, 0x75));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsCabinetData)
{
    auto serializeWithType = [](cabinets c) {
        return serializeAmpSettings(amp_settings{amps::BRITISH_70S, 0, 0, 0, 0, 0, c, 0, 0, 0, 0, 0, 0, 0, 0, false, 0}).getBytes();
    };

    EXPECT_THAT(serializeWithType(cabinets::OFF), CabinetDataIs(0x00));
    EXPECT_THAT(serializeWithType(cabinets::cab57DLX), CabinetDataIs(0x01));
    EXPECT_THAT(serializeWithType(cabinets::cabBSSMN), CabinetDataIs(0x02));
    EXPECT_THAT(serializeWithType(cabinets::cab65DLX), CabinetDataIs(0x03));
    EXPECT_THAT(serializeWithType(cabinets::cab65PRN), CabinetDataIs(0x04));
    EXPECT_THAT(serializeWithType(cabinets::cabCHAMP), CabinetDataIs(0x05));
    EXPECT_THAT(serializeWithType(cabinets::cab4x12M), CabinetDataIs(0x06));
    EXPECT_THAT(serializeWithType(cabinets::cab2x12C), CabinetDataIs(0x07));
    EXPECT_THAT(serializeWithType(cabinets::cab4x12G), CabinetDataIs(0x08));
    EXPECT_THAT(serializeWithType(cabinets::cab65TWN), CabinetDataIs(0x09));
    EXPECT_THAT(serializeWithType(cabinets::cab4x12V), CabinetDataIs(0x0a));
    EXPECT_THAT(serializeWithType(cabinets::cabSS212), CabinetDataIs(0x0b));
    EXPECT_THAT(serializeWithType(cabinets::cabSS112), CabinetDataIs(0x0c));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsLimitSagData)
{
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0x03, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::SAG], Eq(0x02));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsBrightnessData)
{
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, true, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::BRIGHTNESS], Eq(0x01));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsNoiseGate)
{
    constexpr std::uint8_t value{0x04};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, value, 0, 0, 0, 0, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::NOISE_GATE], Eq(value));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsLimitsNoiseGate)
{
    constexpr std::uint8_t value{0x06};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, value, 0, 0, 0, 0, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::NOISE_GATE], Eq(0x05));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsThresholdAndDepthIfNoiseGateFull)
{
    constexpr std::uint8_t noiseGate{0x05};
    constexpr std::uint8_t threshold{0x08};
    constexpr std::uint8_t depth{0x19};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, depth, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::THRESHOLD], Eq(threshold));
    EXPECT_THAT(packet[v1::DEPTH], Eq(depth));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsDoesNotSetThresholdAndDepthIfNoiseGateIsNotFull)
{
    constexpr std::uint8_t noiseGate{0x04};
    constexpr std::uint8_t threshold{0x08};
    constexpr std::uint8_t depth{0x19};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, depth, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::THRESHOLD], Eq(0x00));
    EXPECT_THAT(packet[v1::DEPTH], Eq(0x80));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsLimitsThreshold)
{
    constexpr std::uint8_t noiseGate{0x05};
    constexpr std::uint8_t threshold{0x0a};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::THRESHOLD], Eq(0x09));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsUsbGain)
{
    constexpr std::size_t value{101};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, false, value};

    PacketRawType expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;
    expected[2] = 0x0d;
    expected[6] = 0x01;
    expected[7] = 0x01;
    expected[v1::USB_GAIN] = value;

    const auto packet = serializeAmpSettingsUsbGain(settings);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeClearEffectsSettingsData)
{
    constexpr fx_pedal_settings settings{10, effects::VINTAGE_TREMOLO, 11, 22, 33, 44, 55, 66, Position::input};
    const PacketRawType expected{{0x1c, 0x03, 0x07, 0x00, 0x00, 0x00, 0x01, 0x01,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    const auto packet = serializeClearEffectSettings(settings);
    EXPECT_THAT(packet.getBytes()[2], Eq(0x07));
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeNameData)
{
    const std::string name{"name 123"};
    constexpr std::size_t slot{3};

    PacketRawType expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x03;
    expected[v1::SAVE_SLOT] = slot;
    expected[6] = 0x01;
    expected[7] = 0x01;
    std::copy(name.cbegin(), name.cend(), std::next(expected.begin(), v1::NAME));

    const auto packet = serializeName(slot, name).getBytes();
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeNameTerminatesWithZero)
{
    const std::string name{"abc"};
    constexpr std::size_t slot{3};

    const auto packet = serializeName(slot, name).getBytes();
    EXPECT_THAT(packet[name.size()], Eq('\0'));
}

TEST_F(PacketSerializerTest, serializeNameLimitsToLength)
{
    constexpr std::size_t maxSize{32};
    const std::string name(maxSize + 3, 'x');
    constexpr std::size_t slot{3};

    PacketRawType expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x03;
    expected[v1::SAVE_SLOT] = slot;
    expected[6] = 0x01;
    expected[7] = 0x01;
    std::copy(name.cbegin(), std::next(name.cbegin(), maxSize), std::next(expected.begin(), v1::NAME));

    const auto packet = serializeName(slot, name).getBytes();
    EXPECT_THAT(packet[v1::NAME + maxSize], Eq('\0'));
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsData)
{
    constexpr fx_pedal_settings settings{10, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::input};

    PacketRawType expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    expected[v1::FXSLOT] = 10;
    expected[v1::KNOB1] = 11;
    expected[v1::KNOB2] = 22;
    expected[v1::KNOB3] = 33;
    expected[v1::KNOB4] = 44;
    expected[v1::KNOB5] = 55;
    expected[v1::KNOB6] = 0x00;
    expected[v1::DSP] = 0x06;
    expected[v1::EFFECT] = 0x3c;

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSetsInputPosition)
{
    constexpr std::uint8_t value{45};
    constexpr fx_pedal_settings settings{value, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::input};

    const auto packet = serializeEffectSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::FXSLOT], Eq(value));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsEffectsSetsLoopPosition)
{
    constexpr std::uint8_t value{60};
    constexpr fx_pedal_settings settings{value, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    const auto packet = serializeEffectSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::FXSLOT], Eq(value + 4));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsDoesNotSetAdditionalKnobIfNotRequired)
{
    constexpr fx_pedal_settings settings{10, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    const auto packet = serializeEffectSettings(settings).getBytes();
    EXPECT_THAT(packet[v1::KNOB6], Eq(0x00));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSetSAdditionalKnobIfRequired)
{
    auto serializeWithType = [](effects e, std::uint8_t knob6) {
        return serializeEffectSettings(fx_pedal_settings{100, e, 0, 0, 0, 0, 0, knob6, Position::input}).getBytes();
    };

    EXPECT_THAT(serializeWithType(effects::MONO_ECHO_FILTER, 1)[v1::KNOB6], Eq(1));
    EXPECT_THAT(serializeWithType(effects::STEREO_ECHO_FILTER, 2)[v1::KNOB6], Eq(2));
    EXPECT_THAT(serializeWithType(effects::TAPE_DELAY, 3)[v1::KNOB6], Eq(3));
    EXPECT_THAT(serializeWithType(effects::STEREO_TAPE_DELAY, 4)[v1::KNOB6], Eq(4));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsDspAndEffectIdData)
{
    constexpr std::uint8_t dsp0{0x06};
    constexpr std::uint8_t dsp1{0x07};
    constexpr std::uint8_t dsp2{0x08};
    constexpr std::uint8_t dsp3{0x09};

    auto serializeWithType = [](effects e) {
        return serializeEffectSettings(fx_pedal_settings{100, e, 1, 2, 3, 4, 5, 6, Position::input}).getBytes();
    };

    EXPECT_THAT(serializeWithType(effects::OVERDRIVE), EffectDataIs(dsp0, 0x3c, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::WAH), EffectDataIs(dsp0, 0x49, 0x01, 0x08));
    EXPECT_THAT(serializeWithType(effects::TOUCH_WAH), EffectDataIs(dsp0, 0x4a, 0x01, 0x08));
    EXPECT_THAT(serializeWithType(effects::FUZZ), EffectDataIs(dsp0, 0x1a, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::FUZZ_TOUCH_WAH), EffectDataIs(dsp0, 0x1c, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::SIMPLE_COMP), EffectDataIs(dsp0, 0x88, 0x08, 0x08));
    EXPECT_THAT(serializeWithType(effects::COMPRESSOR), EffectDataIs(dsp0, 0x07, 0x00, 0x08));

    EXPECT_THAT(serializeWithType(effects::SINE_CHORUS), EffectDataIs(dsp1, 0x12, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::TRIANGLE_CHORUS), EffectDataIs(dsp1, 0x13, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::SINE_FLANGER), EffectDataIs(dsp1, 0x18, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::TRIANGLE_FLANGER), EffectDataIs(dsp1, 0x19, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::VIBRATONE), EffectDataIs(dsp1, 0x2d, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::VINTAGE_TREMOLO), EffectDataIs(dsp1, 0x40, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::SINE_TREMOLO), EffectDataIs(dsp1, 0x41, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::RING_MODULATOR), EffectDataIs(dsp1, 0x22, 0x01, 0x08));
    EXPECT_THAT(serializeWithType(effects::STEP_FILTER), EffectDataIs(dsp1, 0x29, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::PHASER), EffectDataIs(dsp1, 0x4f, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::PITCH_SHIFTER), EffectDataIs(dsp1, 0x1f, 0x01, 0x08));

    EXPECT_THAT(serializeWithType(effects::MONO_DELAY), EffectDataIs(dsp2, 0x16, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::MONO_ECHO_FILTER), EffectDataIs(dsp2, 0x43, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::STEREO_ECHO_FILTER), EffectDataIs(dsp2, 0x48, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::MULTITAP_DELAY), EffectDataIs(dsp2, 0x44, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::PING_PONG_DELAY), EffectDataIs(dsp2, 0x45, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::DUCKING_DELAY), EffectDataIs(dsp2, 0x15, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::REVERSE_DELAY), EffectDataIs(dsp2, 0x46, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::TAPE_DELAY), EffectDataIs(dsp2, 0x2b, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::STEREO_TAPE_DELAY), EffectDataIs(dsp2, 0x2a, 0x02, 0x01));

    EXPECT_THAT(serializeWithType(effects::SMALL_HALL_REVERB), EffectDataIs(dsp3, 0x24, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::LARGE_HALL_REVERB), EffectDataIs(dsp3, 0x3a, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::SMALL_ROOM_REVERB), EffectDataIs(dsp3, 0x26, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::LARGE_ROOM_REVERB), EffectDataIs(dsp3, 0x3b, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::SMALL_PLATE_REVERB), EffectDataIs(dsp3, 0x4e, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::LARGE_PLATE_REVERB), EffectDataIs(dsp3, 0x4b, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::AMBIENT_REVERB), EffectDataIs(dsp3, 0x4c, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::ARENA_REVERB), EffectDataIs(dsp3, 0x4d, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::FENDER_63_SPRING_REVERB), EffectDataIs(dsp3, 0x21, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::FENDER_65_SPRING_REVERB), EffectDataIs(dsp3, 0x0b, 0x00, 0x08));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSimpleCompKnobSetting)
{
    constexpr fx_pedal_settings settings{10, effects::SIMPLE_COMP, 3, 2, 3, 4, 5, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), KnobsAre(3, 0, 0, 0, 0, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSimpleCompLimitsValue)
{
    constexpr fx_pedal_settings settings{10, effects::SIMPLE_COMP, 4, 2, 3, 4, 5, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), KnobsAre(3, 0, 0, 0, 0, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsRingModulatorLimitsValue)
{
    constexpr fx_pedal_settings settings{10, effects::RING_MODULATOR, 1, 2, 3, 2, 5, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), KnobsAre(1, 2, 3, 1, 5, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsPhaserLimitsValue)
{
    constexpr fx_pedal_settings settings{10, effects::PHASER, 1, 2, 3, 2, 2, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), KnobsAre(1, 2, 3, 2, 1, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsMultitapDelayLimitsValue)
{
    constexpr fx_pedal_settings settings{10, effects::MULTITAP_DELAY, 1, 2, 3, 2, 4, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), KnobsAre(1, 2, 3, 2, 3, 0));
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameData)
{
    constexpr std::uint8_t slot{17};
    const std::string name{"name 17"};
    constexpr fx_pedal_settings effect{slot, effects::SINE_CHORUS, 1, 2, 3, 4, 5, 6, Position::input};

    PacketRawType expected{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    expected[v1::FXKNOB] = 0x01;
    expected[v1::SAVE_SLOT] = slot;
    std::copy(name.cbegin(), name.cend(), std::next(expected.begin(), v1::NAME));

    const auto packet = serializeSaveEffectName(slot, name, {effect});
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameFxKnobData)
{
    auto serializeWithType = [](effects e) {
        return serializeSaveEffectName(8, "ignore", {fx_pedal_settings{0, e, 0, 0, 0, 0, 0, 0, Position::input}}).getBytes();
    };

    EXPECT_THAT(serializeWithType(effects::SINE_CHORUS), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::TRIANGLE_CHORUS), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::SINE_FLANGER), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::TRIANGLE_FLANGER), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::VIBRATONE), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::VINTAGE_TREMOLO), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::SINE_TREMOLO), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::RING_MODULATOR), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::STEP_FILTER), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::PHASER), FxKnobIs(0x01));
    EXPECT_THAT(serializeWithType(effects::PITCH_SHIFTER), FxKnobIs(0x01));

    EXPECT_THAT(serializeWithType(effects::MONO_DELAY), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::MONO_ECHO_FILTER), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::STEREO_ECHO_FILTER), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::MULTITAP_DELAY), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::PING_PONG_DELAY), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::DUCKING_DELAY), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::REVERSE_DELAY), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::TAPE_DELAY), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::STEREO_TAPE_DELAY), FxKnobIs(0x02));

    EXPECT_THAT(serializeWithType(effects::SMALL_HALL_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::LARGE_HALL_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::SMALL_ROOM_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::LARGE_ROOM_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::SMALL_PLATE_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::LARGE_PLATE_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::AMBIENT_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::ARENA_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::FENDER_63_SPRING_REVERB), FxKnobIs(0x02));
    EXPECT_THAT(serializeWithType(effects::FENDER_65_SPRING_REVERB), FxKnobIs(0x02));
}


TEST_F(PacketSerializerTest, serializeSaveEffectNameThrowsOnInvalidEffect)
{
    auto serializeInvalid = [](effects e) {
        return serializeSaveEffectName(8, "ignore", {fx_pedal_settings{0, e, 0, 0, 0, 0, 0, 0, Position::input}}).getBytes();
    };

    EXPECT_THROW(serializeInvalid(effects::OVERDRIVE), std::invalid_argument);
    EXPECT_THROW(serializeInvalid(effects::WAH), std::invalid_argument);
    EXPECT_THROW(serializeInvalid(effects::TOUCH_WAH), std::invalid_argument);
    EXPECT_THROW(serializeInvalid(effects::FUZZ), std::invalid_argument);
    EXPECT_THROW(serializeInvalid(effects::FUZZ_TOUCH_WAH), std::invalid_argument);
    EXPECT_THROW(serializeInvalid(effects::SIMPLE_COMP), std::invalid_argument);
    EXPECT_THROW(serializeInvalid(effects::COMPRESSOR), std::invalid_argument);
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameSetsFxKnobOfFirstEffect)
{
    constexpr std::uint8_t slot{8};
    const std::string name{"ignore"};

    constexpr auto create = [](effects e) {
        return fx_pedal_settings{0, e, 0, 0, 0, 0, 0, 0, Position::input};
    };

    EXPECT_THAT(serializeSaveEffectName(slot, name, {create(effects::ARENA_REVERB), create(effects::SINE_CHORUS)}).getBytes(), FxKnobIs(0x02));
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameLimitsNameLength)
{
    constexpr std::uint8_t slot{17};
    constexpr std::size_t nameLength{24};
    std::string name(nameLength + 5, 'x');
    constexpr fx_pedal_settings effect{slot, effects::SINE_CHORUS, 1, 2, 3, 4, 5, 6, Position::input};

    PacketRawType expected{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    expected[v1::FXKNOB] = 0x01;
    expected[v1::SAVE_SLOT] = slot;
    std::copy(name.cbegin(), std::next(name.cbegin(), nameLength), std::next(expected.begin(), v1::NAME));

    const auto packet = serializeSaveEffectName(slot, name, {effect});
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameTerminatesName)
{
    constexpr std::uint8_t slot{17};
    constexpr std::size_t nameLength{24};
    std::string name(nameLength + 5, 'x');
    constexpr fx_pedal_settings effect{slot, effects::SINE_CHORUS, 1, 2, 3, 4, 5, 6, Position::input};

    PacketRawType expected{};
    std::copy(name.cbegin(), std::next(name.cbegin(), nameLength), std::next(expected.begin(), v1::NAME));

    const auto packet = serializeSaveEffectName(0, name, {effect}).getBytes();
    EXPECT_THAT(packet[v1::NAME + 24], Eq('\0'));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketData)
{
    constexpr std::uint8_t slot{8};
    constexpr fx_pedal_settings effect{slot, effects::SINE_CHORUS, 1, 2, 3, 4, 5, 6, Position::input};

    PacketRawType expected = serializeEffectSettings(effect).getBytes();
    expected[v1::FXKNOB] = 0x01;
    expected[v1::SAVE_SLOT] = slot;
    expected[1] = 0x03;
    expected[6] = 0x00;

    const auto packet = serializeSaveEffectPacket(slot, {effect});
    EXPECT_THAT(packet, SizeIs(1));
    EXPECT_THAT(packet[0].getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketThrowsOnInvalidEffect)
{
    constexpr std::uint8_t slot{8};
    constexpr fx_pedal_settings effect{slot, effects::COMPRESSOR, 1, 2, 3, 4, 5, 6, Position::input};

    EXPECT_THROW(serializeSaveEffectPacket(slot, {effect}), std::invalid_argument);
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketSerializesListOfTwoEffects)
{
    constexpr std::uint8_t slot{9};
    constexpr fx_pedal_settings effect1{slot, effects::TAPE_DELAY, 1, 2, 3, 4, 5, 6, Position::input};
    constexpr fx_pedal_settings effect2{slot, effects::MONO_DELAY, 1, 2, 3, 4, 5, 6, Position::input};

    const auto packet = serializeSaveEffectPacket(slot, {effect1, effect2});
    EXPECT_THAT(packet, SizeIs(2));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketSetsEffectValues)
{
    auto serializeWithType = [](effects e) {
        constexpr std::uint8_t slot{9};
        return serializeSaveEffectPacket(slot, {fx_pedal_settings{slot, e, 1, 2, 3, 4, 5, 6, Position::input}})[0].getBytes();
    };

    EXPECT_THAT(serializeWithType(effects::SINE_CHORUS), EffectDataIs(0x07, 0x12, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::TRIANGLE_CHORUS), EffectDataIs(0x07, 0x13, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::SINE_FLANGER), EffectDataIs(0x07, 0x18, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::TRIANGLE_FLANGER), EffectDataIs(0x07, 0x19, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::VIBRATONE), EffectDataIs(0x07, 0x2d, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::VINTAGE_TREMOLO), EffectDataIs(0x07, 0x40, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::SINE_TREMOLO), EffectDataIs(0x07, 0x41, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::RING_MODULATOR), EffectDataIs(0x07, 0x22, 0x01, 0x08));
    EXPECT_THAT(serializeWithType(effects::STEP_FILTER), EffectDataIs(0x07, 0x29, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::PHASER), EffectDataIs(0x07, 0x4f, 0x01, 0x01));
    EXPECT_THAT(serializeWithType(effects::PITCH_SHIFTER), EffectDataIs(0x07, 0x1f, 0x01, 0x08));

    EXPECT_THAT(serializeWithType(effects::MONO_DELAY), EffectDataIs(0x08, 0x16, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::MONO_ECHO_FILTER), EffectDataIs(0x08, 0x43, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::STEREO_ECHO_FILTER), EffectDataIs(0x08, 0x48, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::MULTITAP_DELAY), EffectDataIs(0x08, 0x44, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::PING_PONG_DELAY), EffectDataIs(0x08, 0x45, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::DUCKING_DELAY), EffectDataIs(0x08, 0x15, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::REVERSE_DELAY), EffectDataIs(0x08, 0x46, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::TAPE_DELAY), EffectDataIs(0x08, 0x2b, 0x02, 0x01));
    EXPECT_THAT(serializeWithType(effects::STEREO_TAPE_DELAY), EffectDataIs(0x08, 0x2a, 0x02, 0x01));

    EXPECT_THAT(serializeWithType(effects::SMALL_HALL_REVERB), EffectDataIs(0x09, 0x24, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::LARGE_HALL_REVERB), EffectDataIs(0x09, 0x3a, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::SMALL_ROOM_REVERB), EffectDataIs(0x09, 0x26, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::LARGE_ROOM_REVERB), EffectDataIs(0x09, 0x3b, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::SMALL_PLATE_REVERB), EffectDataIs(0x09, 0x4e, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::LARGE_PLATE_REVERB), EffectDataIs(0x09, 0x4b, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::AMBIENT_REVERB), EffectDataIs(0x09, 0x4c, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::ARENA_REVERB), EffectDataIs(0x09, 0x4d, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::FENDER_63_SPRING_REVERB), EffectDataIs(0x09, 0x21, 0x00, 0x08));
    EXPECT_THAT(serializeWithType(effects::FENDER_65_SPRING_REVERB), EffectDataIs(0x09, 0x0b, 0x00, 0x08));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketSerializesListOfTwoEffectsData)
{
    constexpr std::uint8_t slot{9};
    constexpr fx_pedal_settings effect1{slot, effects::TAPE_DELAY, 1, 2, 3, 4, 5, 6, Position::input};
    constexpr fx_pedal_settings effect2{slot, effects::MONO_DELAY, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    PacketRawType expected1 = serializeEffectSettings(effect1).getBytes();
    expected1[v1::FXKNOB] = 0x02;
    expected1[v1::SAVE_SLOT] = slot;
    expected1[1] = 0x03;
    expected1[6] = 0x00;
    PacketRawType expected2 = serializeEffectSettings(effect2).getBytes();
    expected2[v1::FXKNOB] = 0x02;
    expected2[v1::SAVE_SLOT] = slot;
    expected2[1] = 0x03;
    expected2[6] = 0x00;

    const auto packet = serializeSaveEffectPacket(slot, {effect1, effect2});
    EXPECT_THAT(packet, SizeIs(2));
    EXPECT_THAT(packet[0].getBytes(), ContainerEq(expected1));
    EXPECT_THAT(packet[1].getBytes(), ContainerEq(expected2));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketLimitsInputEffects)
{
    constexpr std::uint8_t slot{9};
    constexpr fx_pedal_settings effect1{slot, effects::TAPE_DELAY, 1, 2, 3, 4, 5, 6, Position::input};
    constexpr fx_pedal_settings effect2{slot, effects::MONO_DELAY, 11, 22, 33, 44, 55, 66, Position::effectsLoop};
    constexpr fx_pedal_settings effect3{slot, effects::PING_PONG_DELAY, 0, 0, 0, 0, 0, 0, Position::effectsLoop};

    const auto packet = serializeSaveEffectPacket(slot, {effect1, effect2, effect3});
    EXPECT_THAT(packet, SizeIs(1));
}

TEST_F(PacketSerializerTest, decodePresetListFromData)
{
    const auto emptyData = presetNameEmptyPacket();
    const std::string name1{"abcdefg"};
    const std::string name2{"xyz"};
    const auto presetPacket1 = presetNamePacket(name1);
    const auto presetPacket2 = presetNamePacket(name2);

    std::vector<Packet<NamePayload>> data{presetPacket1, emptyData, presetPacket2, emptyData};

    const auto result = decodePresetListFromData(data);
    EXPECT_THAT(result, SizeIs(2));
    EXPECT_THAT(result[0], StrEq(name1));
    EXPECT_THAT(result[1], StrEq(name2));
}

TEST_F(PacketSerializerTest, decodePresetListLimitsToMaxReceiveSize)
{
    constexpr std::size_t threshold{143};
    const auto emptyData = presetNameEmptyPacket();
    const auto presetPacket = presetNamePacket("abc");
    const std::vector<Packet<NamePayload>> names((threshold / 2), presetPacket);
    std::vector<Packet<NamePayload>> dataReduced;

    std::for_each(names.cbegin(), names.cend(), [&dataReduced, &emptyData](const auto& n) {
        dataReduced.push_back(n);
        dataReduced.push_back(emptyData);
    });
    std::vector<Packet<NamePayload>> dataFull = dataReduced;
    dataFull.insert(dataFull.end(), dataReduced.cbegin(), dataReduced.cend());

    EXPECT_THAT(dataFull.size(), Eq(2 * dataReduced.size()));

    const auto resultReduced = decodePresetListFromData(dataReduced);
    EXPECT_THAT(resultReduced, SizeIs(24));

    const auto resultFull = decodePresetListFromData(dataFull);
    EXPECT_THAT(resultFull, SizeIs(100));
}

TEST_F(PacketSerializerTest, decodePresetListLimitsNameToLength)
{
    const auto emptyData = presetNameEmptyPacket();
    constexpr std::size_t limit{32};
    const std::string name(limit + 10, 'x');
    const auto presetPacket = presetNamePacket(name);
    std::vector<Packet<NamePayload>> data{presetPacket, emptyData};

    const auto result = decodePresetListFromData(data);
    EXPECT_THAT(result[0], SizeIs(limit));
    EXPECT_THAT(result[0], Not(Contains('\0')));
}

TEST_F(PacketSerializerTest, decodePresetListIsSafeToEmptyData)
{
    const auto result = decodePresetListFromData({});
    EXPECT_THAT(result, SizeIs(0));
}

TEST_F(PacketSerializerTest, decodeNameFromData)
{
    const std::string name{"test name"};
    auto data = filledPackage(0xff);
    const auto nameEnd = std::copy(name.cbegin(), name.cend(), std::next(data[0].begin(), 16));
    *nameEnd = '\0';

    const auto result = decodeNameFromData(fromRawData<NamePayload>(data[0]));
    EXPECT_THAT(result, Eq(name));
}

TEST_F(PacketSerializerTest, decodeNameFromDataLimitsToLength)
{
    constexpr std::size_t nameLength{32};
    const std::string name(nameLength + 3, 'z');
    auto data = filledPackage(0xff);
    std::copy(name.cbegin(), name.cend(), std::next(data[0].begin(), 16));

    const auto result = decodeNameFromData(fromRawData<NamePayload>(data[0]));
    EXPECT_THAT(result, Eq(name.substr(0, nameLength)));
    EXPECT_THAT(nameLength, Eq(result.size()));
}

TEST_F(PacketSerializerTest, decodeAmpFromData)
{
    auto data = filledPackage(0xff);
    data[1][v1::AMPLIFIER] = 0x5e;
    data[1][v1::GAIN] = 0xaa;
    data[1][v1::VOLUME] = 0x10;
    data[1][v1::TREBLE] = 0x20;
    data[1][v1::MIDDLE] = 0x30;
    data[1][v1::BASS] = 0x40;
    data[1][v1::CABINET] = static_cast<std::uint8_t>(cabinets::cab65DLX);
    data[1][v1::NOISE_GATE] = 0x02;
    data[1][v1::MASTER_VOL] = 0x04;
    data[1][v1::GAIN2] = 0x05;
    data[1][v1::PRESENCE] = 0x15;
    data[1][v1::THRESHOLD] = 0x16;
    data[1][v1::DEPTH] = 0x21;
    data[1][v1::BIAS] = 0x19;
    data[1][v1::SAG] = 0x09;
    data[1][v1::BRIGHTNESS] = 0x01;
    data[6][v1::USB_GAIN] = 0xe1;

    const auto result = decodeAmpFromData(fromRawData<AmpPayload>(data[1]), fromRawData<AmpPayload>(data[6]));
    EXPECT_THAT(result.amp_num, Eq(amps::BRITISH_80S));
    EXPECT_THAT(result.gain, Eq(0xaa));
    EXPECT_THAT(result.volume, Eq(0x10));
    EXPECT_THAT(result.treble, Eq(0x20));
    EXPECT_THAT(result.middle, Eq(0x30));
    EXPECT_THAT(result.bass, Eq(0x40));
    EXPECT_THAT(result.cabinet, Eq(cabinets::cab65DLX));
    EXPECT_THAT(result.noise_gate, Eq(0x02));
    EXPECT_THAT(result.master_vol, Eq(0x04));
    EXPECT_THAT(result.gain2, Eq(0x05));
    EXPECT_THAT(result.presence, Eq(0x15));
    EXPECT_THAT(result.threshold, Eq(0x16));
    EXPECT_THAT(result.depth, Eq(0x21));
    EXPECT_THAT(result.bias, Eq(0x19));
    EXPECT_THAT(result.sag, Eq(0x09));
    EXPECT_THAT(result.brightness, Eq(true));
    EXPECT_THAT(result.usb_gain, Eq(0xe1));
}

TEST_F(PacketSerializerTest, decodeAmpFromDataAmps)
{
    auto decodeWithId = [this](const auto id) {
        return decodeAmpFromData(ampPackage(id), emptyAmpPayload).amp_num;
    };

    EXPECT_THAT(decodeWithId(0x67), Eq(amps::FENDER_57_DELUXE));
    EXPECT_THAT(decodeWithId(0x64), Eq(amps::FENDER_59_BASSMAN));
    EXPECT_THAT(decodeWithId(0x7c), Eq(amps::FENDER_57_CHAMP));
    EXPECT_THAT(decodeWithId(0x53), Eq(amps::FENDER_65_DELUXE_REVERB));
    EXPECT_THAT(decodeWithId(0x6a), Eq(amps::FENDER_65_PRINCETON));
    EXPECT_THAT(decodeWithId(0x75), Eq(amps::FENDER_65_TWIN_REVERB));
    EXPECT_THAT(decodeWithId(0x72), Eq(amps::FENDER_SUPER_SONIC));
    EXPECT_THAT(decodeWithId(0x61), Eq(amps::BRITISH_60S));
    EXPECT_THAT(decodeWithId(0x79), Eq(amps::BRITISH_70S));
    EXPECT_THAT(decodeWithId(0x5e), Eq(amps::BRITISH_80S));
    EXPECT_THAT(decodeWithId(0x5d), Eq(amps::AMERICAN_90S));
    EXPECT_THAT(decodeWithId(0x6d), Eq(amps::METAL_2000));
}

TEST_F(PacketSerializerTest, decodeAmpFromDataThrowsOnInvalidAmpId)
{
    EXPECT_THROW(decodeAmpFromData(ampPackage(0xf0), emptyAmpPayload), std::invalid_argument);
}

TEST_F(PacketSerializerTest, decodeAmpFromDataCabinets)
{
    auto decodeWithId = [this](const auto id) {
        return decodeAmpFromData(cabinetPackage(id), emptyAmpPayload).cabinet;
    };
    EXPECT_THAT(decodeWithId(0x00), Eq(cabinets::OFF));
    EXPECT_THAT(decodeWithId(0x01), Eq(cabinets::cab57DLX));
    EXPECT_THAT(decodeWithId(0x02), Eq(cabinets::cabBSSMN));
    EXPECT_THAT(decodeWithId(0x03), Eq(cabinets::cab65DLX));
    EXPECT_THAT(decodeWithId(0x04), Eq(cabinets::cab65PRN));
    EXPECT_THAT(decodeWithId(0x05), Eq(cabinets::cabCHAMP));
    EXPECT_THAT(decodeWithId(0x06), Eq(cabinets::cab4x12M));
    EXPECT_THAT(decodeWithId(0x07), Eq(cabinets::cab2x12C));
    EXPECT_THAT(decodeWithId(0x08), Eq(cabinets::cab4x12G));
    EXPECT_THAT(decodeWithId(0x09), Eq(cabinets::cab65TWN));
    EXPECT_THAT(decodeWithId(0x0a), Eq(cabinets::cab4x12V));
    EXPECT_THAT(decodeWithId(0x0b), Eq(cabinets::cabSS212));
    EXPECT_THAT(decodeWithId(0x0c), Eq(cabinets::cabSS112));
}

TEST_F(PacketSerializerTest, decodeAmpFromDataThrowsOnInvalidCabinetId)
{
    EXPECT_THROW(decodeAmpFromData(cabinetPackage(0xe0), emptyAmpPayload), std::invalid_argument);
}

TEST_F(PacketSerializerTest, decodeEffectsFromDataSetsData)
{
    auto package = filledPackage(0x00);
    auto& data = package[2];
    data[v1::FXSLOT] = 0x01;
    data[v1::KNOB1] = 0x11;
    data[v1::KNOB2] = 0x22;
    data[v1::KNOB3] = 0x33;
    data[v1::KNOB4] = 0x44;
    data[v1::KNOB5] = 0x55;
    data[v1::KNOB6] = 0x66;
    data[v1::EFFECT] = 0x49;
    Packet<EffectPayload> payload{};
    payload.fromBytes(data);

    const auto result = decodeEffectsFromData({{payload, emptyEffectPayload, emptyEffectPayload, emptyEffectPayload}});
    EXPECT_THAT(result[1].fx_slot, Eq(0x01));
    EXPECT_THAT(result[1].knob1, Eq(0x11));
    EXPECT_THAT(result[1].knob2, Eq(0x22));
    EXPECT_THAT(result[1].knob3, Eq(0x33));
    EXPECT_THAT(result[1].knob4, Eq(0x44));
    EXPECT_THAT(result[1].knob5, Eq(0x55));
    EXPECT_THAT(result[1].knob6, Eq(0x66));
    EXPECT_THAT(result[1].position, Eq(Position::input));
    EXPECT_THAT(result[1].effect_num, Eq(effects::WAH));
}

TEST_F(PacketSerializerTest, decodeEffectsFromDataEffectsValues)
{
    auto decodeWithId = [this](const auto id) {
        return decodeEffectsFromData(effectPackage(id))[1].effect_num;
    };
    EXPECT_THAT(decodeWithId(0x00), Eq(effects::EMPTY));
    EXPECT_THAT(decodeWithId(0x3c), Eq(effects::OVERDRIVE));
    EXPECT_THAT(decodeWithId(0x49), Eq(effects::WAH));
    EXPECT_THAT(decodeWithId(0x4a), Eq(effects::TOUCH_WAH));
    EXPECT_THAT(decodeWithId(0x1a), Eq(effects::FUZZ));
    EXPECT_THAT(decodeWithId(0x1c), Eq(effects::FUZZ_TOUCH_WAH));
    EXPECT_THAT(decodeWithId(0x88), Eq(effects::SIMPLE_COMP));
    EXPECT_THAT(decodeWithId(0x07), Eq(effects::COMPRESSOR));
    EXPECT_THAT(decodeWithId(0x12), Eq(effects::SINE_CHORUS));
    EXPECT_THAT(decodeWithId(0x13), Eq(effects::TRIANGLE_CHORUS));
    EXPECT_THAT(decodeWithId(0x18), Eq(effects::SINE_FLANGER));
    EXPECT_THAT(decodeWithId(0x19), Eq(effects::TRIANGLE_FLANGER));
    EXPECT_THAT(decodeWithId(0x2d), Eq(effects::VIBRATONE));
    EXPECT_THAT(decodeWithId(0x40), Eq(effects::VINTAGE_TREMOLO));
    EXPECT_THAT(decodeWithId(0x41), Eq(effects::SINE_TREMOLO));
    EXPECT_THAT(decodeWithId(0x22), Eq(effects::RING_MODULATOR));
    EXPECT_THAT(decodeWithId(0x29), Eq(effects::STEP_FILTER));
    EXPECT_THAT(decodeWithId(0x4f), Eq(effects::PHASER));
    EXPECT_THAT(decodeWithId(0x1f), Eq(effects::PITCH_SHIFTER));
    EXPECT_THAT(decodeWithId(0x16), Eq(effects::MONO_DELAY));
    EXPECT_THAT(decodeWithId(0x43), Eq(effects::MONO_ECHO_FILTER));
    EXPECT_THAT(decodeWithId(0x48), Eq(effects::STEREO_ECHO_FILTER));
    EXPECT_THAT(decodeWithId(0x44), Eq(effects::MULTITAP_DELAY));
    EXPECT_THAT(decodeWithId(0x45), Eq(effects::PING_PONG_DELAY));
    EXPECT_THAT(decodeWithId(0x15), Eq(effects::DUCKING_DELAY));
    EXPECT_THAT(decodeWithId(0x46), Eq(effects::REVERSE_DELAY));
    EXPECT_THAT(decodeWithId(0x2b), Eq(effects::TAPE_DELAY));
    EXPECT_THAT(decodeWithId(0x2a), Eq(effects::STEREO_TAPE_DELAY));
    EXPECT_THAT(decodeWithId(0x24), Eq(effects::SMALL_HALL_REVERB));
    EXPECT_THAT(decodeWithId(0x3a), Eq(effects::LARGE_HALL_REVERB));
    EXPECT_THAT(decodeWithId(0x26), Eq(effects::SMALL_ROOM_REVERB));
    EXPECT_THAT(decodeWithId(0x3b), Eq(effects::LARGE_ROOM_REVERB));
    EXPECT_THAT(decodeWithId(0x4e), Eq(effects::SMALL_PLATE_REVERB));
    EXPECT_THAT(decodeWithId(0x4b), Eq(effects::LARGE_PLATE_REVERB));
    EXPECT_THAT(decodeWithId(0x4c), Eq(effects::AMBIENT_REVERB));
    EXPECT_THAT(decodeWithId(0x4d), Eq(effects::ARENA_REVERB));
    EXPECT_THAT(decodeWithId(0x21), Eq(effects::FENDER_63_SPRING_REVERB));
    EXPECT_THAT(decodeWithId(0x0b), Eq(effects::FENDER_65_SPRING_REVERB));
}

TEST_F(PacketSerializerTest, decodeEffectsFromDataSetsPositionInput)
{
    auto data = filledPackage(0x00);
    data[2][v1::FXSLOT] = 0x00;
    data[3][v1::FXSLOT] = 0x01;
    data[4][v1::FXSLOT] = 0x02;
    data[5][v1::FXSLOT] = 0x03;

    const auto result = decodeEffectsFromData({{fromRawData<EffectPayload>(data[2]), fromRawData<EffectPayload>(data[3]),
                                                fromRawData<EffectPayload>(data[4]), fromRawData<EffectPayload>(data[5])}});
    EXPECT_THAT(result[0].fx_slot, Eq(0));
    EXPECT_THAT(result[0].position, Eq(Position::input));
    EXPECT_THAT(result[1].fx_slot, Eq(1));
    EXPECT_THAT(result[1].position, Eq(Position::input));
    EXPECT_THAT(result[2].fx_slot, Eq(2));
    EXPECT_THAT(result[2].position, Eq(Position::input));
    EXPECT_THAT(result[3].fx_slot, Eq(3));
    EXPECT_THAT(result[3].position, Eq(Position::input));
}

TEST_F(PacketSerializerTest, decodeEffectsFromDataSetsPositionEffectsLoop)
{
    auto data = filledPackage(0x00);
    data[2][v1::FXSLOT] = 0x04;
    data[3][v1::FXSLOT] = 0x05;
    data[4][v1::FXSLOT] = 0x06;
    data[5][v1::FXSLOT] = 0x07;

    const auto result = decodeEffectsFromData({{fromRawData<EffectPayload>(data[2]), fromRawData<EffectPayload>(data[3]),
                                                fromRawData<EffectPayload>(data[4]), fromRawData<EffectPayload>(data[5])}});
    EXPECT_THAT(result[0].fx_slot, Eq(0));
    EXPECT_THAT(result[0].position, Eq(Position::effectsLoop));
    EXPECT_THAT(result[1].fx_slot, Eq(1));
    EXPECT_THAT(result[1].position, Eq(Position::effectsLoop));
    EXPECT_THAT(result[2].fx_slot, Eq(2));
    EXPECT_THAT(result[2].position, Eq(Position::effectsLoop));
    EXPECT_THAT(result[3].fx_slot, Eq(3));
    EXPECT_THAT(result[3].position, Eq(Position::effectsLoop));
}
