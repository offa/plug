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
#include <stdexcept>

namespace plug::com
{
    void Header::setStage(Stage stage)
    {
        bytes[0] = [stage]() -> std::uint8_t {
            switch (stage)
            {
                case Stage::init0:
                    return 0x00;
                case Stage::init1:
                    return 0x1a;
                case Stage::ready:
                    return 0x1c;
                default:
                    return 0xff;
            }
        }();
    }

    Stage Header::getStage() const
    {
        switch (bytes[0])
        {
            case 0x00:
                return Stage::init0;
            case 0x1a:
                return Stage::init1;
            case 0x1c:
                return Stage::ready;
            default:
                return Stage::unknown;
        }
    }

    void Header::setType(Type type)
    {
        bytes[1] = [type]() -> std::uint8_t {
            switch (type)
            {
                case Type::operation:
                    return 0x01;
                case Type::data:
                    return 0x03;
                case Type::init0:
                    return 0xc3;
                case Type::init1:
                    return 0x03;
                case Type::load:
                    return 0xc1;
                default:
                    return 0xff;
            }
        }();
    }

    Type Header::getType() const
    {
        switch (bytes[1])
        {
            case 0x01:
                return Type::operation;
            case 0x03:
                return Type::data; // Same value as Type::init1
            case 0xc3:
                return Type::init0;
            case 0xc1:
                return Type::load;
            default:
                throw std::domain_error("Invalid Type: " + std::to_string(bytes[1]));
        }
    }

    void Header::setDSP(DSP dsp)
    {
        bytes[2] = [dsp]() -> std::uint8_t {
            switch (dsp)
            {
                case DSP::none:
                    return 0x00;
                case DSP::amp:
                    return 0x05;
                case DSP::usbGain:
                    return 0x0d;
                case DSP::effect0:
                    return 0x06;
                case DSP::effect1:
                    return 0x07;
                case DSP::effect2:
                    return 0x08;
                case DSP::effect3:
                    return 0x09;
                case DSP::opSave:
                    return 0x03;
                case DSP::opSaveEffectName:
                    return 0x04;
                case DSP::opSelectMemBank:
                    return 0x01;
                default:
                    return 0xff;
            }
        }();
    }

    DSP Header::getDSP() const
    {
        switch (bytes[2])
        {
            case 0x00:
                return DSP::none;
            case 0x05:
                return DSP::amp;
            case 0x0d:
                return DSP::usbGain;
            case 0x06:
                return DSP::effect0;
            case 0x07:
                return DSP::effect1;
            case 0x08:
                return DSP::effect2;
            case 0x09:
                return DSP::effect3;
            case 0x03:
                return DSP::opSave;
            case 0x04:
                return DSP::opSaveEffectName;
            case 0x01:
                return DSP::opSelectMemBank;
            default:
                throw std::domain_error("Invalid DSP: " + std::to_string(bytes[2]));
        }
    }

    void Header::setSlot(std::uint8_t slot)
    {
        bytes[4] = slot;
    }

    std::uint8_t Header::getSlot() const
    {
        return bytes[4];
    }

    void Header::setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
    {
        bytes[3] = value0;
        bytes[6] = value1;
        bytes[7] = value2;
    }

    Header::RawType Header::getBytes() const
    {
        return bytes;
    }

    void Header::fromBytes(const RawType& data)
    {
        bytes = data;
    }


    PayloadBase::RawType PayloadBase::getBytes() const
    {
        return bytes;
    }

    void PayloadBase::fromBytes(const PayloadBase::RawType& data)
    {
        bytes = data;
    }


    void NamePayload::setName(std::string_view name)
    {
        constexpr std::size_t nameLength{32};
        const auto n = std::min(name.length(), nameLength);
        std::copy_n(name.cbegin(), n, bytes.begin());
    }

    std::string NamePayload::getName() const
    {
        constexpr std::size_t nameLength{32};
        const auto end = std::find(bytes.cbegin(), bytes.cend(), '\0');
        const auto maxEnd = std::next(bytes.cbegin(), nameLength);

        return std::string(bytes.cbegin(), std::min(end, maxEnd));
    }


    void EffectPayload::setKnob1(std::uint8_t value)
    {
        bytes[16] = value;
    }

    std::uint8_t EffectPayload::getKnob1() const
    {
        return bytes[16];
    }

    void EffectPayload::setKnob2(std::uint8_t value)
    {
        bytes[17] = value;
    }

    std::uint8_t EffectPayload::getKnob2() const
    {
        return bytes[17];
    }

    void EffectPayload::setKnob3(std::uint8_t value)
    {
        bytes[18] = value;
    }

    std::uint8_t EffectPayload::getKnob3() const
    {
        return bytes[18];
    }

    void EffectPayload::setKnob4(std::uint8_t value)
    {
        bytes[19] = value;
    }

    std::uint8_t EffectPayload::getKnob4() const
    {
        return bytes[19];
    }

    void EffectPayload::setKnob5(std::uint8_t value)
    {
        bytes[20] = value;
    }

    std::uint8_t EffectPayload::getKnob5() const
    {
        return bytes[20];
    }

    void EffectPayload::setKnob6(std::uint8_t value)
    {
        bytes[21] = value;
    }

    std::uint8_t EffectPayload::getKnob6() const
    {
        return bytes[21];
    }

    void EffectPayload::setSlot(std::uint8_t slot)
    {
        bytes[2] = slot;
    }

    std::uint8_t EffectPayload::getSlot() const
    {
        return bytes[2];
    }

    void EffectPayload::setModel(std::uint8_t model)
    {
        bytes[0] = model;
    }

    std::uint8_t EffectPayload::getModel() const
    {
        return bytes[0];
    }

    void EffectPayload::setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
    {
        bytes[3] = value0;
        bytes[4] = value1;
        bytes[5] = value2;
    }


    void AmpPayload::setModel(std::uint8_t value)
    {
        bytes[0] = value;
    }

    std::uint8_t AmpPayload::getModel() const
    {
        return bytes[0];
    }

    void AmpPayload::setVolume(std::uint8_t value)
    {
        bytes[16] = value;
    }

    std::uint8_t AmpPayload::getVolume() const
    {
        return bytes[16];
    }

    void AmpPayload::setGain(std::uint8_t value)
    {
        bytes[17] = value;
    }

    std::uint8_t AmpPayload::getGain() const
    {
        return bytes[17];
    }

    void AmpPayload::setGain2(std::uint8_t value)
    {
        bytes[18] = value;
    }

    std::uint8_t AmpPayload::getGain2() const
    {
        return bytes[18];
    }

    void AmpPayload::setMasterVolume(std::uint8_t value)
    {
        bytes[19] = value;
    }

    std::uint8_t AmpPayload::getMasterVolume() const
    {
        return bytes[19];
    }

    void AmpPayload::setTreble(std::uint8_t value)
    {
        bytes[20] = value;
    }

    std::uint8_t AmpPayload::getTreble() const
    {
        return bytes[20];
    }

    void AmpPayload::setMiddle(std::uint8_t value)
    {
        bytes[21] = value;
    }

    std::uint8_t AmpPayload::getMiddle() const
    {
        return bytes[21];
    }

    void AmpPayload::setBass(std::uint8_t value)
    {
        bytes[22] = value;
    }

    std::uint8_t AmpPayload::getBass() const
    {
        return bytes[22];
    }

    void AmpPayload::setPresence(std::uint8_t value)
    {
        bytes[23] = value;
    }

    std::uint8_t AmpPayload::getPresence() const
    {
        return bytes[23];
    }

    void AmpPayload::setDepth(std::uint8_t value)
    {
        bytes[25] = value;
    }

    std::uint8_t AmpPayload::getDepth() const
    {
        return bytes[25];
    }

    void AmpPayload::setBias(std::uint8_t value)
    {
        bytes[26] = value;
    }

    std::uint8_t AmpPayload::getBias() const
    {
        return bytes[26];
    }

    void AmpPayload::setNoiseGate(std::uint8_t value)
    {
        bytes[31] = value;
    }

    std::uint8_t AmpPayload::getNoiseGate() const
    {
        return bytes[31];
    }

    void AmpPayload::setThreshold(std::uint8_t value)
    {
        bytes[32] = value;
    }

    std::uint8_t AmpPayload::getThreshold() const
    {
        return bytes[32];
    }

    void AmpPayload::setCabinet(std::uint8_t value)
    {
        bytes[33] = value;
    }

    std::uint8_t AmpPayload::getCabinet() const
    {
        return bytes[33];
    }

    void AmpPayload::setSag(std::uint8_t value)
    {
        bytes[35] = value;
    }

    std::uint8_t AmpPayload::getSag() const
    {
        return bytes[35];
    }

    void AmpPayload::setBrightness(std::uint8_t value)
    {
        bytes[36] = value;
    }

    std::uint8_t AmpPayload::getBrightness() const
    {
        return bytes[36];
    }

    void AmpPayload::setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
    {
        bytes[24] = value0;
        bytes[27] = value1;
        bytes[37] = value2;
    }

    void AmpPayload::setUnknownAmpSpecific(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2, std::uint8_t value3, std::uint8_t value4)
    {
        bytes[28] = value0;
        bytes[29] = value1;
        bytes[30] = value2;
        bytes[34] = value3;
        bytes[38] = value4;
    }

    void AmpPayload::setUsbGain(std::uint8_t value)
    {
        bytes[0] = value;
    }

    std::uint8_t AmpPayload::getUsbGain() const
    {
        return bytes[0];
    }
}
