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

#include "com/mustang.h"
#include "com/UsbComm.h"
#include "com/IdLookup.h"
#include "com/PacketSerializer.h"
#include <cstring>

namespace plug::com
{
    namespace
    {
        inline constexpr std::initializer_list<std::uint16_t> pids{
            SMALL_AMPS_USB_PID,
            BIG_AMPS_USB_PID,
            SMALL_AMPS_V2_USB_PID,
            BIG_AMPS_V2_USB_PID,
            MINI_USB_PID,
            FLOOR_USB_PID};


        inline constexpr std::uint8_t endpointSend{0x01};
        inline constexpr std::uint8_t endpointRecv{0x81};
    }

    Mustang::Mustang()
        : comm(std::make_unique<UsbComm>())
    {
    }

    Mustang::~Mustang()
    {
        this->stop_amp();
    }

    void Mustang::start_amp(char list[][32], char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        if (comm->isOpen() == false)
        {
            comm->openFirst(USB_VID, pids);
        }

        initializeAmp();

        loadInitialData(list, name, amp_set, effects_set);
    }

    void Mustang::stop_amp()
    {
        comm->close();
    }

    void Mustang::set_effect(fx_pedal_settings value)
    {
        const auto clearEffectPacket = serializeClearEffectSettings();
        sendPacket(clearEffectPacket);
        receivePacket();
        sendApplyCommand();

        if (static_cast<effects>(value.effect_num) == effects::EMPTY)
        {
            return;
        }

        const auto settingsPacket = serializeEffectSettings(value);
        sendPacket(settingsPacket);
        receivePacket();
        sendApplyCommand();
    }

    void Mustang::set_amplifier(amp_settings value)
    {
        const auto settingsPacket = serializeAmpSettings(value);
        sendPacket(settingsPacket);
        receivePacket();
        sendApplyCommand();

        const auto settingsGainPacket = serializeAmpSettingsUsbGain(value);
        sendPacket(settingsGainPacket);
        receivePacket();
        sendApplyCommand();
    }

    void Mustang::save_on_amp(std::string_view name, std::uint8_t slot)
    {
        const auto data = serializeName(slot, name);
        sendPacket(data);
        receivePacket();
        load_memory_bank(slot, nullptr, nullptr, nullptr);
    }

    void Mustang::load_memory_bank(std::uint8_t slot, char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        Packet array{};
        unsigned char data[7][packetSize];

        array.fill(0x00);
        array[0] = 0x1c;
        array[1] = 0x01;
        array[2] = 0x01;
        array[SAVE_SLOT] = slot;
        array[6] = 0x01;

        auto n = sendPacket(array);

        for (int i = 0; n != 0; ++i)
        {
            const auto recvData = receivePacket();
            n = recvData.size();

            if (i < 7)
            {
                std::copy(recvData.cbegin(), recvData.cend(), data[i]);
            }
        }

        if (name != nullptr || amp_set != nullptr || effects_set != nullptr)
        {
            decode_data(data, name, amp_set, effects_set);
        }
    }

    void Mustang::decode_data(unsigned char data[7][packetSize], char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        if (name != nullptr)
        {
            const std::string nameDecoded = decodeNameFromData(data);
            std::copy(nameDecoded.cbegin(), nameDecoded.cend(), name);
        }

        if (amp_set != nullptr)
        {
            *amp_set = decodeAmpFromData(data);
        }

        if (effects_set != nullptr)
        {
            decodeEffectsFromData(data, effects_set);
        }
    }

    void Mustang::save_effects(std::uint8_t slot, std::string_view name, const std::vector<fx_pedal_settings>& effects)
    {
        const auto saveNamePacket = serializeSaveEffectName(slot, name, effects);
        sendPacket(saveNamePacket);
        receivePacket();

        const auto packets = serializeSaveEffectPacket(slot, effects);

        for (const auto& packet : packets)
        {
            sendPacket(packet);
            receivePacket();
        }

        Packet applyPacket{};
        applyPacket[0] = 0x1c;
        applyPacket[1] = 0x03;
        applyPacket[FXKNOB] = getFxKnob(effects[0]);

        sendPacket(applyPacket);
        receivePacket();
    }

    void Mustang::loadInitialData(char list[][32], char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        if (list != nullptr || name != nullptr || amp_set != nullptr || effects_set != nullptr)
        {
            unsigned char recieved_data[296][packetSize];
            memset(recieved_data, 0x00, 296 * packetSize);

            std::size_t i{0};
            std::size_t j{0};
            Packet array{};
            array.fill(0x00);
            array[0] = 0xff;
            array[1] = 0xc1;
            auto recieved = sendPacket(array);

            for (i = 0; recieved != 0; i++)
            {
                const auto recvData = receivePacket();
                recieved = recvData.size();
                std::copy(recvData.cbegin(), recvData.cend(), recieved_data[i]);
            }

            const std::size_t max_to_receive = (i > 143 ? 200 : 48);
            if (list != nullptr)
            {
                for (i = 0, j = 0; i < max_to_receive; i += 2, ++j)
                {
                    memcpy(list[j], recieved_data[i] + 16, 32);
                }
            }

            if (name != nullptr || amp_set != nullptr || effects_set != nullptr)
            {
                unsigned char data[7][packetSize];

                for (j = 0; j < 7; ++i, ++j)
                {
                    memcpy(data[j], recieved_data[i], packetSize);
                }
                decode_data(data, name, amp_set, effects_set);
            }
        }
    }

    void Mustang::initializeAmp()
    {
        // initialization which is needed if you want
        // to get any replies from the amp in the future
        Packet initPacket1{};
        initPacket1[1] = 0xc3;
        sendPacket(initPacket1);
        receivePacket();

        Packet initPacket2{};
        initPacket2[0] = 0x1a;
        initPacket2[1] = 0x03;
        sendPacket(initPacket2);
        receivePacket();
    }

    std::size_t Mustang::sendPacket(const Packet& packet)
    {
        return comm->interruptWrite(endpointSend, packet);
    }

    std::vector<std::uint8_t> Mustang::receivePacket()
    {
        return comm->interruptReceive(endpointRecv, packetSize);
    }

    void Mustang::sendApplyCommand()
    {
        Packet apply{};
        apply[0] = 0x1c;
        apply[1] = 0x03;

        sendPacket(apply);
        receivePacket();
    }
}
