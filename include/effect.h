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

#ifndef EFFECT_H
#define EFFECT_H

#include "data_structs.h"
#include "effects_enum.h"
#include <QMainWindow>
#include <QSettings>
#include <QShortcut>
#include <memory>

namespace Ui
{
    class Effect;
}

namespace plug
{

    class Effect : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit Effect(QWidget* parent = nullptr, int number = 0);
        ~Effect() override;

        void set_changed(bool);
        bool get_changed();

    private:
        const std::unique_ptr<Ui::Effect> ui;
        unsigned char fx_slot, effect_num, knob1, knob2, knob3, knob4, knob5, knob6;
        bool put_post_amp, changed;
        QString temp1, temp2;

    public slots:
        // functions to set variables
        void set_post_amp(bool);
        void set_knob1(int);
        void set_knob2(int);
        void set_knob3(int);
        void set_knob4(int);
        void set_knob5(int);
        void set_knob6(int);
        void choose_fx(int);
        void off_switch(bool);
        void enable_set_button(bool);

        // send settings to the amplifier
        void send_fx();

        void load(fx_pedal_settings);
        void get_settings(fx_pedal_settings&);
        void load_default_fx();

        void showAndActivate();
    };
}

#endif // EFFECT_H
