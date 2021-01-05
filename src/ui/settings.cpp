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

#include "ui/settings.h"
#include "ui_settings.h"

namespace plug
{

    Settings::Settings(QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::Settings>())
    {
        QSettings settings;

        ui->setupUi(this);

        ui->checkBox_2->setChecked(settings.value("Settings/connectOnStartup").toBool());
        ui->checkBox_3->setChecked(settings.value("Settings/oneSetToSetThemAll").toBool());
        ui->checkBox_4->setChecked(settings.value("Settings/keepWindowsOpen").toBool());
        ui->checkBox_5->setChecked(settings.value("Settings/popupChangedWindows").toBool());
        ui->checkBox_6->setChecked(settings.value("Settings/defaultEffectValues").toBool());

        connect(ui->checkBox_2, SIGNAL(toggled(bool)), this, SLOT(change_connect(bool)));
        connect(ui->checkBox_3, SIGNAL(toggled(bool)), this, SLOT(change_oneset(bool)));
        connect(ui->checkBox_4, SIGNAL(toggled(bool)), this, SLOT(change_keepopen(bool)));
        connect(ui->checkBox_5, SIGNAL(toggled(bool)), this, SLOT(change_popupwindows(bool)));
        connect(ui->checkBox_6, SIGNAL(toggled(bool)), this, SLOT(change_effectvalues(bool)));
    }

    void Settings::change_connect(bool value)
    {
        QSettings settings;

        settings.setValue("Settings/connectOnStartup", value);
    }

    void Settings::change_oneset(bool value)
    {
        QSettings settings;

        settings.setValue("Settings/oneSetToSetThemAll", value);
    }

    void Settings::change_keepopen(bool value)
    {
        QSettings settings;

        settings.setValue("Settings/keepWindowsOpen", value);
    }

    void Settings::change_popupwindows(bool value)
    {
        QSettings settings;

        settings.setValue("Settings/popupChangedWindows", value);
    }

    void Settings::change_effectvalues(bool value)
    {
        QSettings settings;

        settings.setValue("Settings/defaultEffectValues", value);
    }
}

#include "ui/moc_settings.moc"
