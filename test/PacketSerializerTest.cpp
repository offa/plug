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

#include "com/PacketSerializer.h"
#include "data_structs.h"
#include <gmock/gmock.h>

using namespace plug;
using namespace plug::com;
using namespace testing;

MATCHER_P4(AmpDataIs, ampId, v0, v1, v2, "")
{
    const std::tuple actual{arg[AMPLIFIER], arg[40], arg[43], arg[44], arg[45], arg[46], arg[50], arg[54]};
    const auto [a0, a1, a2, a3, a4, a5, a6, a7] = actual;
    *result_listener << " with amp specific values: ("
                     << int{a0} << ", {" << int{a1} << ", " << int{a2} << "}, {"
                     << int{a3} << ", " << int{a4} << ", " << int{a5} << ", " << int{a6}
                     << "}, " << int{a7} << ")";
    return std::tuple{ampId, v0, v0, v1, v1, v1, v1, v2} == actual;
}

MATCHER_P(CabinetDataIs, cabinetValue, "")
{
    const auto actual = arg[CABINET];
    *result_listener << " with cabinet data: " << int{actual};
    return actual == cabinetValue;
}

MATCHER_P4(EffectDataIs, dsp, effect, v0, v1, "")
{
    const std::tuple actual{arg[DSP], arg[EFFECT], arg[19], arg[20]};
    const auto [a0, a1, a2, a3] = actual;
    *result_listener << " with effect values: (" << int{a0} << ", " << int{a1}
                     << ", " << int{a2} << ", " << int{a3} << ")";

    return std::tuple{dsp, effect, v0, v1} == actual;
}

MATCHER_P6(KnobsAre, k1, k2, k3, k4, k5, k6, "")
{
    const std::tuple actual{arg[KNOB1], arg[KNOB2], arg[KNOB3],
                            arg[KNOB4], arg[KNOB5], arg[KNOB6]};
    const auto [a1, a2, a3, a4, a5, a6] = actual;
    *result_listener << " with knobs: (" << int{a1} << ", " << int{a2} << ", " << int{a3}
                     << ", " << int{a4} << ", " << int{a5} << ", " << int{a6} << ")";
    return std::tuple{k1, k2, k3, k4, k5, k6} == actual;
}


class PacketSerializerTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
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
    EXPECT_THAT(packets[0], ContainerEq(packet1));
    EXPECT_THAT(packets[1], ContainerEq(packet2));
}

TEST_F(PacketSerializerTest, serializeApplyCommand)
{
    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;

    const auto packet = serializeApplyCommand();
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeLoadCommand)
{
    Packet expected{};
    expected[0] = 0xff;
    expected[1] = 0xc1;

    const auto packet = serializeLoadCommand();
    EXPECT_THAT(packet, ContainerEq(expected));
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
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsData)
{
    const amp_settings settings{amps::METAL_2000, 11, 22, 33, 44, 55, cabinets::cab2x12C, 1, 2, 3, 4, 5, 6, 7, 8, true, 0};

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
    expected[SAG] = 1;
    expected[BRIGHTNESS] = 1;
    expected[AMPLIFIER] = 0x6d;
    expected[44] = 0x08;
    expected[45] = 0x08;
    expected[46] = 0x08;
    expected[50] = 0x08;
    expected[54] = 0x75;

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsWithEmptyData)
{
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, false, 0};

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

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsDspData)
{
    const amp_settings settings{amps::METAL_2000, 123, 101, 93, 30, 61, cabinets::cab2x12C, 3, 10, 15, 40, 0, 0, 100, 1, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[DSP], Eq(0x05));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsAmpControllsData)
{
    const amp_settings settings{amps::METAL_2000, 123, 101, 93, 30, 61, cabinets::cab2x12C, 3, 10, 15, 40, 0, 0, 100, 1, false, 0};

    const auto packet = serializeAmpSettings(settings);
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
    auto create = [](amps a) {
        return amp_settings{a, 0, 0, 0, 0, 0, cabinets::cab2x12C, 0, 0, 0, 0, 0, 0, 0, 0, false, 0};
    };

    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_57_DELUXE)), AmpDataIs(0x67, 0x80, 0x01, 0x53));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_59_BASSMAN)), AmpDataIs(0x64, 0x80, 0x02, 0x67));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_57_CHAMP)), AmpDataIs(0x7c, 0x80, 0x0c, 0x00));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_65_DELUXE_REVERB)), AmpDataIs(0x53, 0x00, 0x03, 0x6a));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_65_PRINCETON)), AmpDataIs(0x6a, 0x80, 0x04, 0x61));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_65_TWIN_REVERB)), AmpDataIs(0x75, 0x80, 0x05, 0x72));
    EXPECT_THAT(serializeAmpSettings(create(amps::FENDER_SUPER_SONIC)), AmpDataIs(0x72, 0x80, 0x06, 0x79));
    EXPECT_THAT(serializeAmpSettings(create(amps::BRITISH_60S)), AmpDataIs(0x61, 0x80, 0x07, 0x5e));
    EXPECT_THAT(serializeAmpSettings(create(amps::BRITISH_70S)), AmpDataIs(0x79, 0x80, 0x0b, 0x7c));
    EXPECT_THAT(serializeAmpSettings(create(amps::BRITISH_80S)), AmpDataIs(0x5e, 0x80, 0x09, 0x5d));
    EXPECT_THAT(serializeAmpSettings(create(amps::AMERICAN_90S)), AmpDataIs(0x5d, 0x80, 0x0a, 0x6d));
    EXPECT_THAT(serializeAmpSettings(create(amps::METAL_2000)), AmpDataIs(0x6d, 0x80, 0x08, 0x75));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsCabinetData)
{
    auto create = [](cabinets c) {
        return amp_settings{amps::BRITISH_70S, 0, 0, 0, 0, 0, c, 0, 0, 0, 0, 0, 0, 0, 0, false, 0};
    };

    EXPECT_THAT(serializeAmpSettings(create(cabinets::OFF)), CabinetDataIs(0x00));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab57DLX)), CabinetDataIs(0x01));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabBSSMN)), CabinetDataIs(0x02));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab65DLX)), CabinetDataIs(0x03));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab65PRN)), CabinetDataIs(0x04));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabCHAMP)), CabinetDataIs(0x05));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab4x12M)), CabinetDataIs(0x06));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab2x12C)), CabinetDataIs(0x07));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab4x12G)), CabinetDataIs(0x08));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab65TWN)), CabinetDataIs(0x09));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cab4x12V)), CabinetDataIs(0x0a));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabSS212)), CabinetDataIs(0x0b));
    EXPECT_THAT(serializeAmpSettings(create(cabinets::cabSS112)), CabinetDataIs(0x0c));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsLimitSagData)
{
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0x03, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[SAG], Eq(0x01));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsBrightnessData)
{
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, true, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[BRIGHTNESS], Eq(0x01));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsNoiseGate)
{
    constexpr std::uint8_t value{0x04};
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, value, 0, 0, 0, 0, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[NOISE_GATE], Eq(value));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsLimitsNoiseGate)
{
    constexpr std::uint8_t value{0x06};
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, value, 0, 0, 0, 0, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[NOISE_GATE], Eq(0x00));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsThresholdAndDepthIfNoiseGateFull)
{
    constexpr std::uint8_t noiseGate{0x05};
    constexpr std::uint8_t threshold{0x08};
    constexpr std::uint8_t depth{0x19};
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, depth, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[THRESHOLD], Eq(threshold));
    EXPECT_THAT(packet[DEPTH], Eq(depth));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsDoesNotSetThresholdAndDepthIfNoiseGateIsNotFull)
{
    constexpr std::uint8_t noiseGate{0x04};
    constexpr std::uint8_t threshold{0x08};
    constexpr std::uint8_t depth{0x19};
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, depth, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[THRESHOLD], Eq(0x00));
    EXPECT_THAT(packet[DEPTH], Eq(0x80));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsSetsLimitsThreshold)
{
    constexpr std::uint8_t noiseGate{0x05};
    constexpr std::uint8_t threshold{0x0a};
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, noiseGate, 0, 0, 0, threshold, 0, 0, 0, false, 0};

    const auto packet = serializeAmpSettings(settings);
    EXPECT_THAT(packet[THRESHOLD], Eq(0x00));
}

TEST_F(PacketSerializerTest, serializeAmpSettingsUsbGain)
{
    constexpr std::size_t value{101};
    const amp_settings settings{amps::BRITISH_60S, 0, 0, 0, 0, 0, cabinets::OFF, 0, 0, 0, 0, 0, 0, 0, 0, false, value};

    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x03;
    expected[2] = 0x0d;
    expected[6] = 0x01;
    expected[7] = 0x01;
    expected[USB_GAIN] = value;

    const auto packet = serializeAmpSettingsUsbGain(settings);
    EXPECT_THAT(packet, ContainerEq(expected));
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
    EXPECT_THAT(packet, ContainerEq(expected));
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

    const auto packet = serializeName(slot, name);
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeNameTerminatesWithZero)
{
    const std::string name{"abc"};
    constexpr std::size_t slot{3};

    const auto packet = serializeName(slot, name);
    EXPECT_THAT(packet[name.size()], Eq('\0'));
}

TEST_F(PacketSerializerTest, serializeNameLimitsToLength)
{
    constexpr std::size_t maxSize{32};
    const std::string name('x', maxSize + 3);
    constexpr std::size_t slot{3};

    Packet expected{};
    expected[0] = 0x1c;
    expected[1] = 0x01;
    expected[2] = 0x03;
    expected[SAVE_SLOT] = slot;
    expected[6] = 0x01;
    expected[7] = 0x01;
    std::copy(name.cbegin(), std::next(name.cbegin(), maxSize - 1), std::next(expected.begin(), NAME));

    const auto packet = serializeName(slot, name);
    EXPECT_THAT(packet[NAME + maxSize], Eq('\0'));
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsData)
{
    const fx_pedal_settings settings{10, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::input};

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
    EXPECT_THAT(packet, ContainerEq(expected));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSetsInputPosition)
{
    constexpr std::uint8_t value{45};
    const fx_pedal_settings settings{value, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet[FXSLOT], Eq(value));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsEffectsSetsLoopPosition)
{
    constexpr std::uint8_t value{60};
    const fx_pedal_settings settings{value, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet[FXSLOT], Eq(value + 4));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsDoesNotSetAdditionalKnobIfNotRequired)
{
    const fx_pedal_settings settings{10, effects::OVERDRIVE, 11, 22, 33, 44, 55, 66, Position::effectsLoop};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet[KNOB6], Eq(0x00));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSetSAdditionalKnobIfRequired)
{
    auto create = [](effects e, std::uint8_t knob6) {
        return fx_pedal_settings{100, e, 0, 0, 0, 0, 0, knob6, Position::input};
    };

    EXPECT_THAT(serializeEffectSettings(create(effects::MONO_ECHO_FILTER, 1))[KNOB6], Eq(1));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_ECHO_FILTER, 2))[KNOB6], Eq(2));
    EXPECT_THAT(serializeEffectSettings(create(effects::TAPE_DELAY, 3))[KNOB6], Eq(3));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_TAPE_DELAY, 4))[KNOB6], Eq(4));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsDspAndEffectIdData)
{
    constexpr std::uint8_t dsp0{0x06};
    constexpr std::uint8_t dsp1{0x07};
    constexpr std::uint8_t dsp2{0x08};
    constexpr std::uint8_t dsp3{0x09};

    auto create = [](effects e) {
        return fx_pedal_settings{100, e, 1, 2, 3, 4, 5, 6, Position::input};
    };

    EXPECT_THAT(serializeEffectSettings(create(effects::OVERDRIVE)), EffectDataIs(dsp0, 0x3c, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::WAH)), EffectDataIs(dsp0, 0x49, 0x01, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::TOUCH_WAH)), EffectDataIs(dsp0, 0x4a, 0x01, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FUZZ)), EffectDataIs(dsp0, 0x1a, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FUZZ_TOUCH_WAH)), EffectDataIs(dsp0, 0x1c, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::SIMPLE_COMP)), EffectDataIs(dsp0, 0x88, 0x08, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::COMPRESSOR)), EffectDataIs(dsp0, 0x07, 0x00, 0x08));

    EXPECT_THAT(serializeEffectSettings(create(effects::SINE_CHORUS)), EffectDataIs(dsp1, 0x12, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::TRIANGLE_CHORUS)), EffectDataIs(dsp1, 0x13, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::SINE_FLANGER)), EffectDataIs(dsp1, 0x18, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::TRIANGLE_FLANGER)), EffectDataIs(dsp1, 0x19, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::VIBRATONE)), EffectDataIs(dsp1, 0x2d, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::VINTAGE_TREMOLO)), EffectDataIs(dsp1, 0x40, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::SINE_TREMOLO)), EffectDataIs(dsp1, 0x41, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::RING_MODULATOR)), EffectDataIs(dsp1, 0x22, 0x01, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEP_FILTER)), EffectDataIs(dsp1, 0x29, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::PHASER)), EffectDataIs(dsp1, 0x4f, 0x01, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::PITCH_SHIFTER)), EffectDataIs(dsp1, 0x1f, 0x01, 0x08));

    EXPECT_THAT(serializeEffectSettings(create(effects::MONO_DELAY)), EffectDataIs(dsp2, 0x16, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::MONO_ECHO_FILTER)), EffectDataIs(dsp2, 0x43, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_ECHO_FILTER)), EffectDataIs(dsp2, 0x48, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::MULTITAP_DELAY)), EffectDataIs(dsp2, 0x44, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::PING_PONG_DELAY)), EffectDataIs(dsp2, 0x45, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::DUCKING_DELAY)), EffectDataIs(dsp2, 0x15, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::REVERSE_DELAY)), EffectDataIs(dsp2, 0x46, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::TAPE_DELAY)), EffectDataIs(dsp2, 0x2b, 0x02, 0x01));
    EXPECT_THAT(serializeEffectSettings(create(effects::STEREO_TAPE_DELAY)), EffectDataIs(dsp2, 0x2a, 0x02, 0x01));

    EXPECT_THAT(serializeEffectSettings(create(effects::SMALL_HALL_REVERB)), EffectDataIs(dsp3, 0x24, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::LARGE_HALL_REVERB)), EffectDataIs(dsp3, 0x3a, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::SMALL_ROOM_REVERB)), EffectDataIs(dsp3, 0x26, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::LARGE_ROOM_REVERB)), EffectDataIs(dsp3, 0x3b, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::SMALL_PLATE_REVERB)), EffectDataIs(dsp3, 0x4e, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::LARGE_PLATE_REVERB)), EffectDataIs(dsp3, 0x4b, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::AMBIENT_REVERB)), EffectDataIs(dsp3, 0x4c, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::ARENA_REVERB)), EffectDataIs(dsp3, 0x4d, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FENDER_63_SPRING_REVERB)), EffectDataIs(dsp3, 0x21, 0x00, 0x08));
    EXPECT_THAT(serializeEffectSettings(create(effects::FENDER_65_SPRING_REVERB)), EffectDataIs(dsp3, 0x0b, 0x00, 0x08));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSimpleCompKnobSetting)
{
    const fx_pedal_settings settings{10, effects::SIMPLE_COMP, 3, 2, 3, 4, 5, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet, KnobsAre(3, 0, 0, 0, 0, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsSimpleCompLimitsValue)
{
    const fx_pedal_settings settings{10, effects::SIMPLE_COMP, 4, 2, 3, 4, 5, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet, KnobsAre(3, 0, 0, 0, 0, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsRingModulatorLimitsValue)
{
    const fx_pedal_settings settings{10, effects::RING_MODULATOR, 1, 2, 3, 2, 5, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet, KnobsAre(1, 2, 3, 1, 5, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsPhaserLimitsValue)
{
    const fx_pedal_settings settings{10, effects::PHASER, 1, 2, 3, 2, 2, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet, KnobsAre(1, 2, 3, 2, 1, 0));
}

TEST_F(PacketSerializerTest, serializeEffectSettingsMultitapDelayLimitsValue)
{
    const fx_pedal_settings settings{10, effects::MULTITAP_DELAY, 1, 2, 3, 2, 4, 6, Position::input};

    const auto packet = serializeEffectSettings(settings);
    EXPECT_THAT(packet, KnobsAre(1, 2, 3, 2, 3, 0));
}
