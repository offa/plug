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

namespace plug
{

    struct amp_settings
    {
        unsigned char amp_num;
        unsigned char gain;
        unsigned char volume;
        unsigned char treble;
        unsigned char middle;
        unsigned char bass;
        unsigned char cabinet;
        unsigned char noise_gate;
        unsigned char master_vol;
        unsigned char gain2;
        unsigned char presence;
        unsigned char threshold;
        unsigned char depth;
        unsigned char bias;
        unsigned char sag;
        bool brightness;
        unsigned char usb_gain;
    };

    struct fx_pedal_settings
    {
        unsigned char fx_slot;
        unsigned char effect_num;
        unsigned char knob1;
        unsigned char knob2;
        unsigned char knob3;
        unsigned char knob4;
        unsigned char knob5;
        unsigned char knob6;
        bool put_post_amp;
    };
}
