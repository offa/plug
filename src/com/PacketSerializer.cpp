/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2019  offa
 * Copyright (C) 2010-2016  piorekf <piorek@piorekf.org>
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
#include "com/IdLookup.h"
#include "effects_enum.h"
#include <algorithm>

namespace plug::com
{
    namespace
    {
        template <class T, T upperBound>
        constexpr T clampToRange(T value)
        {
            return std::clamp(value, T{0}, upperBound);
        }

        constexpr std::uint8_t getFxKnob(const fx_pedal_settings& effect)
        {
            if ((effect.effect_num >= effects::SINE_CHORUS) && (effect.effect_num <= effects::PITCH_SHIFTER))
            {
                return 0x01;
            }
            return 0x02;
        }

        constexpr std::uint8_t getSlot(const fx_pedal_settings& effect)
        {
            if (effect.position == Position::effectsLoop)
            {
                constexpr std::uint8_t fxLoopOffset{4};
                return effect.fx_slot + fxLoopOffset;
            }
            return effect.fx_slot;
        }


        constexpr bool hasExtraKnob(effects e)
        {
            switch (e)
            {
                case effects::MONO_ECHO_FILTER:
                case effects::STEREO_ECHO_FILTER:
                case effects::TAPE_DELAY:
                case effects::STEREO_TAPE_DELAY:
                    return true;
                default:
                    return false;
            }
        }


        std::size_t getSaveEffectsRepeats(const std::vector<fx_pedal_settings>& effects)
        {
            const auto size = effects.size();

            if (size > 2)
            {
                return 1;
            }
            const auto effect = effects[0].effect_num;

            if ((effect >= effects::SINE_CHORUS) && (effect <= effects::PITCH_SHIFTER))
            {
                return 1;
            }
            return size;
        }

        Packet serializeSaveEffectHeader(std::uint8_t slot, const std::vector<fx_pedal_settings>& effects)
        {
            Packet packet{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

            const std::size_t repeat = getSaveEffectsRepeats(effects);

            for (std::size_t i = 0; i < repeat; ++i)
            {
                if (effects[i].effect_num < effects::SINE_CHORUS)
                {
                    throw std::invalid_argument{"Invalid effect"};
                }
            }

            packet[FXKNOB] = getFxKnob(effects[0]);
            packet[SAVE_SLOT] = slot;
            packet[1] = 0x03;
            packet[6] = 0x00;
            std::fill(std::next(packet.begin(), 16), std::prev(packet.end(), 16), 0x00);

            return packet;
        }

        Packet serializeSaveEffectBody(Packet packet, const std::vector<fx_pedal_settings>& effects, std::size_t i)
        {
            packet[19] = 0x00;
            packet[20] = 0x08;
            packet[21] = 0x01;
            packet[KNOB6] = 0x00;
            packet[FXSLOT] = getSlot(effects[i]);
            packet[KNOB1] = effects[i].knob1;
            packet[KNOB2] = effects[i].knob2;
            packet[KNOB3] = effects[i].knob3;
            packet[KNOB4] = effects[i].knob4;
            packet[KNOB5] = effects[i].knob5;

            const auto effect = effects[i].effect_num;

            if (hasExtraKnob(effect) == true)
            {
                packet[KNOB6] = effects[i].knob6;
            }

            switch (effect)
            {
                case effects::SINE_CHORUS:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x12;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::TRIANGLE_CHORUS:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x13;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::SINE_FLANGER:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x18;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::TRIANGLE_FLANGER:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x19;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::VIBRATONE:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x2d;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::VINTAGE_TREMOLO:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x40;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::SINE_TREMOLO:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x41;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::RING_MODULATOR:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x22;
                    packet[19] = 0x01;
                    packet[KNOB4] = clampToRange<std::uint8_t, 0x01>(packet[KNOB4]);
                    break;

                case effects::STEP_FILTER:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x29;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    break;

                case effects::PHASER:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x4f;
                    packet[19] = 0x01;
                    packet[20] = 0x01;
                    packet[KNOB5] = clampToRange<std::uint8_t, 0x01>(packet[KNOB5]);
                    break;

                case effects::PITCH_SHIFTER:
                    packet[DSP] = 0x07;
                    packet[EFFECT] = 0x1f;
                    packet[19] = 0x01;
                    break;

                case effects::MONO_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x16;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::MONO_ECHO_FILTER:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x43;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::STEREO_ECHO_FILTER:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x48;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::MULTITAP_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x44;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::PING_PONG_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x45;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::DUCKING_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x15;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::REVERSE_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x46;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::TAPE_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x2b;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::STEREO_TAPE_DELAY:
                    packet[DSP] = 0x08;
                    packet[EFFECT] = 0x2a;
                    packet[19] = 0x02;
                    packet[20] = 0x01;
                    break;

                case effects::SMALL_HALL_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x24;
                    break;

                case effects::LARGE_HALL_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x3a;
                    break;

                case effects::SMALL_ROOM_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x26;
                    break;

                case effects::LARGE_ROOM_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x3b;
                    break;

                case effects::SMALL_PLATE_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x4e;
                    break;

                case effects::LARGE_PLATE_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x4b;
                    break;

                case effects::AMBIENT_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x4c;
                    break;

                case effects::ARENA_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x4d;
                    break;

                case effects::FENDER_63_SPRING_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x21;
                    break;

                case effects::FENDER_65_SPRING_REVERB:
                    packet[DSP] = 0x09;
                    packet[EFFECT] = 0x0b;
                    break;

                default:
                    break;
            }
            return packet;
        }
    }


    std::string decodeNameFromData(const std::array<Packet, 7>& data)
    {
        constexpr std::size_t nameOffset{16};
        constexpr std::size_t nameLength{32};
        const auto& p = data[0];
        const auto start = std::next(p.cbegin(), nameOffset);
        const auto itr = std::find(start, std::next(p.cbegin(), nameOffset + nameLength), '\0');

        return std::string{start, itr};
    }

    amp_settings decodeAmpFromData(const std::array<Packet, 7>& data)
    {
        amp_settings settings{};
        settings.amp_num = lookupAmpById(data[1][AMPLIFIER]);
        settings.gain = data[1][GAIN];
        settings.volume = data[1][VOLUME];
        settings.treble = data[1][TREBLE];
        settings.middle = data[1][MIDDLE];
        settings.bass = data[1][BASS];
        settings.cabinet = lookupCabinetById(data[1][CABINET]);
        settings.noise_gate = data[1][NOISE_GATE];
        settings.master_vol = data[1][MASTER_VOL];
        settings.gain2 = data[1][GAIN2];
        settings.presence = data[1][PRESENCE];
        settings.threshold = data[1][THRESHOLD];
        settings.depth = data[1][DEPTH];
        settings.bias = data[1][BIAS];
        settings.sag = data[1][SAG];
        settings.brightness = data[1][BRIGHTNESS];
        settings.usb_gain = data[6][USB_GAIN];
        return settings;
    }

    std::array<fx_pedal_settings, 4> decodeEffectsFromData(const std::array<Packet, 7>& data)
    {
        std::array<fx_pedal_settings, 4> effects{{}};

        for (std::size_t i = 2; i < 6; ++i)
        {
            const std::size_t j = data[i][FXSLOT] % 4;
            effects[j].fx_slot = static_cast<std::uint8_t>(j);
            effects[j].knob1 = data[i][KNOB1];
            effects[j].knob2 = data[i][KNOB2];
            effects[j].knob3 = data[i][KNOB3];
            effects[j].knob4 = data[i][KNOB4];
            effects[j].knob5 = data[i][KNOB5];
            effects[j].knob6 = data[i][KNOB6];
            effects[j].position = (data[i][FXSLOT] > 0x03 ? Position::effectsLoop : Position::input);
            effects[j].effect_num = lookupEffectById(data[i][EFFECT]);
        }

        return effects;
    }

    std::vector<std::string> decodePresetListFromData(const std::vector<Packet>& data)
    {
        const auto max_to_receive = std::min<std::size_t>(data.size(), (data.size() > 143 ? 200 : 48));
        std::vector<std::string> presetNames;
        presetNames.reserve(max_to_receive);

        for (std::size_t i = 0; i < max_to_receive; i += 2)
        {
            std::string presetName{std::next(data[i].cbegin(), 16), std::next(data[i].cbegin(), 16 + 32)};
            const auto itr = std::find_if(presetName.cbegin(), presetName.cend(), [](const auto& c) { return c == '\0'; });

            presetName.erase(itr, presetName.cend());
            presetNames.push_back(presetName);
        }

        return presetNames;
    }

    Packet serializeAmpSettings(const amp_settings& value)
    {
        Packet packet{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
                       0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
                       0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

        packet[DSP] = 0x05;
        packet[GAIN] = value.gain;
        packet[VOLUME] = value.volume;
        packet[TREBLE] = value.treble;
        packet[MIDDLE] = value.middle;
        packet[BASS] = value.bass;
        packet[CABINET] = plug::value(value.cabinet);
        packet[NOISE_GATE] = clampToRange<std::uint8_t, 0x05>(value.noise_gate);
        packet[MASTER_VOL] = value.master_vol;
        packet[GAIN2] = value.gain2;
        packet[PRESENCE] = value.presence;

        if (value.noise_gate == 0x05)
        {
            packet[THRESHOLD] = clampToRange<uint8_t, 0x09>(value.threshold);
            packet[DEPTH] = value.depth;
        }
        packet[BIAS] = value.bias;
        packet[SAG] = clampToRange<std::uint8_t, 0x02>(value.sag);
        packet[BRIGHTNESS] = value.brightness;

        switch (value.amp_num)
        {
            case amps::FENDER_57_DELUXE:
                packet[AMPLIFIER] = 0x67;
                packet[44] = packet[45] = packet[46] = 0x01;
                packet[50] = 0x01;
                packet[54] = 0x53;
                break;

            case amps::FENDER_59_BASSMAN:
                packet[AMPLIFIER] = 0x64;
                packet[44] = packet[45] = packet[46] = 0x02;
                packet[50] = 0x02;
                packet[54] = 0x67;
                break;

            case amps::FENDER_57_CHAMP:
                packet[AMPLIFIER] = 0x7c;
                packet[44] = packet[45] = packet[46] = 0x0c;
                packet[50] = 0x0c;
                packet[54] = 0x00;
                break;

            case amps::FENDER_65_DELUXE_REVERB:
                packet[AMPLIFIER] = 0x53;
                packet[40] = packet[43] = 0x00;
                packet[44] = packet[45] = packet[46] = 0x03;
                packet[50] = 0x03;
                packet[54] = 0x6a;
                break;

            case amps::FENDER_65_PRINCETON:
                packet[AMPLIFIER] = 0x6a;
                packet[44] = packet[45] = packet[46] = 0x04;
                packet[50] = 0x04;
                packet[54] = 0x61;
                break;

            case amps::FENDER_65_TWIN_REVERB:
                packet[AMPLIFIER] = 0x75;
                packet[44] = packet[45] = packet[46] = 0x05;
                packet[50] = 0x05;
                packet[54] = 0x72;
                break;

            case amps::FENDER_SUPER_SONIC:
                packet[AMPLIFIER] = 0x72;
                packet[44] = packet[45] = packet[46] = 0x06;
                packet[50] = 0x06;
                packet[54] = 0x79;
                break;

            case amps::BRITISH_60S:
                packet[AMPLIFIER] = 0x61;
                packet[44] = packet[45] = packet[46] = 0x07;
                packet[50] = 0x07;
                packet[54] = 0x5e;
                break;

            case amps::BRITISH_70S:
                packet[AMPLIFIER] = 0x79;
                packet[44] = packet[45] = packet[46] = 0x0b;
                packet[50] = 0x0b;
                packet[54] = 0x7c;
                break;

            case amps::BRITISH_80S:
                packet[AMPLIFIER] = 0x5e;
                packet[44] = packet[45] = packet[46] = 0x09;
                packet[50] = 0x09;
                packet[54] = 0x5d;
                break;

            case amps::AMERICAN_90S:
                packet[AMPLIFIER] = 0x5d;
                packet[44] = packet[45] = packet[46] = 0x0a;
                packet[50] = 0x0a;
                packet[54] = 0x6d;
                break;

            case amps::METAL_2000:
                packet[AMPLIFIER] = 0x6d;
                packet[44] = packet[45] = packet[46] = 0x08;
                packet[50] = 0x08;
                packet[54] = 0x75;
                break;
        }
        return packet;
    }

    Packet serializeAmpSettingsUsbGain(const amp_settings& value)
    {
        Packet packet{};
        packet[0] = 0x1c;
        packet[1] = 0x03;
        packet[2] = 0x0d;
        packet[6] = 0x01;
        packet[7] = 0x01;
        packet[USB_GAIN] = value.usb_gain;
        return packet;
    }

    v2::Packet<v2::NamePayload> serializeName(std::uint8_t slot, std::string_view name)
    {
        using v2::NamePayload;
        using v2::Header;
        using v2::Type;
        using v2::DSP;
        using v2::Stage;

        NamePayload payload{};
        payload.setName(name);

        Header header{};
        header.setStage(Stage::ready);
        header.setType(Type::operation);
        header.setDSP(DSP::opSave);
        header.setSlot(slot);
        header.setUnknown(0x01, 0x01);

        v2::Packet<NamePayload> packet{};
        packet.setHeader(header);
        packet.setPayload(payload);
        return packet;
    }

    Packet serializeEffectSettings(const fx_pedal_settings& value)
    {
        Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

        data[FXSLOT] = getSlot(value);
        data[KNOB1] = value.knob1;
        data[KNOB2] = value.knob2;
        data[KNOB3] = value.knob3;
        data[KNOB4] = value.knob4;
        data[KNOB5] = value.knob5;

        if (hasExtraKnob(value.effect_num) == true)
        {
            data[KNOB6] = value.knob6;
        }

        switch (value.effect_num)
        {
            case effects::OVERDRIVE:
                data[DSP] = 0x06;
                data[EFFECT] = 0x3c;
                break;

            case effects::WAH:
                data[DSP] = 0x06;
                data[EFFECT] = 0x49;
                data[19] = 0x01;
                break;

            case effects::TOUCH_WAH:
                data[DSP] = 0x06;
                data[EFFECT] = 0x4a;
                data[19] = 0x01;
                break;

            case effects::FUZZ:
                data[DSP] = 0x06;
                data[EFFECT] = 0x1a;
                break;

            case effects::FUZZ_TOUCH_WAH:
                data[DSP] = 0x06;
                data[EFFECT] = 0x1c;
                break;

            case effects::SIMPLE_COMP:
                data[DSP] = 0x06;
                data[EFFECT] = 0x88;
                data[19] = 0x08;
                data[KNOB1] = clampToRange<std::uint8_t, 0x03>(value.knob1);
                data[KNOB2] = 0x00;
                data[KNOB3] = 0x00;
                data[KNOB4] = 0x00;
                data[KNOB5] = 0x00;
                break;

            case effects::COMPRESSOR:
                data[DSP] = 0x06;
                data[EFFECT] = 0x07;
                break;

            case effects::SINE_CHORUS:
                data[DSP] = 0x07;
                data[EFFECT] = 0x12;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::TRIANGLE_CHORUS:
                data[DSP] = 0x07;
                data[EFFECT] = 0x13;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::SINE_FLANGER:
                data[DSP] = 0x07;
                data[EFFECT] = 0x18;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::TRIANGLE_FLANGER:
                data[DSP] = 0x07;
                data[EFFECT] = 0x19;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::VIBRATONE:
                data[DSP] = 0x07;
                data[EFFECT] = 0x2d;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::VINTAGE_TREMOLO:
                data[DSP] = 0x07;
                data[EFFECT] = 0x40;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::SINE_TREMOLO:
                data[DSP] = 0x07;
                data[EFFECT] = 0x41;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::RING_MODULATOR:
                data[DSP] = 0x07;
                data[EFFECT] = 0x22;
                data[19] = 0x01;
                data[KNOB4] = clampToRange<std::uint8_t, 0x01>(value.knob4);
                break;

            case effects::STEP_FILTER:
                data[DSP] = 0x07;
                data[EFFECT] = 0x29;
                data[19] = 0x01;
                data[20] = 0x01;
                break;

            case effects::PHASER:
                data[DSP] = 0x07;
                data[EFFECT] = 0x4f;
                data[19] = 0x01;
                data[20] = 0x01;
                data[KNOB5] = clampToRange<std::uint8_t, 0x01>(value.knob5);
                break;

            case effects::PITCH_SHIFTER:
                data[DSP] = 0x07;
                data[EFFECT] = 0x1f;
                data[19] = 0x01;
                break;

            case effects::MONO_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x16;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::MONO_ECHO_FILTER:
                data[DSP] = 0x08;
                data[EFFECT] = 0x43;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::STEREO_ECHO_FILTER:
                data[DSP] = 0x08;
                data[EFFECT] = 0x48;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::MULTITAP_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x44;
                data[19] = 0x02;
                data[20] = 0x01;
                data[KNOB5] = clampToRange<std::uint8_t, 0x03>(value.knob5);
                break;

            case effects::PING_PONG_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x45;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::DUCKING_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x15;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::REVERSE_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x46;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::TAPE_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x2b;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::STEREO_TAPE_DELAY:
                data[DSP] = 0x08;
                data[EFFECT] = 0x2a;
                data[19] = 0x02;
                data[20] = 0x01;
                break;

            case effects::SMALL_HALL_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x24;
                break;

            case effects::LARGE_HALL_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x3a;
                break;

            case effects::SMALL_ROOM_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x26;
                break;

            case effects::LARGE_ROOM_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x3b;
                break;

            case effects::SMALL_PLATE_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x4e;
                break;

            case effects::LARGE_PLATE_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x4b;
                break;

            case effects::AMBIENT_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x4c;
                break;

            case effects::ARENA_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x4d;
                break;

            case effects::FENDER_63_SPRING_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x21;
                break;

            case effects::FENDER_65_SPRING_REVERB:
                data[DSP] = 0x09;
                data[EFFECT] = 0x0b;
                break;

            default:
                break;
        }

        return data;
    }

    Packet serializeClearEffectSettings()
    {
        return Packet{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    }

    Packet serializeSaveEffectName(std::uint8_t slot, std::string_view name, const std::vector<fx_pedal_settings>& effects)
    {
        Packet packet{{0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

        const std::size_t repeat = getSaveEffectsRepeats(effects);

        for (std::size_t i = 0; i < repeat; ++i)
        {
            if (effects[i].effect_num < effects::SINE_CHORUS)
            {
                throw std::invalid_argument{"Invalid effect"};
            }
        }

        packet[FXKNOB] = getFxKnob(effects[0]);
        packet[SAVE_SLOT] = slot;

        constexpr std::size_t nameLength{24};
        const auto length = std::min(nameLength, name.size());
        std::copy(name.cbegin(), std::next(name.cbegin(), length), std::next(packet.begin(), NAME));

        return packet;
    }

    std::vector<Packet> serializeSaveEffectPacket(std::uint8_t slot, const std::vector<fx_pedal_settings>& effects)
    {
        const std::size_t repeat = getSaveEffectsRepeats(effects);

        const auto packet = serializeSaveEffectHeader(slot, effects);
        std::vector<Packet> packets;

        for (std::size_t i = 0; i < repeat; ++i)
        {
            const auto settingsPacket = serializeSaveEffectBody(packet, effects, i);
            packets.push_back(settingsPacket);
        }

        return packets;
    }

    v2::Packet<v2::EmptyPayload> serializeLoadSlotCommand(std::uint8_t slot)
    {
        using v2::EmptyPayload;
        using v2::Header;
        using v2::Type;
        using v2::DSP;
        using v2::Stage;

        Header h{};
        h.setStage(Stage::ready);
        h.setType(Type::operation);
        h.setDSP(DSP::opSelectMemBank);
        h.setSlot(slot);
        h.setUnknown(0x01, 0x00);

        v2::Packet<EmptyPayload> loadCommand{};
        loadCommand.setHeader(h);
        loadCommand.setPayload(EmptyPayload{});
        return loadCommand;
    }

    v2::Packet<v2::EmptyPayload> serializeLoadCommand()
    {
        using v2::EmptyPayload;
        using v2::Header;
        using v2::Type;
        using v2::DSP;
        using v2::Stage;

        Header h{};
        h.setStage(Stage::unknown);
        h.setType(Type::load);
        h.setDSP(DSP::none);
        v2::Packet<EmptyPayload> loadCommand{};
        loadCommand.setHeader(h);
        loadCommand.setPayload(EmptyPayload{});
        return loadCommand;
    }

    v2::Packet<v2::EmptyPayload> serializeApplyCommand()
    {
        using v2::EmptyPayload;
        using v2::Header;
        using v2::Type;
        using v2::DSP;
        using v2::Stage;

        Header h{};
        h.setStage(Stage::ready);
        h.setType(Type::data);
        h.setDSP(DSP::none);
        v2::Packet<EmptyPayload> applyCommand{};
        applyCommand.setHeader(h);
        applyCommand.setPayload(EmptyPayload{});
        return applyCommand;
    }

    Packet serializeApplyCommand(fx_pedal_settings effect)
    {
        auto applyCommand = serializeApplyCommand().getBytes();
        applyCommand[FXKNOB] = getFxKnob(effect);
        return applyCommand;
    }

    std::array<v2::Packet<v2::EmptyPayload>, 2> serializeInitCommand()
    {
        using v2::EmptyPayload;
        using v2::Header;
        using v2::Type;
        using v2::DSP;
        using v2::Stage;

        Header h0{};
        h0.setStage(Stage::init0);
        h0.setType(Type::init0);
        h0.setDSP(DSP::none);
        v2::Packet<EmptyPayload> p0{};
        p0.setHeader(h0);
        p0.setPayload(EmptyPayload{});

        Header h1{};
        h1.setStage(Stage::init1);
        h1.setType(Type::init1);
        h1.setDSP(DSP::none);
        v2::Packet<EmptyPayload> p1{};
        p1.setHeader(h1);
        p1.setPayload(EmptyPayload{});

        return {{p0, p1}};
    }
}
