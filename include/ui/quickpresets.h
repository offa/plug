/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2024  offa
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
    class QuickPresets;
}

namespace plug
{

    class QuickPresets : public QDialog
    {
        Q_OBJECT

    public:
        explicit QuickPresets(QWidget* parent = nullptr);

        void load_names(const std::vector<std::string>& names);
        void delete_items();
        void change_name(int, QString*);

    protected:
        void changeEvent(QEvent* e) override;

    private slots:
        void setDefaultPreset0(int);
        void setDefaultPreset1(int);
        void setDefaultPreset2(int);
        void setDefaultPreset3(int);
        void setDefaultPreset4(int);
        void setDefaultPreset5(int);
        void setDefaultPreset6(int);
        void setDefaultPreset7(int);
        void setDefaultPreset8(int);
        void setDefaultPreset9(int);

    private:
        const std::unique_ptr<Ui::QuickPresets> ui;
    };
}
