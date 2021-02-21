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

#pragma once

#include <array>
#include <algorithm>
#include <string>
#include <string_view>

namespace plug::com
{
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
        using RawType = std::array<std::uint8_t, 16>;

        void setStage(Stage stage);
        Stage getStage() const;

        void setType(Type type);
        Type getType() const;

        void setDSP(DSP dsp);
        DSP getDSP() const;

        void setSlot(std::uint8_t slot);
        std::uint8_t getSlot() const;

        void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2);

        RawType getBytes() const;
        void fromBytes(const RawType& data);

    private:
        RawType bytes{{}};
    };


    class PayloadBase
    {
    public:
        using RawType = std::array<std::uint8_t, 48>;

        RawType getBytes() const;
        void fromBytes(const RawType& data);

    protected:
        RawType bytes{{}};
    };

    class EmptyPayload : public PayloadBase
    {
    public:
    };

    class NamePayload : public PayloadBase
    {
    public:

        void setName(std::string_view name);
        std::string getName() const;
    };

    class EffectPayload : public PayloadBase
    {
    public:

        void setKnob1(std::uint8_t value);
        std::uint8_t getKnob1() const;

        void setKnob2(std::uint8_t value);
        std::uint8_t getKnob2() const;

        void setKnob3(std::uint8_t value);
        std::uint8_t getKnob3() const;

        void setKnob4(std::uint8_t value);
        std::uint8_t getKnob4() const;

        void setKnob5(std::uint8_t value);
        std::uint8_t getKnob5() const;

        void setKnob6(std::uint8_t value);
        std::uint8_t getKnob6() const;

        void setSlot(std::uint8_t slot);
        std::uint8_t getSlot() const;

        void setModel(std::uint8_t model);
        std::uint8_t getModel() const;

        void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2);
    };


    class AmpPayload : public PayloadBase
    {
    public:

        void setModel(std::uint8_t value);
        std::uint8_t getModel() const;

        void setVolume(std::uint8_t value);
        std::uint8_t getVolume() const;

        void setGain(std::uint8_t value);
        std::uint8_t getGain() const;

        void setGain2(std::uint8_t value);
        std::uint8_t getGain2() const;

        void setMasterVolume(std::uint8_t value);
        std::uint8_t getMasterVolume() const;

        void setTreble(std::uint8_t value);
        std::uint8_t getTreble() const;

        void setMiddle(std::uint8_t value);
        std::uint8_t getMiddle() const;

        void setBass(std::uint8_t value);
        std::uint8_t getBass() const;

        void setPresence(std::uint8_t value);
        std::uint8_t getPresence() const;

        void setDepth(std::uint8_t value);
        std::uint8_t getDepth() const;

        void setBias(std::uint8_t value);
        std::uint8_t getBias() const;

        void setNoiseGate(std::uint8_t value);
        std::uint8_t getNoiseGate() const;

        void setThreshold(std::uint8_t value);
        std::uint8_t getThreshold() const;

        void setCabinet(std::uint8_t value);
        std::uint8_t getCabinet() const;

        void setSag(std::uint8_t value);
        std::uint8_t getSag() const;

        void setBrightness(std::uint8_t value);
        std::uint8_t getBrightness() const;

        void setUnknown(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2);
        void setUnknownAmpSpecific(std::uint8_t value0, std::uint8_t value1, std::uint8_t value2, std::uint8_t value3, std::uint8_t value4);

        void setUsbGain(std::uint8_t value);
        std::uint8_t getUsbGain() const;
    };


    constexpr std::size_t packetRawTypeSize = 64;
    using PacketRawType = std::array<std::uint8_t, packetRawTypeSize>;

    template <class Payload>
    class Packet
    {
    public:
        using RawType = PacketRawType;

        Packet(const Header& h, const Payload& p)
            : header(h), payload(p)
        {
        }

        Packet()
        {
        }

        void setHeader(const Header& h)
        {
            header = h;
        }

        const Header& getHeader() const
        {
            return header;
        }

        void setPayload(const Payload& p)
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
