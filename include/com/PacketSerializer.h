/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2021  offa
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
#include "com/Packet.h"
#include <string>
#include <vector>
#include <array>
#include <cstdint>

namespace plug::com
{
    template <class T>
    Packet<T> fromRawData(const std::array<std::uint8_t, 64>& data)
    {
        Packet<T> packet{};
        packet.fromBytes(data);
        return packet;
    }

    std::string decodeNameFromData(const Packet<NamePayload>& packet);
    amp_settings decodeAmpFromData(const Packet<AmpPayload>& packet, const Packet<AmpPayload>& packetUsbGain);

    std::array<fx_pedal_settings, 4> decodeEffectsFromData(const std::array<Packet<EffectPayload>, 4>& packet);
    std::vector<std::string> decodePresetListFromData(const std::vector<Packet<NamePayload>>& packet);

    Packet<AmpPayload> serializeAmpSettings(const amp_settings& value);
    Packet<AmpPayload> serializeAmpSettingsUsbGain(const amp_settings& value);
    Packet<NamePayload> serializeName(std::uint8_t slot, std::string_view name);
    Packet<EffectPayload> serializeEffectSettings(const fx_pedal_settings& value);
    Packet<EffectPayload> serializeClearEffectSettings(fx_pedal_settings effect);
    Packet<NamePayload> serializeSaveEffectName(std::uint8_t slot, std::string_view name, const std::vector<fx_pedal_settings>& effects);
    std::vector<Packet<EffectPayload>> serializeSaveEffectPacket(std::uint8_t slot, const std::vector<fx_pedal_settings>& effects);

    Packet<EmptyPayload> serializeLoadSlotCommand(std::uint8_t slot);
    Packet<EmptyPayload> serializeLoadCommand();
    Packet<EmptyPayload> serializeApplyCommand();
    Packet<EmptyPayload> serializeApplyCommand(fx_pedal_settings effect);

    std::array<Packet<EmptyPayload>, 2> serializeInitCommand();

}
