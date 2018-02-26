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
#include <memory>
#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace plug
{
    class UsbComm;

// amp's VID and PID
#define USB_VID 0x1ed8
#define SMALL_AMPS_USB_PID 0x0004    //Mustang I and II
#define BIG_AMPS_USB_PID 0x0005      //Mustang III, IV and V
#define MINI_USB_PID 0x0010          //Mustang Mini
#define FLOOR_USB_PID 0x0012         //Mustang Floor
#define SMALL_AMPS_V2_USB_PID 0x0014 //Mustang II (and I?) V2
#define BIG_AMPS_V2_USB_PID 0x0016   //Mustang III+ V2

// amp's VID and PID while in update mode
#define USB_UPDATE_VID 0x1ed8
#define SMALL_AMPS_USB_UPDATE_PID 0x0006    //Mustang I and II
#define BIG_AMPS_USB_UPDATE_PID 0x0007      //Mustang III, IV, V
#define MINI_USB_UPDATE_PID 0x0011          //Mustang Mini
#define FLOOR_USB_UPDATE_PID 0x0013         //Mustang Floor
#define SMALL_AMPS_V2_USB_UPDATE_PID 0x0015 //Mustang I & II V2
#define BIG_AMPS_V2_USB_UPDATE_PID 0x0017   //Mustang III+ V2

// for USB communication
#define TMOUT 500
#define LENGTH 64
//#define NANO_SEC_SLEEP 10000000

// effect array fields
#define DSP 2
#define EFFECT 16
#define FXSLOT 18
#define KNOB1 32
#define KNOB2 33
#define KNOB3 34
#define KNOB4 35
#define KNOB5 36
#define KNOB6 37

// amp array fields
#define AMPLIFIER 16
#define VOLUME 32
#define GAIN 33
#define TREBLE 36
#define MIDDLE 37
#define BASS 38
#define CABINET 49
#define NOISE_GATE 47
#define THRESHOLD 48
#define MASTER_VOL 35
#define GAIN2 34
#define PRESENCE 39
#define DEPTH 41
#define BIAS 42
#define SAG 51
#define BRIGHTNESS 52

// save fields
#define SAVE_SLOT 4
#define FXKNOB 3

    class Mustang
    {
    public:
        Mustang();
        ~Mustang();
        int start_amp(char list[][32] = nullptr, char* name = nullptr, amp_settings* amp_set = nullptr, fx_pedal_settings* effects_set = nullptr); // initialize communication
        void stop_amp();                                                                                                                           // terminate communication
        int set_effect(fx_pedal_settings);
        int set_amplifier(amp_settings);
        int save_on_amp(char*, int);
        int load_memory_bank(int, char* name = nullptr, amp_settings* amp_set = nullptr, fx_pedal_settings* effects_set = nullptr);
        int save_effects(int, char*, int, fx_pedal_settings*);

    private:
        std::unique_ptr<UsbComm> comm;
        unsigned char execute[LENGTH];       // "apply" command sent after each instruction
        unsigned char prev_array[4][LENGTH]; // array used to clear the effect

        int decode_data(unsigned char[7][LENGTH], char* name = nullptr, amp_settings* amp_set = nullptr, fx_pedal_settings* effects_set = nullptr);
    };
}
