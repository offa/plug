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

#include "com/Packet.h"
#include <gmock/gmock.h>

using namespace testing;
using namespace plug::com;


class PacketTest : public testing::Test
{
protected:
    static inline constexpr std::size_t sizePayload{48};
};

TEST_F(PacketTest, packetFormat)
{
    Header h{};
    h.setDSP(DSP::opSave);
    EffectPayload pl{};
    pl.setKnob3(3);

    const Packet<EffectPayload> p{h, pl};
    Packet<EffectPayload> p2{};
    p2.setHeader(h);
    p2.setPayload(pl);


    EXPECT_THAT(p.getHeader().getBytes(), ContainerEq(h.getBytes()));
    EXPECT_THAT(p.getPayload().getBytes(), ContainerEq(pl.getBytes()));
    EXPECT_THAT(p2.getHeader().getBytes(), ContainerEq(h.getBytes()));
    EXPECT_THAT(p2.getPayload().getBytes(), ContainerEq(pl.getBytes()));
}

TEST_F(PacketTest, packetFromData)
{
    Header h{};
    h.setDSP(DSP::opSaveEffectName);
    const auto headerBytes = h.getBytes();

    const std::string name = "abcdefg";
    NamePayload pl{};
    pl.setName(name);
    const auto payloadBytes = pl.getBytes();

    std::array<std::uint8_t, 64> data{{}};
    auto itr = std::copy(headerBytes.cbegin(), headerBytes.cend(), data.begin());
    std::copy(payloadBytes.cbegin(), payloadBytes.cend(), itr);

    Packet<NamePayload> p{};
    p.fromBytes(data);

    EXPECT_THAT(p.getHeader().getDSP(), Eq(DSP::opSaveEffectName));
    EXPECT_THAT(p.getPayload().getName(), Eq(name));
}

TEST_F(PacketTest, packetBytes)
{
    Header h{};
    h.setDSP(DSP::effect0);
    EffectPayload pl{};
    pl.setKnob2(7);
    const auto headerBytes = h.getBytes();
    const auto payloadBytes = pl.getBytes();

    const Packet<EffectPayload> p{h, pl};
    std::array<std::uint8_t, 64> expected{{}};
    const auto itr = std::copy(headerBytes.cbegin(), headerBytes.cend(), expected.begin());
    std::copy(payloadBytes.cbegin(), payloadBytes.cend(), itr);

    EXPECT_THAT(p.getBytes(), ContainerEq(expected));
}

TEST_F(PacketTest, headerFixedFields)
{
    Header h{};
    const auto bytes = h.getBytes();
    EXPECT_THAT(bytes[3], Eq(0x00));
    EXPECT_THAT(bytes[5], Eq(0x00));
    EXPECT_THAT(bytes[8], Eq(0x00));
    EXPECT_THAT(bytes[9], Eq(0x00));
    EXPECT_THAT(bytes[10], Eq(0x00));
    EXPECT_THAT(bytes[11], Eq(0x00));
    EXPECT_THAT(bytes[12], Eq(0x00));
    EXPECT_THAT(bytes[13], Eq(0x00));
    EXPECT_THAT(bytes[13], Eq(0x00));
    EXPECT_THAT(bytes[15], Eq(0x00));
}

TEST_F(PacketTest, headerUnknownSpecificFields)
{
    Header h{};
    EXPECT_THAT(h.getBytes()[3], Eq(0x00));
    EXPECT_THAT(h.getBytes()[6], Eq(0x00));
    EXPECT_THAT(h.getBytes()[7], Eq(0x00));
    h.setUnknown(0x02, 0x01, 0x03);
    EXPECT_THAT(h.getBytes()[3], Eq(0x02));
    EXPECT_THAT(h.getBytes()[6], Eq(0x01));
    EXPECT_THAT(h.getBytes()[7], Eq(0x03));
}

TEST_F(PacketTest, headerStage)
{
    Header h{};
    h.setStage(Stage::init0);
    EXPECT_THAT(h.getBytes()[0], Eq(0x00));
    h.setStage(Stage::init1);
    EXPECT_THAT(h.getBytes()[0], Eq(0x1a));
    h.setStage(Stage::ready);
    EXPECT_THAT(h.getBytes()[0], Eq(0x1c));
    h.setStage(Stage::unknown);
    EXPECT_THAT(h.getBytes()[0], Eq(0xff));
}

TEST_F(PacketTest, headerType)
{
    Header h{};
    h.setType(Type::operation);
    EXPECT_THAT(h.getBytes()[1], Eq(0x01));
    h.setType(Type::data);
    EXPECT_THAT(h.getBytes()[1], Eq(0x03));
    h.setType(Type::init0);
    EXPECT_THAT(h.getBytes()[1], Eq(0xc3));
    h.setType(Type::init1);
    EXPECT_THAT(h.getBytes()[1], Eq(0x03));
    h.setType(Type::load);
    EXPECT_THAT(h.getBytes()[1], Eq(0xc1));
}

TEST_F(PacketTest, headerDsp)
{
    Header h{};
    h.setDSP(DSP::none);
    EXPECT_THAT(h.getBytes()[2], Eq(0x00));
    h.setDSP(DSP::amp);
    EXPECT_THAT(h.getBytes()[2], Eq(0x05));
    h.setDSP(DSP::usbGain);
    EXPECT_THAT(h.getBytes()[2], Eq(0x0d));
    h.setDSP(DSP::effect0);
    EXPECT_THAT(h.getBytes()[2], Eq(0x06));
    h.setDSP(DSP::effect1);
    EXPECT_THAT(h.getBytes()[2], Eq(0x07));
    h.setDSP(DSP::effect2);
    EXPECT_THAT(h.getBytes()[2], Eq(0x08));
    h.setDSP(DSP::effect3);
    EXPECT_THAT(h.getBytes()[2], Eq(0x09));
    h.setDSP(DSP::opSave);
    EXPECT_THAT(h.getBytes()[2], Eq(0x03));
    h.setDSP(DSP::opSaveEffectName);
    EXPECT_THAT(h.getBytes()[2], Eq(0x04));
    h.setDSP(DSP::opSelectMemBank);
    EXPECT_THAT(h.getBytes()[2], Eq(0x01));
}

TEST_F(PacketTest, headerSlot)
{
    Header h{};
    h.setSlot(10);
    EXPECT_THAT(h.getBytes()[4], Eq(10));
}

TEST_F(PacketTest, headerFromData)
{
    std::array<std::uint8_t, 16> data{{}};
    data[0] = 0x1a;
    data[1] = 0x03;
    data[2] = 0x09;
    data[4] = 0xab;

    Header header{};
    header.fromBytes(data);
    EXPECT_THAT(header.getStage(), Eq(Stage::init1));
    EXPECT_THAT(header.getType(), Eq(Type::data));
    EXPECT_THAT(header.getDSP(), Eq(DSP::effect3));
    EXPECT_THAT(header.getSlot(), Eq(0xab));
}

TEST_F(PacketTest, headerStageFromData)
{
    std::array<std::uint8_t, 16> data{{}};
    Header header{};

    data[0] = 0x00;
    header.fromBytes(data);
    EXPECT_THAT(header.getStage(), Eq(Stage::init0));
    data[0] = 0x1a;
    header.fromBytes(data);
    EXPECT_THAT(header.getStage(), Eq(Stage::init1));
    data[0] = 0x1c;
    header.fromBytes(data);
    EXPECT_THAT(header.getStage(), Eq(Stage::ready));
    data[0] = 0x93;
    header.fromBytes(data);
    EXPECT_THAT(header.getStage(), Eq(Stage::unknown));
}

TEST_F(PacketTest, headerTypeFromData)
{
    std::array<std::uint8_t, 16> data{{}};
    Header header{};

    data[1] = 0x01;
    header.fromBytes(data);
    EXPECT_THAT(header.getType(), Eq(Type::operation));
    data[1] = 0x03;
    header.fromBytes(data);
    EXPECT_THAT(header.getType(), Eq(Type::data));
    data[1] = 0xc3;
    header.fromBytes(data);
    EXPECT_THAT(header.getType(), Eq(Type::init0));
    data[1] = 0xc1;
    header.fromBytes(data);
    EXPECT_THAT(header.getType(), Eq(Type::load));
}

TEST_F(PacketTest, headerTypeFromDataThrowsOnInvalidValue)
{
    std::array<std::uint8_t, 16> data{{}};
    Header header{};

    data[1] = 0x99;
    header.fromBytes(data);
    EXPECT_THROW(header.getType(), std::domain_error);
}

TEST_F(PacketTest, headerDSPFromData)
{
    std::array<std::uint8_t, 16> data{{}};
    Header header{};

    data[2] = 0x00;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::none));
    data[2] = 0x05;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::amp));
    data[2] = 0x0d;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::usbGain));
    data[2] = 0x06;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::effect0));
    data[2] = 0x07;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::effect1));
    data[2] = 0x08;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::effect2));
    data[2] = 0x09;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::effect3));
    data[2] = 0x03;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::opSave));
    data[2] = 0x04;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::opSaveEffectName));
    data[2] = 0x01;
    header.fromBytes(data);
    EXPECT_THAT(header.getDSP(), Eq(DSP::opSelectMemBank));
}

TEST_F(PacketTest, headerDSPFromDataThrowsOnInvalidValue)
{
    std::array<std::uint8_t, 16> data{{}};
    Header header{};

    data[2] = 0x99;
    header.fromBytes(data);
    EXPECT_THROW(header.getDSP(), std::domain_error);
}

TEST_F(PacketTest, headerSlotFromData)
{
    std::array<std::uint8_t, 16> data{{}};
    data[4] = 0x17;

    Header header{};
    header.fromBytes(data);
    EXPECT_THAT(header.getSlot(), Eq(0x17));
}

TEST_F(PacketTest, emptyPayload)
{
    EmptyPayload p{};
    EXPECT_THAT(p.getBytes(), Each(0x00));
}

TEST_F(PacketTest, namePayload)
{
    const std::string name = "abc def ghi 123";
    std::array<std::uint8_t, sizePayload> expected{{}};
    expected.fill(0x00);
    std::copy(name.cbegin(), name.cend(), expected.begin());

    NamePayload p{};
    p.setName(name);

    EXPECT_THAT(p.getBytes(), ContainerEq(expected));
}

TEST_F(PacketTest, namePayloadFixedFields)
{
    NamePayload p{};
    const auto bytes = p.getBytes();
    std::array<std::uint8_t, 16> actual{{}};
    std::copy_n(std::next(bytes.cbegin(), 32), 16, actual.begin());

    EXPECT_THAT(actual, Each(0x00));
}

TEST_F(PacketTest, namePayloadHasLimitedNameLength)
{
    const std::string name = "aaaaaaaaaabbbbbbbbbbccccccccccdddddddddd";
    std::array<std::uint8_t, sizePayload> expected{{}};
    expected.fill(0x00);
    std::copy_n(name.cbegin(), 32, expected.begin());

    NamePayload p{};
    p.setName(name);

    EXPECT_THAT(p.getBytes(), ContainerEq(expected));
}

TEST_F(PacketTest, namePayloadFromData)
{
    const std::string name = "abc 123";
    std::array<std::uint8_t, sizePayload> data{{}};
    data.fill(0x00);
    std::copy(name.cbegin(), name.cend(), data.begin());

    NamePayload p{};
    p.fromBytes(data);

    EXPECT_THAT(p.getName(), Eq(name));
}

TEST_F(PacketTest, namePayloadFromDataLimitsLength)
{
    const std::string name = "00000000001111111111222222222233xxxxxxxx";
    std::array<std::uint8_t, sizePayload> data{{}};
    data.fill(0x00);
    std::copy(name.cbegin(), name.cend(), data.begin());

    NamePayload p{};
    p.fromBytes(data);

    EXPECT_THAT(p.getName().size(), Eq(32));
    EXPECT_THAT(p.getName(), Eq("00000000001111111111222222222233"));
}

TEST_F(PacketTest, effectPayloadKnobs)
{
    EffectPayload p{};
    p.setKnob1(11);
    p.setKnob2(12);
    p.setKnob3(13);
    p.setKnob4(14);
    p.setKnob5(15);
    p.setKnob6(16);

    EXPECT_THAT(p.getBytes()[16], Eq(11));
    EXPECT_THAT(p.getBytes()[17], Eq(12));
    EXPECT_THAT(p.getBytes()[18], Eq(13));
    EXPECT_THAT(p.getBytes()[19], Eq(14));
    EXPECT_THAT(p.getBytes()[20], Eq(15));
    EXPECT_THAT(p.getBytes()[21], Eq(16));
}

TEST_F(PacketTest, effectPayloadSlot)
{
    EffectPayload p{};
    p.setSlot(3);

    EXPECT_THAT(p.getBytes()[2], Eq(3));
}

TEST_F(PacketTest, effectPayloadModel)
{
    EffectPayload p{};
    p.setModel(17);

    EXPECT_THAT(p.getBytes()[0], Eq(17));
}

TEST_F(PacketTest, effectPayloadFixedFields)
{
    constexpr std::uint8_t unknown{0x00};
    std::array<std::uint8_t, 48> expected{{}};
    expected.fill(0x00);
    expected[1] = 0x00;
    expected[3] = unknown;
    expected[4] = unknown;
    expected[5] = unknown;

    EffectPayload p{};
    const auto bytes = p.getBytes();
    std::array<std::uint8_t, 10> empty0{{}};
    std::copy_n(std::next(bytes.cbegin(), 6), 10, empty0.begin());
    std::array<std::uint8_t, 10> empty1{{}};
    std::copy_n(std::next(bytes.cbegin(), 22), 10, empty1.begin());
    std::array<std::uint8_t, 16> empty2{{}};
    std::copy_n(std::next(bytes.cbegin(), 32), 16, empty2.begin());

    EXPECT_THAT(bytes[1], Eq(0x00));
    EXPECT_THAT(bytes[3], Eq(unknown));
    EXPECT_THAT(bytes[4], Eq(unknown));
    EXPECT_THAT(bytes[5], Eq(unknown));
    EXPECT_THAT(empty0, Each(0x00));
    EXPECT_THAT(empty1, Each(0x00));
    EXPECT_THAT(empty2, Each(0x00));
}

TEST_F(PacketTest, effectPayloadUnknownSpecificFields)
{
    EffectPayload p{};
    p.setUnknown(0x11, 0x22, 0x33);

    EXPECT_THAT(p.getBytes()[3], Eq(0x11));
    EXPECT_THAT(p.getBytes()[4], Eq(0x22));
    EXPECT_THAT(p.getBytes()[5], Eq(0x33));
}

TEST_F(PacketTest, effectPayloadFromData)
{
    std::array<std::uint8_t, 48> data{{}};
    data[0] = 8;
    data[2] = 3;
    data[16] = 11;
    data[17] = 12;
    data[18] = 13;
    data[19] = 14;
    data[20] = 15;
    data[21] = 16;

    EffectPayload p{};
    p.fromBytes(data);

    EXPECT_THAT(p.getModel(), Eq(8));
    EXPECT_THAT(p.getSlot(), Eq(3));
    EXPECT_THAT(p.getKnob1(), Eq(11));
    EXPECT_THAT(p.getKnob2(), Eq(12));
    EXPECT_THAT(p.getKnob3(), Eq(13));
    EXPECT_THAT(p.getKnob4(), Eq(14));
    EXPECT_THAT(p.getKnob5(), Eq(15));
    EXPECT_THAT(p.getKnob6(), Eq(16));
}

TEST_F(PacketTest, ampPayloadSettings)
{
    AmpPayload p{};
    p.setModel(0xab);
    p.setVolume(0xaa);
    p.setGain(0x11);
    p.setGain2(0x22);
    p.setMasterVolume(0x03);
    p.setTreble(0x1a);
    p.setMiddle(0x1b);
    p.setBass(0x1c);
    p.setPresence(0x1d);
    p.setDepth(0x21);
    p.setBias(0x12);
    p.setNoiseGate(0x05);
    p.setCabinet(0x06);
    p.setThreshold(0x07);
    p.setSag(0x08);
    p.setBrightness(0x09);

    EXPECT_THAT(p.getBytes()[0], Eq(0xab));
    EXPECT_THAT(p.getBytes()[16], Eq(0xaa));
    EXPECT_THAT(p.getBytes()[17], Eq(0x11));
    EXPECT_THAT(p.getBytes()[18], Eq(0x22));
    EXPECT_THAT(p.getBytes()[19], Eq(0x03));
    EXPECT_THAT(p.getBytes()[20], Eq(0x1a));
    EXPECT_THAT(p.getBytes()[21], Eq(0x1b));
    EXPECT_THAT(p.getBytes()[22], Eq(0x1c));
    EXPECT_THAT(p.getBytes()[23], Eq(0x1d));
    EXPECT_THAT(p.getBytes()[25], Eq(0x21));
    EXPECT_THAT(p.getBytes()[26], Eq(0x12));
    EXPECT_THAT(p.getBytes()[31], Eq(0x05));
    EXPECT_THAT(p.getBytes()[32], Eq(0x07));
    EXPECT_THAT(p.getBytes()[33], Eq(0x06));
    EXPECT_THAT(p.getBytes()[35], Eq(0x08));
    EXPECT_THAT(p.getBytes()[36], Eq(0x09));
}

TEST_F(PacketTest, ampPayloadUsbSettings)
{
    AmpPayload p{};
    p.setUsbGain(0x12);

    EXPECT_THAT(p.getBytes()[0], Eq(0x12));
}

TEST_F(PacketTest, ampPayloadFixedFields)
{
    constexpr std::uint8_t unknown{0x00};
    AmpPayload p{};
    const auto bytes = p.getBytes();
    std::array<std::uint8_t, 15> empty0{{}};
    std::copy_n(std::next(bytes.cbegin(), 1), 15, empty0.begin());
    std::array<std::uint8_t, 9> empty1{{}};
    std::copy_n(std::next(bytes.cbegin(), 39), 9, empty0.begin());

    EXPECT_THAT(bytes[24], Eq(unknown));
    EXPECT_THAT(bytes[27], Eq(unknown));
    EXPECT_THAT(bytes[28], Eq(unknown));
    EXPECT_THAT(bytes[29], Eq(unknown));
    EXPECT_THAT(bytes[30], Eq(unknown));
    EXPECT_THAT(bytes[34], Eq(unknown));
    EXPECT_THAT(bytes[38], Eq(unknown));
    EXPECT_THAT(empty0, Each(0x00));
    EXPECT_THAT(empty1, Each(0x00));
}

TEST_F(PacketTest, ampPayloadUnknownSpecificFields)
{
    AmpPayload p{};
    p.setUnknown(0x01, 0x02, 0x03);
    p.setUnknownAmpSpecific(0x21, 0x22, 0x23, 0x24, 0x25);

    EXPECT_THAT(p.getBytes()[24], Eq(0x01));
    EXPECT_THAT(p.getBytes()[27], Eq(0x02));
    EXPECT_THAT(p.getBytes()[37], Eq(0x03));

    EXPECT_THAT(p.getBytes()[28], Eq(0x21));
    EXPECT_THAT(p.getBytes()[29], Eq(0x22));
    EXPECT_THAT(p.getBytes()[30], Eq(0x23));
    EXPECT_THAT(p.getBytes()[34], Eq(0x24));
    EXPECT_THAT(p.getBytes()[38], Eq(0x25));
}

TEST_F(PacketTest, ampPayloadFromData)
{
    std::array<std::uint8_t, sizePayload> data{{}};
    data[0] = 0xab;
    data[16] = 0xaa;
    data[17] = 0x11;
    data[18] = 0x22;
    data[19] = 0x03;
    data[20] = 0x1a;
    data[21] = 0x1b;
    data[22] = 0x1c;
    data[23] = 0x1d;
    data[25] = 0x21;
    data[26] = 0x12;
    data[31] = 0x05;
    data[32] = 0x07;
    data[33] = 0x06;
    data[35] = 0x08;
    data[36] = 0x09;

    AmpPayload p{};
    p.fromBytes(data);

    EXPECT_THAT(p.getModel(), Eq(0xab));
    EXPECT_THAT(p.getVolume(), Eq(0xaa));
    EXPECT_THAT(p.getGain(), Eq(0x11));
    EXPECT_THAT(p.getGain2(), Eq(0x22));
    EXPECT_THAT(p.getMasterVolume(), Eq(0x03));
    EXPECT_THAT(p.getTreble(), Eq(0x1a));
    EXPECT_THAT(p.getMiddle(), Eq(0x1b));
    EXPECT_THAT(p.getBass(), Eq(0x1c));
    EXPECT_THAT(p.getPresence(), Eq(0x1d));
    EXPECT_THAT(p.getDepth(), Eq(0x21));
    EXPECT_THAT(p.getBias(), Eq(0x12));
    EXPECT_THAT(p.getNoiseGate(), Eq(0x05));
    EXPECT_THAT(p.getCabinet(), Eq(0x06));
    EXPECT_THAT(p.getThreshold(), Eq(0x07));
    EXPECT_THAT(p.getSag(), Eq(0x08));
    EXPECT_THAT(p.getBrightness(), Eq(0x09));
}


TEST_F(PacketTest, ampPayloadUsbGainFromData)
{
    std::array<std::uint8_t, sizePayload> data{{}};
    data[0] = 0x12;

    AmpPayload p{};
    p.fromBytes(data);

    EXPECT_THAT(p.getUsbGain(), Eq(0x12));
}
