/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2019  offa
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

#include <array>
#include <algorithm>

namespace plug::com
{
namespace v2
{
    constexpr std::size_t sizeTotal{64};
    constexpr std::size_t sizeHeader{16};
    constexpr std::size_t sizePayload{sizeTotal - sizeHeader};

    enum class DSP
    {
        none,
        amp,
        usbGain,
        effect0,
        effect1,
        effect2,
        effect3,
        opSave,
        opSelectMemBank
    };

    enum class Type
    {
        operation,
        data,
        init0,
        init1,
        load
    };

    enum class Stage
    {
        init0,
        init1,
        ready,
        unknown
    };

    class Header
    {
    public:
        Header()
        {
        }

        void setStage(Stage stage)
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

        void setType(Type type)
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

        void setDSP(DSP dsp)
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
                    case DSP::opSelectMemBank:
                        return 0x01;
                    default:
                        return 0xff;
                }
            }();
        }

        void setSlot(std::uint8_t slot)
        {
            bytes[4] = slot;
        }

        void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
        {
            bytes[3] = value0;
            bytes[6] = value1;
            bytes[7] = value2;
        }

        std::array<std::uint8_t, sizeHeader> getBytes() const
        {
            return bytes;
        }

    private:
        std::array<std::uint8_t, sizeHeader> bytes{{}};
    };


    class EmptyPayload
    {
    public:
        std::array<std::uint8_t, sizePayload> getBytes() const
        {
            return bytes;
        }

    private:
        std::array<std::uint8_t, sizePayload> bytes{{}};
    };

    class NamePayload
    {
    public:
        void setName(std::string_view name)
        {
            constexpr std::size_t nameLength{32};
            const auto n = std::min(name.length(), nameLength);
            std::copy_n(name.cbegin(), n, bytes.begin());
        }

        std::array<std::uint8_t, sizePayload> getBytes() const
        {
            return bytes;
        }

    private:
        std::array<std::uint8_t, sizePayload> bytes{{}};
    };

    class EffectPayload
    {
    public:
        void setKnob1(std::uint8_t value)
        {
            bytes[16] = value;
        }

        void setKnob2(std::uint8_t value)
        {
            bytes[17] = value;
        }

        void setKnob3(std::uint8_t value)
        {
            bytes[18] = value;
        }

        void setKnob4(std::uint8_t value)
        {
            bytes[19] = value;
        }

        void setKnob5(std::uint8_t value)
        {
            bytes[20] = value;
        }

        void setKnob6(std::uint8_t value)
        {
            bytes[21] = value;
        }

        void setSlot(std::uint8_t slot)
        {
            bytes[2] = slot;
        }

        void setModel(std::uint8_t model)
        {
            bytes[0] = model;
        }

        void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
        {
            bytes[3] = value0;
            bytes[4] = value1;
            bytes[5] = value2;
        }

        std::array<std::uint8_t, sizePayload> getBytes() const
        {
            return bytes;
        }

    private:
        std::array<std::uint8_t, sizePayload> bytes{{}};
    };


    class AmpPayload
    {
    public:
        AmpPayload()
        {
        }

        void setModel(std::uint8_t value)
        {
            bytes[0] = value;
        }

        void setVolume(std::uint8_t value)
        {
            bytes[16] = value;
        }

        void setGain(std::uint8_t value)
        {
            bytes[17] = value;
        }

        void setGain2(std::uint8_t value)
        {
            bytes[18] = value;
        }

        void setMasterVolume(std::uint8_t value)
        {
            bytes[19] = value;
        }

        void setTreble(std::uint8_t value)
        {
            bytes[20] = value;
        }

        void setMiddle(std::uint8_t value)
        {
            bytes[21] = value;
        }

        void setBass(std::uint8_t value)
        {
            bytes[22] = value;
        }

        void setPresence(std::uint8_t value)
        {
            bytes[23] = value;
        }

        void setDepth(std::uint8_t value)
        {
            bytes[25] = value;
        }

        void setBias(std::uint8_t value)
        {
            bytes[26] = value;
        }

        void setNoiseGate(std::uint8_t value)
        {
            bytes[31] = value;
        }

        void setThreshold(std::uint8_t value)
        {
            bytes[32] = value;
        }

        void setCabinet(std::uint8_t value)
        {
            bytes[33] = value;
        }

        void setSag(std::uint8_t value)
        {
            bytes[35] = value;
        }

        void setBrightness(std::uint8_t value)
        {
            bytes[36] = value;
        }

        void setUnknown(std::uint8_t value)
        {
            bytes[37] = value;
        }

        void setUsbGain(std::uint8_t value)
        {
            bytes[0] = value;
        }

        std::array<std::uint8_t, sizePayload> getBytes() const
        {
            return bytes;
        }

    private:
        std::array<std::uint8_t, sizePayload> bytes{{}};
    };

    template <class Payload>
    class Packet
    {
    public:
        void setHeader(Header h)
        {
            header = h;
        }

        const Header& getHeader() const
        {
            return header;
        }

        void setPayload(Payload p)
        {
            payload = p;
        }

        const Payload& getPayload() const
        {
            return payload;
        }

        std::array<std::uint8_t, sizeTotal> getBytes() const
        {
            std::array<std::uint8_t, sizeTotal> bytes{{}};
            const auto headerBytes = header.getBytes();
            const auto payloadBytes = payload.getBytes();
            const auto itr = std::copy(headerBytes.cbegin(), headerBytes.cend(), bytes.begin());
            std::copy(payloadBytes.cbegin(), payloadBytes.cend(), itr);

            return bytes;
        }

    private:
        Header header;
        Payload payload;
    };

}
    constexpr std::size_t packetSize{64};
    using Packet = std::array<std::uint8_t, packetSize>;

}
