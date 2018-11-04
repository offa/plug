/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2018  offa
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

namespace plug::com
{
    namespace
    {
        std::size_t getSaveEffectsRepeats(const std::vector<fx_pedal_settings>& effects)
        {
            const auto size = effects.size();

            if (size > 2)
            {
                return 1;
            }
            if ((effects[0].effect_num >= effects::SINE_CHORUS) && (effects[0].effect_num <= effects::PITCH_SHIFTER))
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

            std::size_t repeat = getSaveEffectsRepeats(effects);

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

            if (effects[i].position == Position::effectsLoop)
            {
                packet[FXSLOT] = effects[i].fx_slot + 4;
            }
            else
            {
                packet[FXSLOT] = effects[i].fx_slot;
            }
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
                    if (packet[KNOB4] > 0x01)
                    {
                        packet[KNOB4] = 0x01;
                    }
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
                    if (packet[KNOB5] > 0x01)
                    {
                        packet[KNOB5] = 0x01;
                    }
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
        constexpr std::size_t nameLength{32};
        std::string name(nameLength, '\0');
        std::size_t length{0};

        for (std::size_t i = 0, j = 16; data[0][j] != 0x00; ++i, ++j)
        {
            name[i] = data[0][j];
            ++length;

            if (length > nameLength)
            {
                break;
            }
        }

        name.resize(std::min(length, nameLength));
        return name;
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
        settings.cabinet = static_cast<cabinets>(data[1][CABINET]);
        settings.noise_gate = data[1][NOISE_GATE];
        settings.master_vol = data[1][MASTER_VOL];
        settings.gain2 = data[1][GAIN2];
        settings.presence = data[1][PRESENCE];
        settings.threshold = data[1][THRESHOLD];
        settings.depth = data[1][DEPTH];
        settings.bias = data[1][BIAS];
        settings.sag = data[1][SAG];
        settings.brightness = data[1][BRIGHTNESS] != 0u;
        settings.usb_gain = data[6][USB_GAIN];
        return settings;
    }

    std::array<fx_pedal_settings, 4> decodeEffectsFromData(const std::array<Packet, 7>& data)
    {
        std::array<fx_pedal_settings, 4> effects{{}};
        decodeEffectsFromData(data, effects.data());
        return effects;
    }

    void decodeEffectsFromData(const std::array<Packet, 7>& data, fx_pedal_settings* const& effects_set_out)
    {
        for (int i = 2; i < 6; ++i)
        {
            int j = 0;

            switch (data[i][FXSLOT])
            {
                case 0x00:
                case 0x04:
                    j = 0;
                    break;

                case 0x01:
                case 0x05:
                    j = 1;
                    break;

                case 0x02:
                case 0x06:
                    j = 2;
                    break;

                case 0x03:
                case 0x07:
                    j = 3;
                    break;
            }

            effects_set_out[j].fx_slot = j;
            effects_set_out[j].knob1 = data[i][KNOB1];
            effects_set_out[j].knob2 = data[i][KNOB2];
            effects_set_out[j].knob3 = data[i][KNOB3];
            effects_set_out[j].knob4 = data[i][KNOB4];
            effects_set_out[j].knob5 = data[i][KNOB5];
            effects_set_out[j].knob6 = data[i][KNOB6];
            effects_set_out[j].position = (data[i][FXSLOT] > 0x03 ? Position::effectsLoop : Position::input);
            effects_set_out[j].effect_num = lookupEffectById(data[i][EFFECT]);
        }
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

        if (value.noise_gate > 0x05)
        {
            packet[NOISE_GATE] = 0x00;
        }
        else
        {
            packet[NOISE_GATE] = value.noise_gate;
        }

        packet[MASTER_VOL] = value.master_vol;
        packet[GAIN2] = value.gain2;
        packet[PRESENCE] = value.presence;

        if (value.noise_gate == 0x05)
        {
            if (value.threshold > 0x09)
            {
                packet[THRESHOLD] = 0x00;
            }
            else
            {
                packet[THRESHOLD] = value.threshold;
            }

            packet[DEPTH] = value.depth;
        }
        packet[BIAS] = value.bias;

        if (value.sag > 0x02)
        {
            packet[SAG] = 0x01;
        }
        else
        {
            packet[SAG] = value.sag;
        }

        packet[BRIGHTNESS] = value.brightness ? 1 : 0;

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
        Packet packet{{0}};
        packet.fill(0x00);
        packet[0] = 0x1c;
        packet[1] = 0x03;
        packet[2] = 0x0d;
        packet[6] = 0x01;
        packet[7] = 0x01;
        packet[USB_GAIN] = value.usb_gain;
        return packet;
    }

    Packet serializeName(std::uint8_t slot, std::string_view name)
    {
        Packet data;
        data.fill(0x00);
        data[0] = 0x1c;
        data[1] = 0x01;
        data[2] = 0x03;
        data[SAVE_SLOT] = slot;
        data[6] = 0x01;
        data[7] = 0x01;

        constexpr std::size_t nameLength{31};
        std::string sizedName{name};
        sizedName.resize(nameLength, '\0');
        std::copy(sizedName.cbegin(), std::next(sizedName.cend()), std::next(data.data(), NAME));
        return data;
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

        const std::uint8_t slot = (value.position == Position::effectsLoop ? (value.fx_slot + 4) : value.fx_slot);

        data[FXSLOT] = slot;
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
                if (data[KNOB1] > 0x03)
                {
                    data[KNOB1] = 0x03;
                }
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
                if (data[KNOB4] > 0x01)
                {
                    data[KNOB4] = 0x01;
                }
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
                if (data[KNOB5] > 0x01)
                {
                    data[KNOB5] = 0x01;
                }
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
                if (data[KNOB5] > 0x03)
                {
                    data[KNOB5] = 0x03;
                }
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
        Packet data{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

        data[EFFECT] = 0x00;
        data[KNOB1] = 0x00;
        data[KNOB2] = 0x00;
        data[KNOB3] = 0x00;
        data[KNOB4] = 0x00;
        data[KNOB5] = 0x00;
        data[KNOB6] = 0x00;
        return data;
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
        std::string sizedName{name};
        sizedName.resize(nameLength, '\0');
        std::copy(sizedName.cbegin(), std::next(sizedName.cend()), std::next(packet.begin(), NAME));

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

    Packet serializeLoadSlotCommand(std::uint8_t slot)
    {
        Packet loadCommand{};
        loadCommand[0] = 0x1c;
        loadCommand[1] = 0x01;
        loadCommand[2] = 0x01;
        loadCommand[SAVE_SLOT] = slot;
        loadCommand[6] = 0x01;
        return loadCommand;
    }

    Packet serializeLoadCommand()
    {
        Packet loadCommand{};
        loadCommand[0] = 0xff;
        loadCommand[1] = 0xc1;
        return loadCommand;
    }

    Packet serializeApplyCommand()
    {
        Packet applyCommand{};
        applyCommand[0] = 0x1c;
        applyCommand[1] = 0x03;
        return applyCommand;
    }

    Packet serializeApplyCommand(std::uint8_t fxKnob)
    {
        Packet applyCommand{};
        applyCommand[0] = 0x1c;
        applyCommand[1] = 0x03;
        applyCommand[FXKNOB] = fxKnob;
        return applyCommand;
    }

    std::array<Packet, 2> serializeInitCommand()
    {
        Packet packet1{};
        packet1[1] = 0xc3;

        Packet packet2{};
        packet2[0] = 0x1a;
        packet2[1] = 0x03;

        return {{packet1, packet2}};
    }
}
