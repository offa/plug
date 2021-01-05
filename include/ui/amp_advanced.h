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

#include <QDialog>
#include <QSettings>
#include <memory>

namespace Ui
{
    class Amp_Advanced;
}

namespace plug
{

    class Amp_Advanced : public QDialog
    {
        Q_OBJECT

    public:
        explicit Amp_Advanced(QWidget* parent = nullptr);
        Amp_Advanced(const Amp_Advanced&) = delete;
        ~Amp_Advanced() override;

        Amp_Advanced& operator=(const Amp_Advanced&) = delete;

    public slots:
        void change_cabinet(int);
        void change_noise_gate(int);
        void set_master_vol(int);
        void set_gain2(int);
        void set_presence(int);
        void set_depth(int);
        void set_threshold(int);
        void set_bias(int);
        void set_sag(int);
        void set_brightness(bool);
        void set_usb_gain(int);

    private slots:
        void activate_custom_ng(int);

    private:
        const std::unique_ptr<Ui::Amp_Advanced> ui;
    };
}
