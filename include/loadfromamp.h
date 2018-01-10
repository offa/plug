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

#ifndef LOADFROMAMP_H
#define LOADFROMAMP_H

#include <QMainWindow>
#include <memory>

namespace Ui
{
    class LoadFromAmp;
}

namespace plug
{

    class LoadFromAmp : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit LoadFromAmp(QWidget* parent = nullptr);
        ~LoadFromAmp() override;
        void load_names(char names[][32]);
        void delete_items();
        void change_name(int, QString*);

    private:
        const std::unique_ptr<Ui::LoadFromAmp> ui;

    private slots:
        void load();
    };
}

#endif // LOADFROMAMP_H
