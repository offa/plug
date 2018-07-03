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

#pragma once

#include "data_structs.h"
#include "effects_enum.h"
#include "com/MustangConstants.h"
#include "com/Packet.h"
#include <string>
#include <vector>
#include <cstdint>

namespace plug::com
{
    std::string decodeNameFromData(const unsigned char data[7][64]);
    amp_settings decodeAmpFromData(const unsigned char data[7][64]);
    void decodeEffectsFromData(const unsigned char data[7][64], fx_pedal_settings* const& effects_set_out);

    Packet serializeAmpSettings(const amp_settings& value);
    Packet serializeAmpSettingsUsbGain(const amp_settings& value);
    Packet serializeName(std::uint8_t slot, std::string_view name);
    Packet serializeEffectSettings(const fx_pedal_settings& value);
    Packet serializeClearEffectSettings();
    Packet serializeSaveEffectName(int slot, std::string_view name, const std::vector<fx_pedal_settings>& effects);
    std::vector<Packet> serializeSaveEffectPacket(int slot, const std::vector<fx_pedal_settings>& effects);

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

    constexpr std::uint8_t getFxKnob(const fx_pedal_settings& effect)
    {
        if (effect.effect_num >= value(effects::SINE_CHORUS) && effect.effect_num <= value(effects::PITCH_SHIFTER))
        {
            return 0x01;
        }
        return 0x02;
    }
}
