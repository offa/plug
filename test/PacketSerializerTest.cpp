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

#include "com/PacketSerializer.h"
#include "data_structs.h"
#include "matcher/PacketMatcher.h"
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

    std::array<Packet, 7> filledPackage(std::uint8_t value) const
    {
        Packet packet{};
        std::fill(packet.begin(), packet.end(), value);
        std::array<Packet, 7> data{};
        std::fill(data.begin(), data.end(), packet);
        return data;
    };

    v2::Packet<v2::AmpPayload> ampPackage(std::uint8_t ampId) const
    {
        v2::AmpPayload payload{};
        payload.setModel(ampId);
        payload.setCabinet(static_cast<std::uint8_t>(cabinets::OFF));

        v2::Packet<v2::AmpPayload> packet{};
        packet.setPayload(payload);
        return packet;
    };

    v2::Packet<v2::AmpPayload> cabinetPackage(std::uint8_t cabinetId) const
    {
        v2::AmpPayload payload{};
        payload.setModel(0x67);
        payload.setCabinet(cabinetId);

        v2::Packet<v2::AmpPayload> packet{};
        packet.setPayload(payload);
        return packet;
    };

    constexpr std::array<Packet, 7> effectPackage(std::uint8_t effectId) const
    {
        std::array<Packet, 7> package{};
        package[2][FXSLOT] = 0x01;
        package[2][EFFECT] = effectId;
        return package;
    };

    constexpr Packet presetNameEmptyPacket() const
    {
        Packet data{};
        data[0] = 0x1c;
        data[1] = 0x01;
        data[2] = 0x04;
        return data;
    }

    Packet presetNamePacket(std::string_view name) const
    {
        Packet data{};
        data[0] = 0x1c;
        data[1] = 0x01;
        data[2] = 0x04;
        data[4] = 0x01;

        std::copy(name.cbegin(), name.cend(), std::next(data.begin(), 16));
        return data;
    }

    const v2::Packet<v2::AmpPayload> emptyPayload{};
};

TEST_F(PacketSerializerTest, serializeInitCommand)
{
    Packet packet1{};
    packet1[1] = 0xc3;
    Packet packet2{};
    packet2[0] = 0x1a;
    packet2[1] = 0x03;

    const auto packets = serializeInitCommand();
    EXPECT_THAT(packets, SizeIs(2));
    EXPECT_THAT(packets[0].getBytes(), ContainerEq(packet1));
    EXPECT_THAT(packets[1].getBytes(), ContainerEq(packet2));
}

TEST_F(PacketSerializerTest, serializeApplyCommand)
{
    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;

    const auto packet = serializeApplyCommand();
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeApplyCommandWithFxKnob)
{
    constexpr std::uint8_t fxKnob{0x02};
    const fx_pedal_settings effect{fxKnob, effects::EMPTY, 0, 0, 0, 0, 0, 0, Position::input};
    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;
    expected[FXKNOB] = fxKnob;

    const auto packet = serializeApplyCommand(effect);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeLoadCommand)
{
    Packet expected{};
    expected[0] = 0xff;
    expected[1] = 0xc1;

    const auto packet = serializeLoadCommand();
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeLoadSlotCommand)
{
    constexpr std::uint8_t slot{15};
    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x01;
    expected[SAVE_SLOT] = slot;
    expected[6] = 0x01;

    const auto packet = serializeLoadSlotCommand(slot);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsData)
{
    constexpr amp_settings settings{amps::METAL_2000, 11, 22, 33, 44, 55, cabinets::cab2x12C, 1, 2, 3, 4, 5, 6, 7, 8, true, 0};

    Packet expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                     0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                     0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    expected[DSP] = 0x05;
    expected[GAIN] = 11;
    expected[VOLUME] = 22;
    expected[TREBLE] = 33;
    expected[MIDDLE] = 44;
    expected[BASS] = 55;
    expected[CABINET] = 0x07;
    expected[NOISE_GATE] = 1;
    expected[MASTER_VOL] = 2;
    expected[GAIN2] = 3;
    expected[PRESENCE] = 4;
    expected[THRESHOLD] = 0;
    expected[DEPTH] = 0x80;
    expected[BIAS] = 7;
    expected[SAG] = 0x02;
    expected[BRIGHTNESS] = 1;
    expected[AMPLIFIER] = 0x6d;
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

    Packet expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                     0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                     0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    expected[DSP] = 0x05;
    expected[GAIN] = 0;
    expected[VOLUME] = 0;
    expected[TREBLE] = 0;
    expected[MIDDLE] = 0;
    expected[BASS] = 0;
    expected[CABINET] = 0x00;
    expected[NOISE_GATE] = 0;
    expected[MASTER_VOL] = 0;
    expected[GAIN2] = 0;
    expected[PRESENCE] = 0;
    expected[THRESHOLD] = 0;
    expected[DEPTH] = 0x80;
    expected[BIAS] = 0;
    expected[SAG] = 0;
    expected[BRIGHTNESS] = 0;
    expected[AMPLIFIER] = 0x61;
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
    EXPECT_THAT(packet[DSP], Eq(0x05));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsAmpControllsData)
{
    constexpr amp_settings settings{amps::METAL_2000, 123, 101, 93, 30, 61, cabinets::cab2x12C, 3, 10, 15, 40, 0, 0, 100, 1, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[GAIN], Eq(123));
    EXPECT_THAT(packet[GAIN2], Eq(15));
    EXPECT_THAT(packet[VOLUME], Eq(101));
    EXPECT_THAT(packet[TREBLE], Eq(93));
    EXPECT_THAT(packet[MIDDLE], Eq(30));
    EXPECT_THAT(packet[BASS], Eq(61));
    EXPECT_THAT(packet[MASTER_VOL], Eq(10));
    EXPECT_THAT(packet[PRESENCE], Eq(40));
    EXPECT_THAT(packet[NOISE_GATE], Eq(3));
    EXPECT_THAT(packet[BIAS], Eq(100));
    EXPECT_THAT(packet[BRIGHTNESS], Eq(0));
    EXPECT_THAT(packet[SAG], Eq(1));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsAmpData)
{
    constexpr auto create = [](amps a) {
        return amp_settings{a, 0, 0, 0, 0, 0, cabinets::cab2x12C, 0, 0, 0, 0, 0, 0, 0, 0, false, 0};
    };

    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_57_DELUXE)).getBytes(), AmpDataIs(0x67, 0x80, 0x01, 0x53));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_59_BASSMAN)).getBytes(), AmpDataIs(0x64, 0x80, 0x02, 0x67));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_57_CHAMP)).getBytes(), AmpDataIs(0x7c, 0x80, 0x0c, 0x00));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_65_DELUXE_REVERB)).getBytes(), AmpDataIs(0x53, 0x00, 0x03, 0x6a));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_65_PRINCETON)).getBytes(), AmpDataIs(0x6a, 0x80, 0x04, 0x61));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_65_TWIN_REVERB)).getBytes(), AmpDataIs(0x75, 0x80, 0x05, 0x72));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_SUPER_SONIC)).getBytes(), AmpDataIs(0x72, 0x80, 0x06, 0x79));
    EXPECT_THAT(serializeAmpSettings(create(amps::BRITISH_60S)).getBytes(), AmpDataIs(0x61, 0x80, 0x07, 0x5e));
    EXPECT_THAT(serializeAmpSettings(create(amps::BRITISH_70S)).getBytes(), AmpDataIs(0x79, 0x80, 0x0b, 0x7c));
    EXPECT_THAT(serializeAmpSettings(create(amps::BRITISH_80S)).getBytes(), AmpDataIs(0x5e, 0x80, 0x09, 0x5d));
    EXPECT_THAT(serializeAmpSettings(create(amps::AMERICAN_90S)).getBytes(), AmpDataIs(0x5d, 0x80, 0x0a, 0x6d));
    EXPECT_THAT(serializeAmpSettings(create(amps::METAL_2000)).getBytes(), AmpDataIs(0x6d, 0x80, 0x08, 0x75));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsCabinetData)
{
    constexpr auto create = [](cabinets c) {
        return amp_settings{amps::BRITISH_70S, 0, 0, 0, 0, 0, c, 0, 0, 0, 0, 0, 0, 0, 0, false, 0};
    };

    EXPECT_THAT(serializeAmpSettings(create(cabinets::OFF)).getBytes(), CabinetDataIs(0x00));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab57DLX)).getBytes(), CabinetDataIs(0x01));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabBSSMN)).getBytes(), CabinetDataIs(0x02));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab65DLX)).getBytes(), CabinetDataIs(0x03));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab65PRN)).getBytes(), CabinetDataIs(0x04));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabCHAMP)).getBytes(), CabinetDataIs(0x05));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab4x12M)).getBytes(), CabinetDataIs(0x06));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab2x12C)).getBytes(), CabinetDataIs(0x07));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab4x12G)).getBytes(), CabinetDataIs(0x08));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab65TWN)).getBytes(), CabinetDataIs(0x09));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab4x12V)).getBytes(), CabinetDataIs(0x0a));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabSS212)).getBytes(), CabinetDataIs(0x0b));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabSS112)).getBytes(), CabinetDataIs(0x0c));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsLimitSagData)
{
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0x03, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[SAG], Eq(0x02));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsBrightnessData)
{
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, true, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[BRIGHTNESS], Eq(0x01));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsNoiseGate)
{
    constexpr std::uint8_t value{0x04};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, value, 0, 0, 0, 0, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[NOISE_GATE], Eq(value));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsLimitsNoiseGate)
{
    constexpr std::uint8_t value{0x06};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, value, 0, 0, 0, 0, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[NOISE_GATE], Eq(0x05));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsThresholdAndDepthIfNoiseGateFull)
{
    constexpr std::uint8_t noiseGate{0x05};
    constexpr std::uint8_t threshold{0x08};
    constexpr std::uint8_t depth{0x19};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, depth, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[THRESHOLD], Eq(threshold));
    EXPECT_THAT(packet[DEPTH], Eq(depth));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsDoesNotSetThresholdAndDepthIfNoiseGateIsNotFull)
{
    constexpr std::uint8_t noiseGate{0x04};
    constexpr std::uint8_t threshold{0x08};
    constexpr std::uint8_t depth{0x19};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, depth, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[THRESHOLD], Eq(0x00));
    EXPECT_THAT(packet[DEPTH], Eq(0x80));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsLimitsThreshold)
{
    constexpr std::uint8_t noiseGate{0x05};
    constexpr std::uint8_t threshold{0x0a};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings).getBytes();
    EXPECT_THAT(packet[THRESHOLD], Eq(0x09));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsUsbGain)
{
    constexpr std::size_t value{101};
    constexpr amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, false, value};

    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;
    expected[2] = 0x0d;
    expected[6] = 0x01;
    expected[7] = 0x01;
    expected[USB_GAIN] = value;

    const auto packet = serializeAmpSettingsUsbGain(settings);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeClearEffectsSettingsData)
{
    const Packet expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    const auto packet = serializeClearEffectSettings();
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeNameData)
{
    const std::string name{"name 123"};
    constexpr std::size_t slot{3};

    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x03;
    expected[SAVE_SLOT] = slot;
    expected[6] = 0x01;
    expected[7] = 0x01;
    std::copy(name.cbegin(), name.cend(), std::next(expected.begin(), NAME));

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

    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x03;
    expected[SAVE_SLOT] = slot;
    expected[6] = 0x01;
    expected[7] = 0x01;
    std::copy(name.cbegin(), std::next(name.cbegin(), maxSize), std::next(expected.begin(), NAME));

    const auto packet = serializeName(slot, name).getBytes();
    EXPECT_THAT(packet[NAME + maxSize], Eq('\0'));
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsData)
{
    constexpr fx_pedal_settings settings{10, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::input};

    Packet expected{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    expected[FXSLOT] = 10;
    expected[KNOB1] = 11;
    expected[KNOB2] = 22;
    expected[KNOB3] = 33;
    expected[KNOB4] = 44;
    expected[KNOB5] = 55;
    expected[KNOB6] = 0x00;
    expected[DSP] = 0x06;
    expected[EFFECT] = 0x3c;

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSetsInputPosition)
{
    constexpr std::uint8_t value{45};
    constexpr fx_pedal_settings settings{value, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::input};

    const auto packet = serializeEffectSettings(settings).getBytes();
    EXPECT_THAT(packet[FXSLOT], Eq(value));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsEffectsSetsLoopPosition)
{
    constexpr std::uint8_t value{60};
    constexpr fx_pedal_settings settings{value, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    const auto packet = serializeEffectSettings(settings).getBytes();
    EXPECT_THAT(packet[FXSLOT], Eq(value + 4));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsDoesNotSetAdditionalKnobIfNotRequired)
{
    constexpr fx_pedal_settings settings{10, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    const auto packet = serializeEffectSettings(settings).getBytes();
    EXPECT_THAT(packet[KNOB6], Eq(0x00));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSetSAdditionalKnobIfRequired)
{
    constexpr auto create = [](effects e, std::uint8_t knob6) {
        return fx_pedal_settings{100, e, 0, 0, 0, 0, 0, knob6, Position::input};
    };

    EXPECT_THAT(serializeEffectSettings(create(effects::MONO_ECHO_FILTER, 1)).getBytes()[KNOB6], Eq(1));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_ECHO_FILTER, 2)).getBytes()[KNOB6], Eq(2));
    EXPECT_THAT(serializeEffectSettings(create(effects::TAPE_DELAY, 3)).getBytes()[KNOB6], Eq(3));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_TAPE_DELAY, 4)).getBytes()[KNOB6], Eq(4));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsDspAndEffectIdData)
{
    constexpr std::uint8_t dsp0{0x06};
    constexpr std::uint8_t dsp1{0x07};
    constexpr std::uint8_t dsp2{0x08};
    constexpr std::uint8_t dsp3{0x09};

    constexpr auto create = [](effects e) {
        return fx_pedal_settings{100, e, 1, 2, 3, 4, 5, 6, Position::input};
    };

    EXPECT_THAT(serializeEffectSettings(create(effects::OVERDRIVE)).getBytes(), EffectDataIs(dsp0, 0x3c, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::WAH)).getBytes(), EffectDataIs(dsp0, 0x49, 0x01, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::TOUCH_WAH)).getBytes(), EffectDataIs(dsp0, 0x4a, 0x01, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FUZZ)).getBytes(), EffectDataIs(dsp0, 0x1a, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FUZZ_TOUCH_WAH)).getBytes(), EffectDataIs(dsp0, 0x1c, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::SIMPLE_COMP)).getBytes(), EffectDataIs(dsp0, 0x88, 0x08, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::COMPRESSOR)).getBytes(), EffectDataIs(dsp0, 0x07, 0x00, 0x08));

    EXPECT_THAT(serializeEffectSettings(create(effects::SINE_CHORUS)).getBytes(), EffectDataIs(dsp1, 0x12, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::TRIANGLE_CHORUS)).getBytes(), EffectDataIs(dsp1, 0x13, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::SINE_FLANGER)).getBytes(), EffectDataIs(dsp1, 0x18, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::TRIANGLE_FLANGER)).getBytes(), EffectDataIs(dsp1, 0x19, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::VIBRATONE)).getBytes(), EffectDataIs(dsp1, 0x2d, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::VINTAGE_TREMOLO)).getBytes(), EffectDataIs(dsp1, 0x40, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::SINE_TREMOLO)).getBytes(), EffectDataIs(dsp1, 0x41, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::RING_MODULATOR)).getBytes(), EffectDataIs(dsp1, 0x22, 0x01, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEP_FILTER)).getBytes(), EffectDataIs(dsp1, 0x29, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::PHASER)).getBytes(), EffectDataIs(dsp1, 0x4f, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::PITCH_SHIFTER)).getBytes(), EffectDataIs(dsp1, 0x1f, 0x01, 0x08));

    EXPECT_THAT(serializeEffectSettings(create(effects::MONO_DELAY)).getBytes(), EffectDataIs(dsp2, 0x16, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::MONO_ECHO_FILTER)).getBytes(), EffectDataIs(dsp2, 0x43, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_ECHO_FILTER)).getBytes(), EffectDataIs(dsp2, 0x48, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::MULTITAP_DELAY)).getBytes(), EffectDataIs(dsp2, 0x44, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::PING_PONG_DELAY)).getBytes(), EffectDataIs(dsp2, 0x45, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::DUCKING_DELAY)).getBytes(), EffectDataIs(dsp2, 0x15, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::REVERSE_DELAY)).getBytes(), EffectDataIs(dsp2, 0x46, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::TAPE_DELAY)).getBytes(), EffectDataIs(dsp2, 0x2b, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_TAPE_DELAY)).getBytes(), EffectDataIs(dsp2, 0x2a, 0x02, 0x01));

    EXPECT_THAT(serializeEffectSettings(create(effects::SMALL_HALL_REVERB)).getBytes(), EffectDataIs(dsp3, 0x24, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::LARGE_HALL_REVERB)).getBytes(), EffectDataIs(dsp3, 0x3a, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::SMALL_ROOM_REVERB)).getBytes(), EffectDataIs(dsp3, 0x26, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::LARGE_ROOM_REVERB)).getBytes(), EffectDataIs(dsp3, 0x3b, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::SMALL_PLATE_REVERB)).getBytes(), EffectDataIs(dsp3, 0x4e, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::LARGE_PLATE_REVERB)).getBytes(), EffectDataIs(dsp3, 0x4b, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::AMBIENT_REVERB)).getBytes(), EffectDataIs(dsp3, 0x4c, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::ARENA_REVERB)).getBytes(), EffectDataIs(dsp3, 0x4d, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FENDER_63_SPRING_REVERB)).getBytes(), EffectDataIs(dsp3, 0x21, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FENDER_65_SPRING_REVERB)).getBytes(), EffectDataIs(dsp3, 0x0b, 0x00, 0x08));
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

    Packet expected{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    expected[FXKNOB] = 0x01;
    expected[SAVE_SLOT] = slot;
    std::copy(name.cbegin(), name.cend(), std::next(expected.begin(), NAME));

    const auto packet = serializeSaveEffectName(slot, name, {effect});
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameFxKnobData)
{
    constexpr std::uint8_t slot{8};
    const std::string name{"ignore"};

    constexpr auto create = [](effects e) -> std::vector<fx_pedal_settings> {
        return {fx_pedal_settings{0, e, 0, 0, 0, 0, 0, 0, Position::input}};
    };

    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::SINE_CHORUS)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::TRIANGLE_CHORUS)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::SINE_FLANGER)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::TRIANGLE_FLANGER)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::VIBRATONE)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::VINTAGE_TREMOLO)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::SINE_TREMOLO)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::RING_MODULATOR)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::STEP_FILTER)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::PHASER)).getBytes(), FxKnobIs(0x01));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::PITCH_SHIFTER)).getBytes(), FxKnobIs(0x01));

    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::MONO_DELAY)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::MONO_ECHO_FILTER)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::STEREO_ECHO_FILTER)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::MULTITAP_DELAY)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::PING_PONG_DELAY)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::DUCKING_DELAY)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::REVERSE_DELAY)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::TAPE_DELAY)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::STEREO_TAPE_DELAY)).getBytes(), FxKnobIs(0x02));

    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::SMALL_HALL_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::LARGE_HALL_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::SMALL_ROOM_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::LARGE_ROOM_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::SMALL_PLATE_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::LARGE_PLATE_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::AMBIENT_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::ARENA_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::FENDER_63_SPRING_REVERB)).getBytes(), FxKnobIs(0x02));
    EXPECT_THAT(serializeSaveEffectName(slot, name, create(effects::FENDER_65_SPRING_REVERB)).getBytes(), FxKnobIs(0x02));
}


TEST_F(PacketSerializerTest, serializeSaveEffectNameThrowsOnInvalidEffect)
{
    constexpr std::uint8_t slot{8};
    const std::string name{"ignore"};

    constexpr auto create = [](effects e) -> std::vector<fx_pedal_settings> {
        return {fx_pedal_settings{0, e, 0, 0, 0, 0, 0, 0, Position::input}};
    };

    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::OVERDRIVE)).getBytes(), std::invalid_argument);
    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::WAH)).getBytes(), std::invalid_argument);
    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::TOUCH_WAH)).getBytes(), std::invalid_argument);
    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::FUZZ)).getBytes(), std::invalid_argument);
    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::FUZZ_TOUCH_WAH)).getBytes(), std::invalid_argument);
    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::SIMPLE_COMP)).getBytes(), std::invalid_argument);
    EXPECT_THROW(serializeSaveEffectName(slot, name, create(effects::COMPRESSOR)).getBytes(), std::invalid_argument);
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

    Packet expected{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    expected[FXKNOB] = 0x01;
    expected[SAVE_SLOT] = slot;
    std::copy(name.cbegin(), std::next(name.cbegin(), nameLength), std::next(expected.begin(), NAME));

    const auto packet = serializeSaveEffectName(slot, name, {effect});
    EXPECT_THAT(packet.getBytes(), ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeSaveEffectNameTerminatesName)
{
    constexpr std::uint8_t slot{17};
    constexpr std::size_t nameLength{24};
    std::string name(nameLength + 5, 'x');
    constexpr fx_pedal_settings effect{slot, effects::SINE_CHORUS, 1, 2, 3, 4, 5, 6, Position::input};

    Packet expected{};
    std::copy(name.cbegin(), std::next(name.cbegin(), nameLength), std::next(expected.begin(), NAME));

    const auto packet = serializeSaveEffectName(0, name, {effect}).getBytes();
    EXPECT_THAT(packet[NAME + 24], Eq('\0'));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketData)
{
    constexpr std::uint8_t slot{8};
    constexpr fx_pedal_settings effect{slot, effects::SINE_CHORUS, 1, 2, 3, 4, 5, 6, Position::input};

    Packet expected = serializeEffectSettings(effect).getBytes();
    expected[FXKNOB] = 0x01;
    expected[SAVE_SLOT] = slot;
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
    constexpr std::uint8_t slot{9};

    constexpr auto createEffect = [](effects e) {
        return fx_pedal_settings{slot, e, 1, 2, 3, 4, 5, 6, Position::input};
    };

    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::SINE_CHORUS)})[0].getBytes(), EffectDataIs(0x07, 0x12, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::TRIANGLE_CHORUS)})[0].getBytes(), EffectDataIs(0x07, 0x13, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::SINE_FLANGER)})[0].getBytes(), EffectDataIs(0x07, 0x18, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::TRIANGLE_FLANGER)})[0].getBytes(), EffectDataIs(0x07, 0x19, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::VIBRATONE)})[0].getBytes(), EffectDataIs(0x07, 0x2d, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::VINTAGE_TREMOLO)})[0].getBytes(), EffectDataIs(0x07, 0x40, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::SINE_TREMOLO)})[0].getBytes(), EffectDataIs(0x07, 0x41, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::RING_MODULATOR)})[0].getBytes(), EffectDataIs(0x07, 0x22, 0x01, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::STEP_FILTER)})[0].getBytes(), EffectDataIs(0x07, 0x29, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::PHASER)})[0].getBytes(), EffectDataIs(0x07, 0x4f, 0x01, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::PITCH_SHIFTER)})[0].getBytes(), EffectDataIs(0x07, 0x1f, 0x01, 0x08));

    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::MONO_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x16, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::MONO_ECHO_FILTER)})[0].getBytes(), EffectDataIs(0x08, 0x43, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::STEREO_ECHO_FILTER)})[0].getBytes(), EffectDataIs(0x08, 0x48, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::MULTITAP_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x44, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::PING_PONG_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x45, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::DUCKING_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x15, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::REVERSE_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x46, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::TAPE_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x2b, 0x02, 0x01));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::STEREO_TAPE_DELAY)})[0].getBytes(), EffectDataIs(0x08, 0x2a, 0x02, 0x01));

    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::SMALL_HALL_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x24, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::LARGE_HALL_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x3a, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::SMALL_ROOM_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x26, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::LARGE_ROOM_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x3b, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::SMALL_PLATE_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x4e, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::LARGE_PLATE_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x4b, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::AMBIENT_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x4c, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::ARENA_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x4d, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::FENDER_63_SPRING_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x21, 0x00, 0x08));
    EXPECT_THAT(serializeSaveEffectPacket(slot, {createEffect(effects::FENDER_65_SPRING_REVERB)})[0].getBytes(), EffectDataIs(0x09, 0x0b, 0x00, 0x08));
}

TEST_F(PacketSerializerTest, serializeSaveEffectPacketSerializesListOfTwoEffectsData)
{
    constexpr std::uint8_t slot{9};
    constexpr fx_pedal_settings effect1{slot, effects::TAPE_DELAY, 1, 2, 3, 4, 5, 6, Position::input};
    constexpr fx_pedal_settings effect2{slot, effects::MONO_DELAY, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    Packet expected1 = serializeEffectSettings(effect1).getBytes();
    expected1[FXKNOB] = 0x02;
    expected1[SAVE_SLOT] = slot;
    expected1[1] = 0x03;
    expected1[6] = 0x00;
    Packet expected2 = serializeEffectSettings(effect2).getBytes();
    expected2[FXKNOB] = 0x02;
    expected2[SAVE_SLOT] = slot;
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

    std::vector<Packet> data{presetPacket1, emptyData, presetPacket2, emptyData};

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
    const std::vector<Packet> names((threshold / 2), presetPacket);
    std::vector<Packet> dataReduced;

    std::for_each(names.cbegin(), names.cend(), [&dataReduced, &emptyData](const auto& n) {
        dataReduced.push_back(n);
        dataReduced.push_back(emptyData);
    });
    std::vector<Packet> dataFull = dataReduced;
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
    std::vector<Packet> data{presetPacket, emptyData};

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

    const auto result = decodeNameFromData(adapt<v2::NamePayload>(data[0]));
    EXPECT_THAT(result, Eq(name));
}

TEST_F(PacketSerializerTest, decodeNameFromDataLimitsToLength)
{
    constexpr std::size_t nameLength{32};
    const std::string name(nameLength + 3, 'z');
    auto data = filledPackage(0xff);
    std::copy(name.cbegin(), name.cend(), std::next(data[0].begin(), 16));

    const auto result = decodeNameFromData(adapt<v2::NamePayload>(data[0]));
    EXPECT_THAT(result, Eq(name.substr(0, nameLength)));
    EXPECT_THAT(nameLength, Eq(result.size()));
}

TEST_F(PacketSerializerTest, decodeAmpFromData)
{
    auto data = filledPackage(0xff);
    data[1][AMPLIFIER] = 0x5e;
    data[1][GAIN] = 0xaa;
    data[1][VOLUME] = 0x10;
    data[1][TREBLE] = 0x20;
    data[1][MIDDLE] = 0x30;
    data[1][BASS] = 0x40;
    data[1][CABINET] = static_cast<std::uint8_t>(cabinets::cab65DLX);
    data[1][NOISE_GATE] = 0x02;
    data[1][MASTER_VOL] = 0x04;
    data[1][GAIN2] = 0x05;
    data[1][PRESENCE] = 0x15;
    data[1][THRESHOLD] = 0x16;
    data[1][DEPTH] = 0x21;
    data[1][BIAS] = 0x19;
    data[1][SAG] = 0x09;
    data[1][BRIGHTNESS] = 0x01;
    data[6][USB_GAIN] = 0xe1;

    const auto result = decodeAmpFromData(adapt<v2::AmpPayload>(data[1]), adapt<v2::AmpPayload>(data[6]));
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
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x67), emptyPayload).amp_num, Eq(amps::FENDER_57_DELUXE));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x64), emptyPayload).amp_num, Eq(amps::FENDER_59_BASSMAN));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x7c), emptyPayload).amp_num, Eq(amps::FENDER_57_CHAMP));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x53), emptyPayload).amp_num, Eq(amps::FENDER_65_DELUXE_REVERB));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x6a), emptyPayload).amp_num, Eq(amps::FENDER_65_PRINCETON));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x75), emptyPayload).amp_num, Eq(amps::FENDER_65_TWIN_REVERB));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x72), emptyPayload).amp_num, Eq(amps::FENDER_SUPER_SONIC));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x61), emptyPayload).amp_num, Eq(amps::BRITISH_60S));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x79), emptyPayload).amp_num, Eq(amps::BRITISH_70S));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x5e), emptyPayload).amp_num, Eq(amps::BRITISH_80S));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x5d), emptyPayload).amp_num, Eq(amps::AMERICAN_90S));
    EXPECT_THAT(decodeAmpFromData(ampPackage(0x6d), emptyPayload).amp_num, Eq(amps::METAL_2000));
}

TEST_F(PacketSerializerTest, decodeAmpFromDataThrowsOnInvalidAmpId)
{
    EXPECT_THROW(decodeAmpFromData(ampPackage(0xf0), emptyPayload), std::invalid_argument);
}

TEST_F(PacketSerializerTest, decodeAmpFromDataCabinets)
{
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x00), emptyPayload).cabinet, Eq(cabinets::OFF));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x01), emptyPayload).cabinet, Eq(cabinets::cab57DLX));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x02), emptyPayload).cabinet, Eq(cabinets::cabBSSMN));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x03), emptyPayload).cabinet, Eq(cabinets::cab65DLX));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x04), emptyPayload).cabinet, Eq(cabinets::cab65PRN));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x05), emptyPayload).cabinet, Eq(cabinets::cabCHAMP));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x06), emptyPayload).cabinet, Eq(cabinets::cab4x12M));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x07), emptyPayload).cabinet, Eq(cabinets::cab2x12C));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x08), emptyPayload).cabinet, Eq(cabinets::cab4x12G));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x09), emptyPayload).cabinet, Eq(cabinets::cab65TWN));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x0a), emptyPayload).cabinet, Eq(cabinets::cab4x12V));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x0b), emptyPayload).cabinet, Eq(cabinets::cabSS212));
    EXPECT_THAT(decodeAmpFromData(cabinetPackage(0x0c), emptyPayload).cabinet, Eq(cabinets::cabSS112));
}

TEST_F(PacketSerializerTest, decodeAmpFromDataThrowsOnInvalidCabinetId)
{
    EXPECT_THROW(decodeAmpFromData(cabinetPackage(0xe0), emptyPayload), std::invalid_argument);
}

TEST_F(PacketSerializerTest, decodeEffectsFromDataSetsData)
{
    auto package = filledPackage(0x00);
    auto& data = package[2];
    data[FXSLOT] = 0x01;
    data[KNOB1] = 0x11;
    data[KNOB2] = 0x22;
    data[KNOB3] = 0x33;
    data[KNOB4] = 0x44;
    data[KNOB5] = 0x55;
    data[KNOB6] = 0x66;
    data[EFFECT] = 0x49;

    const auto result = decodeEffectsFromData(package);
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
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x00))[1].effect_num, Eq(effects::EMPTY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x3c))[1].effect_num, Eq(effects::OVERDRIVE));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x49))[1].effect_num, Eq(effects::WAH));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x4a))[1].effect_num, Eq(effects::TOUCH_WAH));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x1a))[1].effect_num, Eq(effects::FUZZ));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x1c))[1].effect_num, Eq(effects::FUZZ_TOUCH_WAH));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x88))[1].effect_num, Eq(effects::SIMPLE_COMP));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x07))[1].effect_num, Eq(effects::COMPRESSOR));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x12))[1].effect_num, Eq(effects::SINE_CHORUS));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x13))[1].effect_num, Eq(effects::TRIANGLE_CHORUS));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x18))[1].effect_num, Eq(effects::SINE_FLANGER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x19))[1].effect_num, Eq(effects::TRIANGLE_FLANGER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x2d))[1].effect_num, Eq(effects::VIBRATONE));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x40))[1].effect_num, Eq(effects::VINTAGE_TREMOLO));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x41))[1].effect_num, Eq(effects::SINE_TREMOLO));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x22))[1].effect_num, Eq(effects::RING_MODULATOR));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x29))[1].effect_num, Eq(effects::STEP_FILTER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x4f))[1].effect_num, Eq(effects::PHASER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x1f))[1].effect_num, Eq(effects::PITCH_SHIFTER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x16))[1].effect_num, Eq(effects::MONO_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x43))[1].effect_num, Eq(effects::MONO_ECHO_FILTER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x48))[1].effect_num, Eq(effects::STEREO_ECHO_FILTER));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x44))[1].effect_num, Eq(effects::MULTITAP_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x45))[1].effect_num, Eq(effects::PING_PONG_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x15))[1].effect_num, Eq(effects::DUCKING_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x46))[1].effect_num, Eq(effects::REVERSE_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x2b))[1].effect_num, Eq(effects::TAPE_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x2a))[1].effect_num, Eq(effects::STEREO_TAPE_DELAY));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x24))[1].effect_num, Eq(effects::SMALL_HALL_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x3a))[1].effect_num, Eq(effects::LARGE_HALL_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x26))[1].effect_num, Eq(effects::SMALL_ROOM_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x3b))[1].effect_num, Eq(effects::LARGE_ROOM_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x4e))[1].effect_num, Eq(effects::SMALL_PLATE_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x4b))[1].effect_num, Eq(effects::LARGE_PLATE_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x4c))[1].effect_num, Eq(effects::AMBIENT_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x4d))[1].effect_num, Eq(effects::ARENA_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x21))[1].effect_num, Eq(effects::FENDER_63_SPRING_REVERB));
    EXPECT_THAT(decodeEffectsFromData(effectPackage(0x0b))[1].effect_num, Eq(effects::FENDER_65_SPRING_REVERB));
}

TEST_F(PacketSerializerTest, decodeEffectsFromDataSetsPositionInput)
{
    auto package = filledPackage(0x00);
    package[2][FXSLOT] = 0x00;
    package[3][FXSLOT] = 0x01;
    package[4][FXSLOT] = 0x02;
    package[5][FXSLOT] = 0x03;

    const auto result = decodeEffectsFromData(package);
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
    auto package = filledPackage(0x00);
    package[2][FXSLOT] = 0x04;
    package[3][FXSLOT] = 0x05;
    package[4][FXSLOT] = 0x06;
    package[5][FXSLOT] = 0x07;

    const auto result = decodeEffectsFromData(package);
    EXPECT_THAT(result[0].fx_slot, Eq(0));
    EXPECT_THAT(result[0].position, Eq(Position::effectsLoop));
    EXPECT_THAT(result[1].fx_slot, Eq(1));
    EXPECT_THAT(result[1].position, Eq(Position::effectsLoop));
    EXPECT_THAT(result[2].fx_slot, Eq(2));
    EXPECT_THAT(result[2].position, Eq(Position::effectsLoop));
    EXPECT_THAT(result[3].fx_slot, Eq(3));
    EXPECT_THAT(result[3].position, Eq(Position::effectsLoop));
}
