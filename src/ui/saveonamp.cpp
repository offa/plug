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

#include "ui/saveonamp.h"
#include "ui/mainwindow.h"
#include "ui_saveonamp.h"
#include <QSettings>

namespace plug
{

    SaveOnAmp::SaveOnAmp(QWidget* parent)
        : QMainWindow(parent),
          ui(std::make_unique<Ui::SaveOnAmp>())
    {
        ui->setupUi(this);

        QSettings settings;
        restoreGeometry(settings.value("Windows/saveAmpPresetWindowGeometry").toByteArray());

        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(save()));
        connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
    }

    SaveOnAmp::~SaveOnAmp()
    {
        QSettings settings;
        settings.setValue("Windows/saveAmpPresetWindowGeometry", saveGeometry());
    }

    void SaveOnAmp::save()
    {
        QSettings settings;
        QString name(QString("[%1] %2").arg(ui->comboBox->currentIndex()).arg(ui->lineEdit->text()));

        ui->comboBox->setItemText(ui->comboBox->currentIndex(), name);
        dynamic_cast<MainWindow*>(parent())->change_name(ui->comboBox->currentIndex(), &name);
        dynamic_cast<MainWindow*>(parent())->save_on_amp(ui->lineEdit->text().toLatin1().data(), ui->comboBox->currentIndex());
        if (!settings.value("Settings/keepWindowsOpen").toBool())
        {
            this->close();
        }
    }

    void SaveOnAmp::load_names(const std::vector<std::string>& names)
    {
        for (std::size_t i = 0; i < 100; ++i)
        {
            if (names[i][0] == 0x00)
            {
                break;
            }
            ui->comboBox->addItem(QString("[%1] %2").arg(i + 1).arg(QString::fromStdString(names[i])));
        }
    }

    void SaveOnAmp::delete_items()
    {
        for (int i = 0; i < ui->comboBox->count(); ++i)
        {
            ui->comboBox->removeItem(0);
        }
    }

    void SaveOnAmp::change_index(int value, const QString& name)
    {
        if (value > 0)
        {
            ui->comboBox->setCurrentIndex(value);
        }
        ui->lineEdit->setText(name);
    }
}

#include "ui/moc_saveonamp.moc"
