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
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <memory>

namespace Ui
{
    class SaveToFile;
}

namespace plug
{

    class SaveToFile : public QDialog
    {
        Q_OBJECT

    public:
        explicit SaveToFile(QWidget* parent = nullptr);

    private slots:
        QString choose_destination();
        void savefile();

    signals:
        void destination_chosen(QString);

    private:
        const std::unique_ptr<Ui::SaveToFile> ui;
        std::unique_ptr<QXmlStreamWriter> xml;

        void writeAmp(amp_settings);
        void manageWriteFX(fx_pedal_settings[4]);
        void writeFX(fx_pedal_settings);
        void writeFUSE();
        void writeUSBGain(int);
    };
}
