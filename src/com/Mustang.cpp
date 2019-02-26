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

#include "com/Mustang.h"
#include "com/PacketSerializer.h"
#include "com/CommunicationException.h"
#include <algorithm>

namespace plug::com
{
    namespace
    {
        inline constexpr std::uint8_t endpointSend{0x01};
        inline constexpr std::uint8_t endpointRecv{0x81};
    }

    Mustang::Mustang(std::shared_ptr<Connection> connection)
        : comm(connection)
    {
    }

    InitalData Mustang::start_amp()
    {
        if (comm->isOpen() == false)
        {
            throw CommunicationException{"Device not connected"};
        }

        initializeAmp();

        return loadData();
    }

    void Mustang::stop_amp()
    {
        comm->close();
    }

    void Mustang::set_effect(fx_pedal_settings value)
    {
        const auto clearEffectPacket = serializeClearEffectSettings();
        sendCommand(clearEffectPacket);
        sendApplyCommand();

        if (value.effect_num != effects::EMPTY)
        {
            const auto settingsPacket = serializeEffectSettings(value);
            sendCommand(settingsPacket);
            sendApplyCommand();
        }
    }

    void Mustang::set_amplifier(amp_settings value)
    {
        const auto settingsPacket = serializeAmpSettings(value);
        sendCommand(settingsPacket);
        sendApplyCommand();

        const auto settingsGainPacket = serializeAmpSettingsUsbGain(value);
        sendCommand(settingsGainPacket);
        sendApplyCommand();
    }

    void Mustang::save_on_amp(std::string_view name, std::uint8_t slot)
    {
        const auto data = serializeName(slot, name);
        sendCommand(data);
        loadBankData(slot);
    }

    SignalChain Mustang::load_memory_bank(std::uint8_t slot)
    {
        return decode_data(loadBankData(slot));
    }

    SignalChain Mustang::decode_data(const std::array<Packet, 7>& data)
    {
        const auto name = decodeNameFromData(data);
        const auto amp = decodeAmpFromData(data);
        const auto effects = decodeEffectsFromData(data);

        return SignalChain{name, amp, effects};
    }

    void Mustang::save_effects(std::uint8_t slot, std::string_view name, const std::vector<fx_pedal_settings>& effects)
    {
        const auto saveNamePacket = serializeSaveEffectName(slot, name, effects);
        sendCommand(saveNamePacket);

        const auto packets = serializeSaveEffectPacket(slot, effects);
        std::for_each(packets.cbegin(), packets.cend(), [this](const auto& p) { sendCommand(p); });

        sendCommand(serializeApplyCommand(effects[0]));
    }

    InitalData Mustang::loadData()
    {
        std::vector<Packet> recieved_data;

        const auto loadCommand = serializeLoadCommand();
        auto recieved = sendPacket(loadCommand);

        while (recieved != 0)
        {
            const auto recvData = receivePacket();
            recieved = recvData.size();
            Packet p{};
            std::copy(recvData.cbegin(), recvData.cend(), p.begin());
            recieved_data.push_back(p);
        }

        const std::size_t max_to_receive = (recieved_data.size() > 143 ? 200 : 48);
        auto presetNames = decodePresetListFromData(recieved_data);

        std::array<Packet, 7> presetData{{}};
        std::copy(std::next(recieved_data.cbegin(), max_to_receive), std::next(recieved_data.cbegin(), max_to_receive + 7), presetData.begin());

        return {decode_data(presetData), presetNames};
    }

    std::array<Packet, 7> Mustang::loadBankData(std::uint8_t slot)
    {
        std::array<Packet, 7> data{{}};

        const auto loadCommand = serializeLoadSlotCommand(slot);
        auto n = sendPacket(loadCommand);

        for (std::size_t i = 0; n != 0; ++i)
        {
            const auto recvData = receivePacket();
            n = recvData.size();

            if (i < 7)
            {
                std::copy(recvData.cbegin(), recvData.cend(), data[i].begin());
            }
        }
        return data;
    }

    void Mustang::initializeAmp()
    {
        const auto packets = serializeInitCommand();
        std::for_each(packets.cbegin(), packets.cend(), [this](const auto& p) { sendCommand(p); });
    }

    std::size_t Mustang::sendPacket(const Packet& packet)
    {
        return comm->send(endpointSend, packet);
    }

    std::vector<std::uint8_t> Mustang::receivePacket()
    {
        return comm->receive(endpointRecv, packetSize);
    }

    void Mustang::sendCommand(const Packet& packet)
    {
        sendPacket(packet);
        receivePacket();
    }

    void Mustang::sendApplyCommand()
    {
        sendCommand(serializeApplyCommand());
    }
}
