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
#include <QMainWindow>
#include <memory>

namespace Ui
{
    class Amplifier;
}

namespace plug
{
    class Amp_Advanced;
}

namespace plug
{

    class Amplifier : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit Amplifier(QWidget* parent = nullptr);
        Amplifier(const Amplifier&) = delete;
        ~Amplifier() override;

        Amplifier& operator=(const Amplifier&) = delete;

    private:
        const std::unique_ptr<Ui::Amplifier> ui;
        std::unique_ptr<Amp_Advanced> advanced;
        amps amp_num;
        unsigned char gain, volume, treble, middle, bass;
        cabinets cabinet;
        unsigned char noise_gate, presence, gain2, master_vol, threshold, depth, bias, sag, usb_gain;
        bool changed, brightness;

    public slots:
        // set basic variables
        void set_gain(int);
        void set_volume(int);
        void set_treble(int);
        void set_middle(int);
        void set_bass(int);
        void choose_amp(int);

        // set advanced variables
        void set_cabinet(int);
        void set_noise_gate(int);
        void set_presence(int);
        void set_gain2(int);
        void set_master_vol(int);
        void set_threshold(int);
        void set_depth(int);
        void set_bias(int);
        void set_sag(int);
        void set_brightness(bool);
        void set_usb_gain(int);

        // send settings to the amplifier
        void send_amp();

        void load(amp_settings);
        void get_settings(amp_settings*);
        void enable_set_button(bool);

        void showAndActivate();
    };
}
