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

#include "ui/library.h"
#include "ui/mainwindow.h"
#include "ui_library.h"
#include <QDir>
#include <QFileDialog>
#include <QSettings>

namespace plug
{

    Library::Library(const std::vector<std::string>& names, QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::Library>()),
          files(std::make_unique<QList<QFileInfo>>())
    {
        ui->setupUi(this);
        QSettings settings;
        restoreGeometry(settings.value("Windows/libraryWindowGeometry").toByteArray());

        if (settings.contains("Library/lastDirectory"))
        {
            ui->label_3->setText(settings.value("Library/lastDirectory").toString());
            get_files(settings.value("Library/lastDirectory").toString());
        }

        QFont font(settings.value("Library/FontFamily", ui->listWidget->font().family()).toString(), settings.value("Library/FontSize", ui->listWidget->font().pointSize()).toInt());
        ui->listWidget->setFont(font);
        ui->listWidget_2->setFont(font);

        ui->spinBox->setValue(font.pointSize());
        ui->fontComboBox->setCurrentFont(font);

        for (std::size_t i = 0; i < 100; ++i)
        {
            if (names[i][0] == 0x00)
            {
                break;
            }
            ui->listWidget->addItem(QString("[%1] %2").arg(i + 1).arg(QString::fromStdString(names[i])));
        }

        connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(load_slot(int)));
        connect(ui->listWidget_2, SIGNAL(currentRowChanged(int)), this, SLOT(load_file(int)));
        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(get_directory()));
        connect(this, SIGNAL(directory_changed(QString)), ui->label_3, SLOT(setText(QString)));
        connect(this, SIGNAL(directory_changed(QString)), this, SLOT(get_files(QString)));
        connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(change_font_size(int)));
        connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(change_font_family(QFont)));
    }

    Library::~Library()
    {
        QSettings settings;
        settings.setValue("Windows/libraryWindowGeometry", saveGeometry());
    }

    void Library::load_slot(int slot)
    {
        if (slot < 0)
        {
            return;
        }

        ui->listWidget_2->setCurrentRow(-1);
        dynamic_cast<MainWindow*>(parent())->load_from_amp(slot);
    }

    void Library::get_directory()
    {
        QSettings settings;
        QString directory = QFileDialog::getExistingDirectory(this, tr("Choose directory"), settings.value("Library/lastDirectory", QDir::homePath()).toString());

        if (directory.isEmpty())
        {
            return;
        }

        settings.setValue("Library/lastDirectory", directory);
        emit directory_changed(directory);
    }

    void Library::get_files(const QString& path)
    {
        QDir directory(path, "*.fuse", (QDir::Name | QDir::IgnoreCase), (QDir::Files | QDir::NoDotAndDotDot | QDir::Readable));

        if (!files->isEmpty())
        {
            files->clear();
        }
        ui->listWidget_2->clear();
        *files = directory.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        for (int i = 0; i < files->size(); ++i)
        {
            ui->listWidget_2->addItem((*files)[i].completeBaseName());
        }
    }

    void Library::load_file(int row)
    {
        if (row < 0)
        {
            return;
        }

        ui->listWidget->setCurrentRow(-1);
        dynamic_cast<MainWindow*>(parent())->loadfile((*files)[row].canonicalFilePath());
    }

    void Library::resizeEvent(QResizeEvent* event)
    {
        ui->label_3->setMaximumWidth((event->size().width() / 2) - ui->pushButton->size().width());
    }

    void Library::change_font_size(int value)
    {
        QSettings settings;
        QFont font(ui->listWidget_2->font());

        font.setPointSize(value);
        ui->listWidget->setFont(font);
        ui->listWidget_2->setFont(font);

        settings.setValue("Library/FontSize", value);
    }

    void Library::change_font_family(QFont font)
    {
        QSettings settings;

        font.setPointSize(ui->spinBox->value());
        ui->listWidget->setFont(font);
        ui->listWidget_2->setFont(font);

        settings.setValue("Library/FontFamily", font.family());
    }
}

#include "ui/moc_library.moc"
