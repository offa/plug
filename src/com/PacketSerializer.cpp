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
    std::string decodeNameFromData(const unsigned char data[7][64])
    {
        constexpr std::size_t nameLength{32};
        std::string name(nameLength, '\0');

        for (int i = 0, j = 16; data[0][j] != 0x00; ++i, ++j)
        {
            name[i] = data[0][j];
        }

        name.resize(nameLength);
        return name;
    }

    amp_settings decodeAmpFromData(const unsigned char data[7][64])
    {
        amp_settings settings{};
        settings.amp_num = value(lookupAmpById(data[1][AMPLIFIER]));
        settings.gain = data[1][GAIN];
        settings.volume = data[1][VOLUME];
        settings.treble = data[1][TREBLE];
        settings.middle = data[1][MIDDLE];
        settings.bass = data[1][BASS];
        settings.cabinet = data[1][CABINET];
        settings.noise_gate = data[1][NOISE_GATE];
        settings.master_vol = data[1][MASTER_VOL];
        settings.gain2 = data[1][GAIN2];
        settings.presence = data[1][PRESENCE];
        settings.threshold = data[1][THRESHOLD];
        settings.depth = data[1][DEPTH];
        settings.bias = data[1][BIAS];
        settings.sag = data[1][SAG];
        settings.brightness = data[1][BRIGHTNESS] != 0u;
        settings.usb_gain = data[6][16];
        return settings;
    }

    void decodeEffectsFromData(unsigned char prev_array[4][packetSize], const unsigned char data[7][64], fx_pedal_settings* const& effects_set_out)
    {
        for (int i = 2; i < 6; ++i)
        {
            int j = 0;

            prev_array[data[i][DSP] - 6][0] = 0x1c;
            prev_array[data[i][DSP] - 6][1] = 0x03;
            prev_array[data[i][DSP] - 6][FXSLOT] = data[i][FXSLOT];
            prev_array[data[i][DSP] - 6][DSP] = data[i][DSP];
            prev_array[data[i][DSP] - 6][19] = data[i][19];
            prev_array[data[i][DSP] - 6][20] = data[i][20];

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
            effects_set_out[j].put_post_amp = (data[i][FXSLOT] > 0x03);
            effects_set_out[j].effect_num = value(lookupEffectById(data[i][EFFECT]));
        }
    }

    std::array<std::uint8_t, packetSize> serializeAmpSettings(const amp_settings& value)
    {
        std::array<std::uint8_t, packetSize> packet{{0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
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

        if (value.cabinet > 0x0c)
        {
            packet[CABINET] = 0x00;
        }
        else
        {
            packet[CABINET] = value.cabinet;
        }

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

        switch (static_cast<amps>(value.amp_num))
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

    std::array<std::uint8_t, packetSize> serializeAmpSettingsUsbGain(const amp_settings& value)
    {
        std::array<std::uint8_t, packetSize> packet{{0}};
        packet.fill(0x00);
        packet[0] = 0x1c;
        packet[1] = 0x03;
        packet[2] = 0x0d;
        packet[6] = 0x01;
        packet[7] = 0x01;
        packet[16] = value.usb_gain;
        return packet;
    }

    std::array<std::uint8_t, packetSize> serializeName(std::uint8_t slot, std::string_view name)
    {
        std::array<std::uint8_t, packetSize> data;
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
        std::copy(sizedName.cbegin(), std::next(sizedName.cend()), std::next(data.data(), 16));
        return data;
    }
}
