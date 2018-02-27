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

#include "mustang.h"
#include "UsbComm.h"
#include "IdLookup.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace plug
{
    namespace
    {
        // Compatibility function
        auto adapt(const std::uint8_t* data, std::size_t size)
        {
            return std::vector<std::uint8_t>(data, std::next(data, size));
        }

        constexpr bool hasExtraKnob(effects e)
        {
            switch(e)
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


        constexpr std::initializer_list<std::uint16_t> pids{
            SMALL_AMPS_USB_PID,
            BIG_AMPS_USB_PID,
            SMALL_AMPS_V2_USB_PID,
            BIG_AMPS_V2_USB_PID,
            MINI_USB_PID,
            FLOOR_USB_PID};


        constexpr std::uint8_t endpointSend{0x01};
        constexpr std::uint8_t endpointRecv{0x81};
    }

    Mustang::Mustang()
        : comm(std::make_unique<UsbComm>())
    {
        // "apply effect" command
        memset(execute, 0x00, LENGTH);
        execute[0] = 0x1c;
        execute[1] = 0x03;

        memset(prev_array, 0x00, LENGTH * 4);
        for (int i = 0; i < 4; i++)
        {
            prev_array[i][0] = 0x1c;
            prev_array[i][1] = 0x03;
            prev_array[i][6] = prev_array[i][7] = prev_array[i][21] = 0x01;
            prev_array[i][20] = 0x08;
            prev_array[i][FXSLOT] = 0xff;
        }
    }

    Mustang::~Mustang()
    {
        this->stop_amp();
    }

    int Mustang::start_amp(char list[][32], char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        int recieved;
        unsigned char array[LENGTH];
        unsigned char recieved_data[296][LENGTH];
        memset(recieved_data, 0x00, 296 * LENGTH);

        if (comm->isOpen() == false)
        {
            comm->openFirst(USB_VID, pids);
        }

        // initialization which is needed if you want
        // to get any replies from the amp in the future
        memset(array, 0x00, LENGTH);
        array[1] = 0xc3;
        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        memset(array, 0x00, LENGTH);
        array[0] = 0x1a;
        array[1] = 0x03;
        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        if (list != nullptr || name != nullptr || amp_set != nullptr || effects_set != nullptr)
        {
            int i = 0, j = 0;
            memset(array, 0x00, LENGTH);
            array[0] = 0xff;
            array[1] = 0xc1;
            recieved = comm->interruptWrite(endpointSend, adapt(array, LENGTH));

            for (i = 0; recieved != 0; i++)
            {
                const auto recvData = comm->interruptReceive(endpointRecv, LENGTH);
                recieved = recvData.size();
                memcpy(recieved_data[i], recvData.data(), recieved);
            }

            int max_to_receive;
            i > 143 ? max_to_receive = 200 : max_to_receive = 48;
            if (list != nullptr)
            {
                for (i = 0, j = 0; i < max_to_receive; i += 2, j++)
                {
                    memcpy(list[j], recieved_data[i] + 16, 32);
                }
            }

            if (name != nullptr || amp_set != nullptr || effects_set != nullptr)
            {
                unsigned char data[7][LENGTH];

                for (j = 0; j < 7; i++, j++)
                {
                    memcpy(data[j], recieved_data[i], LENGTH);
                }
                decode_data(data, name, amp_set, effects_set);
            }
        }

        return 0;
    }

    void Mustang::stop_amp()
    {
        comm->close();
    }

    int Mustang::set_effect(fx_pedal_settings value)
    {
        unsigned char slot; // where to put the effect
        unsigned char array[LENGTH] = {
            0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        // clear effect on previous DSP before setting a new one
        for (int i = 0; i < 4; i++)
        {
            if (prev_array[i][FXSLOT] == value.fx_slot || prev_array[i][FXSLOT] == (value.fx_slot + 4))
            {
                memcpy(array, prev_array[i], LENGTH);
                prev_array[i][FXSLOT] = 0xff;
                break;
            }
        }
        array[EFFECT] = 0x00;
        array[KNOB1] = 0x00;
        array[KNOB2] = 0x00;
        array[KNOB3] = 0x00;
        array[KNOB4] = 0x00;
        array[KNOB5] = 0x00;
        array[KNOB6] = 0x00;

        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);
        comm->interruptWrite(endpointSend, adapt(execute, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        constexpr int ret{0};
        const auto effectType = static_cast<effects>(value.effect_num);

        if (effectType == effects::EMPTY)
        {
            return ret;
        }

        if (value.put_post_amp) // put effect in a slot after amplifier
        {
            slot = value.fx_slot + 4;
        }
        else
        {
            slot = value.fx_slot;
        }

        // fill the form with data
        array[FXSLOT] = slot;
        array[KNOB1] = value.knob1;
        array[KNOB2] = value.knob2;
        array[KNOB3] = value.knob3;
        array[KNOB4] = value.knob4;
        array[KNOB5] = value.knob5;

        if( hasExtraKnob(effectType) == true )
        {
            array[KNOB6] = value.knob6;
        }

        // fill the form with missing data
        switch (effectType)
        {
            case effects::OVERDRIVE:
                array[DSP] = 0x06;
                array[EFFECT] = 0x3c;
                break;

            case effects::WAH:
                array[DSP] = 0x06;
                array[EFFECT] = 0x49;
                array[19] = 0x01;
                break;

            case effects::TOUCH_WAH:
                array[DSP] = 0x06;
                array[EFFECT] = 0x4a;
                array[19] = 0x01;
                break;

            case effects::FUZZ:
                array[DSP] = 0x06;
                array[EFFECT] = 0x1a;
                break;

            case effects::FUZZ_TOUCH_WAH:
                array[DSP] = 0x06;
                array[EFFECT] = 0x1c;
                break;

            case effects::SIMPLE_COMP:
                array[DSP] = 0x06;
                array[EFFECT] = 0x88;
                array[19] = 0x08;
                if (array[KNOB1] > 0x03)
                {
                    array[KNOB1] = 0x03;
                }
                array[KNOB2] = 0x00;
                array[KNOB3] = 0x00;
                array[KNOB4] = 0x00;
                array[KNOB5] = 0x00;
                break;

            case effects::COMPRESSOR:
                array[DSP] = 0x06;
                array[EFFECT] = 0x07;
                break;

            case effects::SINE_CHORUS:
                array[DSP] = 0x07;
                array[EFFECT] = 0x12;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::TRIANGLE_CHORUS:
                array[DSP] = 0x07;
                array[EFFECT] = 0x13;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::SINE_FLANGER:
                array[DSP] = 0x07;
                array[EFFECT] = 0x18;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::TRIANGLE_FLANGER:
                array[DSP] = 0x07;
                array[EFFECT] = 0x19;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::VIBRATONE:
                array[DSP] = 0x07;
                array[EFFECT] = 0x2d;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::VINTAGE_TREMOLO:
                array[DSP] = 0x07;
                array[EFFECT] = 0x40;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::SINE_TREMOLO:
                array[DSP] = 0x07;
                array[EFFECT] = 0x41;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::RING_MODULATOR:
                array[DSP] = 0x07;
                array[EFFECT] = 0x22;
                array[19] = 0x01;
                if (array[KNOB4] > 0x01)
                {
                    array[KNOB4] = 0x01;
                }
                break;

            case effects::STEP_FILTER:
                array[DSP] = 0x07;
                array[EFFECT] = 0x29;
                array[19] = 0x01;
                array[20] = 0x01;
                break;

            case effects::PHASER:
                array[DSP] = 0x07;
                array[EFFECT] = 0x4f;
                array[19] = 0x01;
                array[20] = 0x01;
                if (array[KNOB5] > 0x01)
                {
                    array[KNOB5] = 0x01;
                }
                break;

            case effects::PITCH_SHIFTER:
                array[DSP] = 0x07;
                array[EFFECT] = 0x1f;
                array[19] = 0x01;
                break;

            case effects::MONO_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x16;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::MONO_ECHO_FILTER:
                array[DSP] = 0x08;
                array[EFFECT] = 0x43;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::STEREO_ECHO_FILTER:
                array[DSP] = 0x08;
                array[EFFECT] = 0x48;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::MULTITAP_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x44;
                array[19] = 0x02;
                array[20] = 0x01;
                if (array[KNOB5] > 0x03)
                {
                    array[KNOB5] = 0x03;
                }
                break;

            case effects::PING_PONG_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x45;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::DUCKING_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x15;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::REVERSE_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x46;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::TAPE_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x2b;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::STEREO_TAPE_DELAY:
                array[DSP] = 0x08;
                array[EFFECT] = 0x2a;
                array[19] = 0x02;
                array[20] = 0x01;
                break;

            case effects::SMALL_HALL_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x24;
                break;

            case effects::LARGE_HALL_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x3a;
                break;

            case effects::SMALL_ROOM_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x26;
                break;

            case effects::LARGE_ROOM_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x3b;
                break;

            case effects::SMALL_PLATE_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x4e;
                break;

            case effects::LARGE_PLATE_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x4b;
                break;

            case effects::AMBIENT_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x4c;
                break;

            case effects::ARENA_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x4d;
                break;

            case effects::FENDER_63_SPRING_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x21;
                break;

            case effects::FENDER_65_SPRING_REVERB:
                array[DSP] = 0x09;
                array[EFFECT] = 0x0b;
                break;

            default:
                break;
        }

        // send packet to the amp
        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);
        comm->interruptWrite(endpointSend, adapt(execute, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        // save current settings
        memcpy(prev_array[array[DSP] - 6], array, LENGTH);

        return ret;
    }

    int Mustang::set_amplifier(amp_settings value)
    {
        unsigned char array[LENGTH] = {
            0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xaa, 0xa2, 0x80, 0x63, 0x99, 0x80, 0xb0, 0x00,
            0x80, 0x80, 0x80, 0x80, 0x07, 0x07, 0x07, 0x05,
            0x00, 0x07, 0x07, 0x01, 0x00, 0x01, 0x5e, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        array[DSP] = 0x05;
        array[GAIN] = value.gain;
        array[VOLUME] = value.volume;
        array[TREBLE] = value.treble;
        array[MIDDLE] = value.middle;
        array[BASS] = value.bass;

        if (value.cabinet > 0x0c)
        {
            array[CABINET] = 0x00;
        }
        else
        {
            array[CABINET] = value.cabinet;
        }

        if (value.noise_gate > 0x05)
        {
            array[NOISE_GATE] = 0x00;
        }
        else
        {
            array[NOISE_GATE] = value.noise_gate;
        }

        array[MASTER_VOL] = value.master_vol;
        array[GAIN2] = value.gain2;
        array[PRESENCE] = value.presence;

        if (value.noise_gate == 0x05)
        {
            if (value.threshold > 0x09)
            {
                array[THRESHOLD] = 0x00;
            }
            else
            {
                array[THRESHOLD] = value.threshold;
            }

            array[DEPTH] = value.depth;
        }
        array[BIAS] = value.bias;

        if (value.sag > 0x02)
        {
            array[SAG] = 0x01;
        }
        else
        {
            array[SAG] = value.sag;
        }

        array[BRIGHTNESS] = value.brightness ? 1 : 0;

        switch (static_cast<amps>(value.amp_num))
        {
            case amps::FENDER_57_DELUXE:
                array[AMPLIFIER] = 0x67;
                array[44] = array[45] = array[46] = 0x01;
                array[50] = 0x01;
                array[54] = 0x53;
                break;

            case amps::FENDER_59_BASSMAN:
                array[AMPLIFIER] = 0x64;
                array[44] = array[45] = array[46] = 0x02;
                array[50] = 0x02;
                array[54] = 0x67;
                break;

            case amps::FENDER_57_CHAMP:
                array[AMPLIFIER] = 0x7c;
                array[44] = array[45] = array[46] = 0x0c;
                array[50] = 0x0c;
                array[54] = 0x00;
                break;

            case amps::FENDER_65_DELUXE_REVERB:
                array[AMPLIFIER] = 0x53;
                array[40] = array[43] = 0x00;
                array[44] = array[45] = array[46] = 0x03;
                array[50] = 0x03;
                array[54] = 0x6a;
                break;

            case amps::FENDER_65_PRINCETON:
                array[AMPLIFIER] = 0x6a;
                array[44] = array[45] = array[46] = 0x04;
                array[50] = 0x04;
                array[54] = 0x61;
                break;

            case amps::FENDER_65_TWIN_REVERB:
                array[AMPLIFIER] = 0x75;
                array[44] = array[45] = array[46] = 0x05;
                array[50] = 0x05;
                array[54] = 0x72;
                break;

            case amps::FENDER_SUPER_SONIC:
                array[AMPLIFIER] = 0x72;
                array[44] = array[45] = array[46] = 0x06;
                array[50] = 0x06;
                array[54] = 0x79;
                break;

            case amps::BRITISH_60S:
                array[AMPLIFIER] = 0x61;
                array[44] = array[45] = array[46] = 0x07;
                array[50] = 0x07;
                array[54] = 0x5e;
                break;

            case amps::BRITISH_70S:
                array[AMPLIFIER] = 0x79;
                array[44] = array[45] = array[46] = 0x0b;
                array[50] = 0x0b;
                array[54] = 0x7c;
                break;

            case amps::BRITISH_80S:
                array[AMPLIFIER] = 0x5e;
                array[44] = array[45] = array[46] = 0x09;
                array[50] = 0x09;
                array[54] = 0x5d;
                break;

            case amps::AMERICAN_90S:
                array[AMPLIFIER] = 0x5d;
                array[44] = array[45] = array[46] = 0x0a;
                array[50] = 0x0a;
                array[54] = 0x6d;
                break;

            case amps::METAL_2000:
                array[AMPLIFIER] = 0x6d;
                array[44] = array[45] = array[46] = 0x08;
                array[50] = 0x08;
                array[54] = 0x75;
                break;
        }

        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);
        comm->interruptWrite(endpointSend, adapt(execute, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        memset(array, 0x00, LENGTH);
        array[0] = 0x1c;
        array[1] = 0x03;
        array[2] = 0x0d;
        array[6] = 0x01;
        array[7] = 0x01;
        array[16] = value.usb_gain;

        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);
        comm->interruptWrite(endpointSend, adapt(execute, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        return 0;
    }

    int Mustang::save_on_amp(char* name, std::uint8_t slot)
    {
        unsigned char array[LENGTH];

        memset(array, 0x00, LENGTH);
        array[0] = 0x1c;
        array[1] = 0x01;
        array[2] = 0x03;
        array[SAVE_SLOT] = slot;
        array[6] = 0x01;
        array[7] = 0x01;

        if (strlen(name) > 31)
        {
            name[31] = 0x00;
        }

        for (unsigned int i = 16, j = 0; name[j] != 0x00; i++, j++)
        {
            array[i] = name[j];
        }

        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);
        load_memory_bank(slot, nullptr, nullptr, nullptr);

        return 0;
    }

    int Mustang::load_memory_bank(int slot, char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        unsigned char array[LENGTH], data[7][LENGTH];

        memset(array, 0x00, LENGTH);
        array[0] = 0x1c;
        array[1] = 0x01;
        array[2] = 0x01;
        array[SAVE_SLOT] = slot;
        array[6] = 0x01;

        auto n = comm->interruptWrite(endpointSend, adapt(array, LENGTH));

        for (int i = 0; n != 0; i++)
        {
            const auto recvData = comm->interruptReceive(endpointRecv, LENGTH);
            n = recvData.size();

            if (i < 7)
            {
                memcpy(data[i], recvData.data(), n);
            }
        }

        if (name != nullptr || amp_set != nullptr || effects_set != nullptr)
        {
            decode_data(data, name, amp_set, effects_set);
        }

        return 0;
    }

    int Mustang::decode_data(unsigned char data[7][LENGTH], char* name, amp_settings* amp_set, fx_pedal_settings* effects_set)
    {
        if (name != nullptr)
        {
            // NAME
            memset(name, 0x00, 32);
            for (int i = 0, j = 16; data[0][j] != 0x00; i++, j++)
            {
                name[i] = data[0][j];
            }
        }


        if (amp_set != nullptr)
        {
            // AMPLIFIER
            amp_set->amp_num = value(lookupAmpById(data[1][AMPLIFIER]));

            amp_set->gain = data[1][GAIN];
            amp_set->volume = data[1][VOLUME];
            amp_set->treble = data[1][TREBLE];
            amp_set->middle = data[1][MIDDLE];
            amp_set->bass = data[1][BASS];
            amp_set->cabinet = data[1][CABINET];
            amp_set->noise_gate = data[1][NOISE_GATE];
            amp_set->master_vol = data[1][MASTER_VOL];
            amp_set->gain2 = data[1][GAIN2];
            amp_set->presence = data[1][PRESENCE];
            amp_set->threshold = data[1][THRESHOLD];
            amp_set->depth = data[1][DEPTH];
            amp_set->bias = data[1][BIAS];
            amp_set->sag = data[1][SAG];
            amp_set->brightness = data[1][BRIGHTNESS] != 0u;
            amp_set->usb_gain = data[6][16];
        }


        if (effects_set != nullptr)
        {
            // EFFECTS
            for (int i = 2; i < 6; i++)
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

                effects_set[j].fx_slot = j;
                effects_set[j].knob1 = data[i][KNOB1];
                effects_set[j].knob2 = data[i][KNOB2];
                effects_set[j].knob3 = data[i][KNOB3];
                effects_set[j].knob4 = data[i][KNOB4];
                effects_set[j].knob5 = data[i][KNOB5];
                effects_set[j].knob6 = data[i][KNOB6];
                if (data[i][FXSLOT] > 0x03)
                {
                    effects_set[j].put_post_amp = true;
                }
                else
                {
                    effects_set[j].put_post_amp = false;
                }

                effects_set[j].effect_num = value(lookupEffectById(data[i][EFFECT]));
            }
        }

        return 0;
    }

    int Mustang::save_effects(int slot, char name[24], int number_of_effects, fx_pedal_settings effects[2])
    {
        unsigned char fxknob, repeat;
        unsigned char array[LENGTH] = {
            0x1c, 0x01, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        execute[2] = 0x00; // why this must be here?

        if (number_of_effects > 2)
        {
            repeat = 1;
        }
        else
        {
            repeat = number_of_effects;
        }

        for (int i = 0; i < repeat; i++)
        {
            if (effects[i].effect_num < value(effects::SINE_CHORUS))
            {
                return -1;
            }
        }

        if (effects[0].effect_num >= value(effects::SINE_CHORUS) && effects[0].effect_num <= value(effects::PITCH_SHIFTER))
        {
            fxknob = 0x01;
            repeat = 1; //just to be sure
        }
        else
        {
            fxknob = 0x02;
        }

        array[FXKNOB] = fxknob;

        array[SAVE_SLOT] = slot;

        // set and send the name
        if (name[23] != 0x00)
        {
            name[23] = 0x00;
        }
        for (int i = 0, j = 16; name[i] != 0x00; i++, j++)
        {
            array[j] = name[i];
        }

        comm->interruptWrite(endpointSend, adapt(array, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);

        array[1] = 0x03;
        array[6] = 0x00;
        memset(array + 16, 0x00, LENGTH - 16);
        for (int i = 0; i < repeat; i++)
        {
            array[19] = 0x00;
            array[20] = 0x08;
            array[21] = 0x01;
            array[KNOB6] = 0x00;

            if (effects[i].put_post_amp)
            {
                array[FXSLOT] = effects[i].fx_slot + 4;
            }
            else
            {
                array[FXSLOT] = effects[i].fx_slot;
            }
            array[KNOB1] = effects[i].knob1;
            array[KNOB2] = effects[i].knob2;
            array[KNOB3] = effects[i].knob3;
            array[KNOB4] = effects[i].knob4;
            array[KNOB5] = effects[i].knob5;
            // some effects have more knobs
            if (effects[i].effect_num == value(effects::MONO_ECHO_FILTER) ||
                effects[i].effect_num == value(effects::STEREO_ECHO_FILTER) ||
                effects[i].effect_num == value(effects::TAPE_DELAY) ||
                effects[i].effect_num == value(effects::STEREO_TAPE_DELAY))
            {
                array[KNOB6] = effects[i].knob6;
            }

            // fill the form with missing data
            switch (static_cast<plug::effects>(effects[i].effect_num))
            {
                case effects::SINE_CHORUS:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x12;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::TRIANGLE_CHORUS:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x13;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::SINE_FLANGER:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x18;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::TRIANGLE_FLANGER:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x19;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::VIBRATONE:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x2d;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::VINTAGE_TREMOLO:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x40;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::SINE_TREMOLO:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x41;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::RING_MODULATOR:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x22;
                    array[19] = 0x01;
                    if (array[KNOB4] > 0x01)
                    {
                        array[KNOB4] = 0x01;
                    }
                    break;

                case effects::STEP_FILTER:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x29;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    break;

                case effects::PHASER:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x4f;
                    array[19] = 0x01;
                    array[20] = 0x01;
                    if (array[KNOB5] > 0x01)
                    {
                        array[KNOB5] = 0x01;
                    }
                    break;

                case effects::PITCH_SHIFTER:
                    array[DSP] = 0x07;
                    array[EFFECT] = 0x1f;
                    array[19] = 0x01;
                    break;

                case effects::MONO_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x16;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::MONO_ECHO_FILTER:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x43;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::STEREO_ECHO_FILTER:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x48;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::MULTITAP_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x44;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::PING_PONG_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x45;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::DUCKING_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x15;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::REVERSE_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x46;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::TAPE_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x2b;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::STEREO_TAPE_DELAY:
                    array[DSP] = 0x08;
                    array[EFFECT] = 0x2a;
                    array[19] = 0x02;
                    array[20] = 0x01;
                    break;

                case effects::SMALL_HALL_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x24;
                    break;

                case effects::LARGE_HALL_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x3a;
                    break;

                case effects::SMALL_ROOM_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x26;
                    break;

                case effects::LARGE_ROOM_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x3b;
                    break;

                case effects::SMALL_PLATE_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x4e;
                    break;

                case effects::LARGE_PLATE_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x4b;
                    break;

                case effects::AMBIENT_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x4c;
                    break;

                case effects::ARENA_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x4d;
                    break;

                case effects::FENDER_63_SPRING_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x21;
                    break;

                case effects::FENDER_65_SPRING_REVERB:
                    array[DSP] = 0x09;
                    array[EFFECT] = 0x0b;
                    break;

                default:
                    break;
            }
            // send packet
            comm->interruptWrite(endpointSend, adapt(array, LENGTH));
            comm->interruptReceive(endpointRecv, LENGTH);
        }

        execute[FXKNOB] = fxknob;
        comm->interruptWrite(endpointSend, adapt(execute, LENGTH));
        comm->interruptReceive(endpointRecv, LENGTH);
        execute[FXKNOB] = 0x00;

        return 0;
    }
}
