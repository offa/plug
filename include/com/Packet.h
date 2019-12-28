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
#include <string>
#include <string_view>

namespace plug::com
{
    inline namespace v2
    {
        constexpr std::size_t sizeHeader{16};
        constexpr std::size_t sizePayload{48};

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
            opSaveEffectName,
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
            using RawType = std::array<std::uint8_t, sizeHeader>;


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

            Stage getStage() const
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

            Type getType() const
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
                        case DSP::opSaveEffectName:
                            return 0x04;
                        case DSP::opSelectMemBank:
                            return 0x01;
                        default:
                            return 0xff;
                    }
                }();
            }

            DSP getDSP() const
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

            void setSlot(std::uint8_t slot)
            {
                bytes[4] = slot;
            }

            std::uint8_t getSlot() const
            {
                return bytes[4];
            }

            void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
            {
                bytes[3] = value0;
                bytes[6] = value1;
                bytes[7] = value2;
            }

            RawType getBytes() const
            {
                return bytes;
            }

            void fromBytes(const RawType& data)
            {
                bytes = data;
            }

        private:
            RawType bytes{{}};
        };


        class EmptyPayload
        {
        public:
            using RawType = std::array<std::uint8_t, sizePayload>;


            RawType getBytes() const
            {
                return bytes;
            }

        private:
            RawType bytes{{}};
        };

        class NamePayload
        {
        public:
            using RawType = std::array<std::uint8_t, sizePayload>;


            void setName(std::string_view name)
            {
                constexpr std::size_t nameLength{32};
                const auto n = std::min(name.length(), nameLength);
                std::copy_n(name.cbegin(), n, bytes.begin());
            }

            std::string getName() const
            {
                constexpr std::size_t nameLength{32};
                const auto end = std::find(bytes.cbegin(), bytes.cend(), '\0');
                const auto maxEnd = std::next(bytes.cbegin(), nameLength);

                return std::string(bytes.cbegin(), std::min(end, maxEnd));
            }

            RawType getBytes() const
            {
                return bytes;
            }

            void fromBytes(const RawType& data)
            {
                bytes = data;
            }

        private:
            RawType bytes{{}};
        };

        class EffectPayload
        {
        public:
            using RawType = std::array<std::uint8_t, sizePayload>;


            void setKnob1(std::uint8_t value)
            {
                bytes[16] = value;
            }

            std::uint8_t getKnob1() const
            {
                return bytes[16];
            }

            void setKnob2(std::uint8_t value)
            {
                bytes[17] = value;
            }

            std::uint8_t getKnob2() const
            {
                return bytes[17];
            }

            void setKnob3(std::uint8_t value)
            {
                bytes[18] = value;
            }

            std::uint8_t getKnob3() const
            {
                return bytes[18];
            }

            void setKnob4(std::uint8_t value)
            {
                bytes[19] = value;
            }

            std::uint8_t getKnob4() const
            {
                return bytes[19];
            }

            void setKnob5(std::uint8_t value)
            {
                bytes[20] = value;
            }

            std::uint8_t getKnob5() const
            {
                return bytes[20];
            }

            void setKnob6(std::uint8_t value)
            {
                bytes[21] = value;
            }

            std::uint8_t getKnob6() const
            {
                return bytes[21];
            }

            void setSlot(std::uint8_t slot)
            {
                bytes[2] = slot;
            }

            std::uint8_t getSlot() const
            {
                return bytes[2];
            }

            void setModel(std::uint8_t model)
            {
                bytes[0] = model;
            }

            std::uint8_t getModel() const
            {
                return bytes[0];
            }

            void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
            {
                bytes[3] = value0;
                bytes[4] = value1;
                bytes[5] = value2;
            }

            RawType getBytes() const
            {
                return bytes;
            }

            void fromBytes(const RawType& data)
            {
                bytes = data;
            }

        private:
            RawType bytes{{}};
        };


        class AmpPayload
        {
        public:
            using RawType = std::array<std::uint8_t, sizePayload>;


            AmpPayload()
            {
            }

            void setModel(std::uint8_t value)
            {
                bytes[0] = value;
            }

            std::uint8_t getModel() const
            {
                return bytes[0];
            }

            void setVolume(std::uint8_t value)
            {
                bytes[16] = value;
            }

            std::uint8_t getVolume() const
            {
                return bytes[16];
            }

            void setGain(std::uint8_t value)
            {
                bytes[17] = value;
            }

            std::uint8_t getGain() const
            {
                return bytes[17];
            }

            void setGain2(std::uint8_t value)
            {
                bytes[18] = value;
            }

            std::uint8_t getGain2() const
            {
                return bytes[18];
            }

            void setMasterVolume(std::uint8_t value)
            {
                bytes[19] = value;
            }

            std::uint8_t getMasterVolume() const
            {
                return bytes[19];
            }

            void setTreble(std::uint8_t value)
            {
                bytes[20] = value;
            }

            std::uint8_t getTreble() const
            {
                return bytes[20];
            }

            void setMiddle(std::uint8_t value)
            {
                bytes[21] = value;
            }

            std::uint8_t getMiddle() const
            {
                return bytes[21];
            }

            void setBass(std::uint8_t value)
            {
                bytes[22] = value;
            }

            std::uint8_t getBass() const
            {
                return bytes[22];
            }

            void setPresence(std::uint8_t value)
            {
                bytes[23] = value;
            }

            std::uint8_t getPresence() const
            {
                return bytes[23];
            }

            void setDepth(std::uint8_t value)
            {
                bytes[25] = value;
            }

            std::uint8_t getDepth() const
            {
                return bytes[25];
            }

            void setBias(std::uint8_t value)
            {
                bytes[26] = value;
            }

            std::uint8_t getBias() const
            {
                return bytes[26];
            }

            void setNoiseGate(std::uint8_t value)
            {
                bytes[31] = value;
            }

            std::uint8_t getNoiseGate() const
            {
                return bytes[31];
            }

            void setThreshold(std::uint8_t value)
            {
                bytes[32] = value;
            }

            std::uint8_t getThreshold() const
            {
                return bytes[32];
            }

            void setCabinet(std::uint8_t value)
            {
                bytes[33] = value;
            }

            std::uint8_t getCabinet() const
            {
                return bytes[33];
            }

            void setSag(std::uint8_t value)
            {
                bytes[35] = value;
            }

            std::uint8_t getSag() const
            {
                return bytes[35];
            }

            void setBrightness(std::uint8_t value)
            {
                bytes[36] = value;
            }

            std::uint8_t getBrightness() const
            {
                return bytes[36];
            }

            void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2)
            {
                bytes[24] = value0;
                bytes[27] = value1;
                bytes[37] = value2;
            }

            void setUnknownAmpSpecific(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2, std::uint8_t value3, std::uint8_t value4)
            {
                bytes[28] = value0;
                bytes[29] = value1;
                bytes[30] = value2;
                bytes[34] = value3;
                bytes[38] = value4;
            }

            void setUsbGain(std::uint8_t value)
            {
                bytes[0] = value;
            }

            std::uint8_t getUsbGain() const
            {
                return bytes[0];
            }

            RawType getBytes() const
            {
                return bytes;
            }

            void fromBytes(const RawType& data)
            {
                bytes = data;
            }


        private:
            RawType bytes{{}};
        };


        constexpr std::size_t packetRawTypeSize = 64;
        using PacketRawType = std::array<std::uint8_t, packetRawTypeSize>;

        template <class Payload>
        class Packet
        {
        public:
            using RawType = PacketRawType;


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

            RawType getBytes() const
            {
                RawType bytes{{}};
                const auto headerBytes = header.getBytes();
                const auto payloadBytes = payload.getBytes();
                const auto itr = std::copy(headerBytes.cbegin(), headerBytes.cend(), bytes.begin());
                std::copy(payloadBytes.cbegin(), payloadBytes.cend(), itr);

                return bytes;
            }

            void fromBytes(const RawType& data)
            {
                typename Header::RawType headerData{{}};
                const auto startOfPayload = std::next(data.cbegin(), headerData.size());
                std::copy(data.cbegin(), startOfPayload, headerData.begin());
                header.fromBytes(headerData);

                typename Payload::RawType payloadData{{}};
                std::copy(startOfPayload, data.cend(), payloadData.begin());
                payload.fromBytes(payloadData);
            }


        private:
            Header header;
            Payload payload;
        };

    }

}
