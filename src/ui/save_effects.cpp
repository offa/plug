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

#include "ui/save_effects.h"
#include "ui/mainwindow.h"
#include "ui_save_effects.h"
#include <QSettings>

namespace plug
{

    SaveEffects::SaveEffects(QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::Save_effects>())
    {
        ui->setupUi(this);

        QSettings settings;
        restoreGeometry(settings.value("Windows/saveEffectPresetWindowGeometry").toByteArray());

        connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(select_checkbox()));
        connect(ui->checkBox_2, SIGNAL(clicked()), this, SLOT(select_checkbox()));
        connect(ui->checkBox_3, SIGNAL(clicked()), this, SLOT(select_checkbox()));
        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(send()));
        connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
    }

    SaveEffects::~SaveEffects()
    {
        QSettings settings;
        settings.setValue("Windows/saveEffectPresetWindowGeometry", saveGeometry());
    }

    void SaveEffects::select_checkbox()
    {
        const bool buttonDisabled = (!ui->checkBox->isChecked() && !ui->checkBox_2->isChecked() && !ui->checkBox_3->isChecked());
        ui->pushButton->setDisabled(buttonDisabled);

        if (sender() == ui->checkBox)
        {
            ui->checkBox_2->setChecked(false);
            ui->checkBox_3->setChecked(false);
        }
        else
        {
            ui->checkBox->setChecked(false);
        }
    }

    void SaveEffects::send()
    {
        int number = 0;

        if (ui->checkBox->isChecked())
        {
            number = 1;
        }
        else
        {
            if (ui->checkBox_2->isChecked())
            {
                ++number;
            }
            if (ui->checkBox_3->isChecked())
            {
                ++number;
            }
        }

        dynamic_cast<MainWindow*>(parent())->save_effects(ui->comboBox->currentIndex(), ui->lineEdit->text().toLatin1().data(), number,
                                                          ui->checkBox->isChecked(), ui->checkBox_2->isChecked(), ui->checkBox_3->isChecked());
        this->close();
    }
}

#include "ui/moc_save_effects.moc"
