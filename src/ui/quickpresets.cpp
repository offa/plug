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

#include "ui/quickpresets.h"
#include "ui_quickpresets.h"

namespace plug
{

    QuickPresets::QuickPresets(QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::QuickPresets>())
    {
        ui->setupUi(this);

        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));

        connect(ui->comboBox, SIGNAL(activated(int)), this, SLOT(setDefaultPreset0(int)));
        connect(ui->comboBox_2, SIGNAL(activated(int)), this, SLOT(setDefaultPreset1(int)));
        connect(ui->comboBox_3, SIGNAL(activated(int)), this, SLOT(setDefaultPreset2(int)));
        connect(ui->comboBox_4, SIGNAL(activated(int)), this, SLOT(setDefaultPreset3(int)));
        connect(ui->comboBox_5, SIGNAL(activated(int)), this, SLOT(setDefaultPreset4(int)));
        connect(ui->comboBox_6, SIGNAL(activated(int)), this, SLOT(setDefaultPreset5(int)));
        connect(ui->comboBox_7, SIGNAL(activated(int)), this, SLOT(setDefaultPreset6(int)));
        connect(ui->comboBox_8, SIGNAL(activated(int)), this, SLOT(setDefaultPreset7(int)));
        connect(ui->comboBox_9, SIGNAL(activated(int)), this, SLOT(setDefaultPreset8(int)));
        connect(ui->comboBox_10, SIGNAL(activated(int)), this, SLOT(setDefaultPreset9(int)));
    }

    void QuickPresets::load_names(const std::vector<std::string>& names)
    {
        QSettings settings;
        std::size_t i = 0;

        for (i = 0; i < 100; i++)
        {
            if (names[i][0] == 0x00)
            {
                break;
            }
            const QString name = QString::fromStdString(names[i]);
            ui->comboBox->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_2->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_3->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_4->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_5->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_6->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_7->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_8->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_9->addItem(QString("[%1] %2").arg(i + 1).arg(name));
            ui->comboBox_10->addItem(QString("[%1] %2").arg(i + 1).arg(name));
        }

        ui->comboBox->addItem(tr("[Empty]"));
        ui->comboBox_2->addItem(tr("[Empty]"));
        ui->comboBox_3->addItem(tr("[Empty]"));
        ui->comboBox_4->addItem(tr("[Empty]"));
        ui->comboBox_5->addItem(tr("[Empty]"));
        ui->comboBox_6->addItem(tr("[Empty]"));
        ui->comboBox_7->addItem(tr("[Empty]"));
        ui->comboBox_8->addItem(tr("[Empty]"));
        ui->comboBox_9->addItem(tr("[Empty]"));
        ui->comboBox_10->addItem(tr("[Empty]"));

        if (settings.contains("DefaultPresets/Preset0"))
            ui->comboBox->setCurrentIndex(settings.value("DefaultPresets/Preset0").toInt());
        else
            ui->comboBox->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset1"))
            ui->comboBox_2->setCurrentIndex(settings.value("DefaultPresets/Preset1").toInt());
        else
            ui->comboBox_2->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset2"))
            ui->comboBox_3->setCurrentIndex(settings.value("DefaultPresets/Preset2").toInt());
        else
            ui->comboBox_3->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset3"))
            ui->comboBox_4->setCurrentIndex(settings.value("DefaultPresets/Preset3").toInt());
        else
            ui->comboBox_4->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset4"))
            ui->comboBox_5->setCurrentIndex(settings.value("DefaultPresets/Preset4").toInt());
        else
            ui->comboBox_5->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset5"))
            ui->comboBox_6->setCurrentIndex(settings.value("DefaultPresets/Preset5").toInt());
        else
            ui->comboBox_6->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset6"))
            ui->comboBox_7->setCurrentIndex(settings.value("DefaultPresets/Preset6").toInt());
        else
            ui->comboBox_7->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset7"))
            ui->comboBox_8->setCurrentIndex(settings.value("DefaultPresets/Preset7").toInt());
        else
            ui->comboBox_8->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset8"))
            ui->comboBox_9->setCurrentIndex(settings.value("DefaultPresets/Preset8").toInt());
        else
            ui->comboBox_9->setCurrentIndex(i);

        if (settings.contains("DefaultPresets/Preset9"))
            ui->comboBox_10->setCurrentIndex(settings.value("DefaultPresets/Preset9").toInt());
        else
            ui->comboBox_10->setCurrentIndex(i);
    }

    void QuickPresets::delete_items()
    {
        int j = ui->comboBox->count();
        for (int i = 0; i < j; i++)
        {
            ui->comboBox->removeItem(0);
            ui->comboBox_2->removeItem(0);
            ui->comboBox_3->removeItem(0);
            ui->comboBox_4->removeItem(0);
            ui->comboBox_5->removeItem(0);
            ui->comboBox_6->removeItem(0);
            ui->comboBox_7->removeItem(0);
            ui->comboBox_8->removeItem(0);
            ui->comboBox_9->removeItem(0);
            ui->comboBox_10->removeItem(0);
        }
    }

    void QuickPresets::change_name(int slot, QString* name)
    {
        ui->comboBox->setItemText(slot, *name);
        ui->comboBox->setCurrentIndex(slot);

        ui->comboBox_2->setItemText(slot, *name);
        ui->comboBox_2->setCurrentIndex(slot);

        ui->comboBox_3->setItemText(slot, *name);
        ui->comboBox_3->setCurrentIndex(slot);

        ui->comboBox_4->setItemText(slot, *name);
        ui->comboBox_4->setCurrentIndex(slot);

        ui->comboBox_5->setItemText(slot, *name);
        ui->comboBox_5->setCurrentIndex(slot);

        ui->comboBox_6->setItemText(slot, *name);
        ui->comboBox_6->setCurrentIndex(slot);

        ui->comboBox_7->setItemText(slot, *name);
        ui->comboBox_7->setCurrentIndex(slot);

        ui->comboBox_8->setItemText(slot, *name);
        ui->comboBox_8->setCurrentIndex(slot);

        ui->comboBox_9->setItemText(slot, *name);
        ui->comboBox_9->setCurrentIndex(slot);

        ui->comboBox_10->setItemText(slot, *name);
        ui->comboBox_10->setCurrentIndex(slot);
    }

    void QuickPresets::setDefaultPreset0(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset0");
        else
            settings.setValue("DefaultPresets/Preset0", slot);
    }

    void QuickPresets::setDefaultPreset1(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset1");
        else
            settings.setValue("DefaultPresets/Preset1", slot);
    }

    void QuickPresets::setDefaultPreset2(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset2");
        else
            settings.setValue("DefaultPresets/Preset2", slot);
    }

    void QuickPresets::setDefaultPreset3(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset3");
        else
            settings.setValue("DefaultPresets/Preset3", slot);
    }

    void QuickPresets::setDefaultPreset4(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset4");
        else
            settings.setValue("DefaultPresets/Preset4", slot);
    }

    void QuickPresets::setDefaultPreset5(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset5");
        else
            settings.setValue("DefaultPresets/Preset5", slot);
    }

    void QuickPresets::setDefaultPreset6(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset6");
        else
            settings.setValue("DefaultPresets/Preset6", slot);
    }

    void QuickPresets::setDefaultPreset7(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset7");
        else
            settings.setValue("DefaultPresets/Preset7", slot);
    }

    void QuickPresets::setDefaultPreset8(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset8");
        else
            settings.setValue("DefaultPresets/Preset8", slot);
    }

    void QuickPresets::setDefaultPreset9(int slot)
    {
        QSettings settings;

        if (slot == 24 || slot == 100)
            settings.remove("DefaultPresets/Preset9");
        else
            settings.setValue("DefaultPresets/Preset9", slot);
    }

    void QuickPresets::changeEvent(QEvent* e)
    {
        QDialog::changeEvent(e);
        switch (e->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }
}

#include "ui/moc_quickpresets.moc"
