/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2026  offa
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
#include <algorithm>

namespace plug
{

    QuickPresets::QuickPresets(QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::QuickPresets>())
    {
        ui->setupUi(this);

        connect(ui->pushButton, &QPushButton::clicked, this, &QWidget::close);

        connect(ui->comboBox, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(0, slot); });
        connect(ui->comboBox_2, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(1, slot); });
        connect(ui->comboBox_3, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(2, slot); });
        connect(ui->comboBox_4, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(3, slot); });
        connect(ui->comboBox_5, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(4, slot); });
        connect(ui->comboBox_6, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(5, slot); });
        connect(ui->comboBox_7, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(6, slot); });
        connect(ui->comboBox_8, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(7, slot); });
        connect(ui->comboBox_9, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(8, slot); });
        connect(ui->comboBox_10, &QComboBox::activated, this, [this](int slot)
                { setDefaultPreset(9, slot); });
    }

    void QuickPresets::load_names(const std::vector<std::string>& names)
    {
        QSettings settings;
        const QString fmt = QStringLiteral("[%1] %2");
        std::size_t i = 0;

        std::for_each(names.cbegin(), names.cend(), [&i, &fmt, this](const auto& nameStr)
                      {
            const QString item = fmt.arg(i + 1).arg(QString::fromStdString(nameStr));

            ui->comboBox->addItem(item);
            ui->comboBox_2->addItem(item);
            ui->comboBox_3->addItem(item);
            ui->comboBox_4->addItem(item);
            ui->comboBox_5->addItem(item);
            ui->comboBox_6->addItem(item);
            ui->comboBox_7->addItem(item);
            ui->comboBox_8->addItem(item);
            ui->comboBox_9->addItem(item);
            ui->comboBox_10->addItem(item);
            ++i; });

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

        auto setCurrentIndexOrPreset = [&settings](QComboBox& cb, int presetIndex, std::size_t index)
        {
            if (const QString key = QStringLiteral("DefaultPresets/Preset%1").arg(presetIndex); settings.contains(key))
            {
                cb.setCurrentIndex(settings.value(key).toInt());
            }
            else
            {
                cb.setCurrentIndex(index);
            }
        };

        setCurrentIndexOrPreset(*ui->comboBox, 0, i);
        setCurrentIndexOrPreset(*ui->comboBox_2, 1, i);
        setCurrentIndexOrPreset(*ui->comboBox_3, 2, i);
        setCurrentIndexOrPreset(*ui->comboBox_4, 3, i);
        setCurrentIndexOrPreset(*ui->comboBox_5, 4, i);
        setCurrentIndexOrPreset(*ui->comboBox_6, 5, i);
        setCurrentIndexOrPreset(*ui->comboBox_7, 6, i);
        setCurrentIndexOrPreset(*ui->comboBox_8, 7, i);
        setCurrentIndexOrPreset(*ui->comboBox_9, 8, i);
        setCurrentIndexOrPreset(*ui->comboBox_10, 9, i);
    }

    void QuickPresets::delete_items()
    {
        ui->comboBox->clear();
        ui->comboBox_2->clear();
        ui->comboBox_3->clear();
        ui->comboBox_4->clear();
        ui->comboBox_5->clear();
        ui->comboBox_6->clear();
        ui->comboBox_7->clear();
        ui->comboBox_8->clear();
        ui->comboBox_9->clear();
        ui->comboBox_10->clear();
    }

    void QuickPresets::change_name(int slot, QString* name)
    {
        auto setTextAndSlot = [](QComboBox& cb, int s, const QString& n)
        {
            cb.setItemText(s, n);
            cb.setCurrentIndex(s);
        };

        setTextAndSlot(*ui->comboBox, slot, *name);
        setTextAndSlot(*ui->comboBox_2, slot, *name);
        setTextAndSlot(*ui->comboBox_3, slot, *name);
        setTextAndSlot(*ui->comboBox_4, slot, *name);
        setTextAndSlot(*ui->comboBox_5, slot, *name);
        setTextAndSlot(*ui->comboBox_6, slot, *name);
        setTextAndSlot(*ui->comboBox_7, slot, *name);
        setTextAndSlot(*ui->comboBox_8, slot, *name);
        setTextAndSlot(*ui->comboBox_9, slot, *name);
        setTextAndSlot(*ui->comboBox_10, slot, *name);
    }

    void QuickPresets::changeEvent(QEvent* e)
    {
        QDialog::changeEvent(e);

        if (e->type() == QEvent::LanguageChange)
        {
            ui->retranslateUi(this);
        }
    }

    void QuickPresets::setDefaultPreset(int index, int slot)
    {
        QSettings settings;
        const QString key = QStringLiteral("DefaultPresets/Preset%1").arg(index);

        if (slot == 24 || slot == 100)
        {
            settings.remove(key);
        }
        else
        {
            settings.setValue(key, slot);
        }
    }
}

#include "ui/moc_quickpresets.moc"
