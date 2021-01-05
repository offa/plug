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

#include "ui/defaulteffects.h"
#include "ui/mainwindow.h"
#include "ui_defaulteffects.h"
#include <QSettings>
#include <array>

namespace plug
{
    namespace
    {
        struct UIText
        {
            QString label;
            QString dialName;
            QString dialDescription;
            QString spinBoxName;
            QString spinBoxDescription;
        };

        void setTexts(const Ui::DefaultEffects* ui, const UIText& e1, const UIText& e2, const UIText& e3, const UIText& e4, const UIText& e5, const UIText& e6)
        {
            ui->label->setText(e1.label);
            ui->dial->setAccessibleName(e1.dialName);
            ui->dial->setAccessibleDescription(e1.dialDescription);
            ui->spinBox->setAccessibleName(e1.spinBoxName);
            ui->spinBox->setAccessibleDescription(e1.spinBoxDescription);

            ui->label_2->setText(e2.label);
            ui->dial_2->setAccessibleName(e2.dialName);
            ui->dial_2->setAccessibleDescription(e2.dialDescription);
            ui->spinBox_2->setAccessibleName(e2.spinBoxName);
            ui->spinBox_2->setAccessibleDescription(e2.spinBoxDescription);

            ui->label_3->setText(e3.label);
            ui->dial_3->setAccessibleName(e3.dialName);
            ui->dial_3->setAccessibleDescription(e3.dialDescription);
            ui->spinBox_3->setAccessibleName(e3.spinBoxName);
            ui->spinBox_3->setAccessibleDescription(e3.spinBoxDescription);

            ui->label_4->setText(e4.label);
            ui->dial_4->setAccessibleName(e4.dialName);
            ui->dial_4->setAccessibleDescription(e4.dialDescription);
            ui->spinBox_4->setAccessibleName(e4.spinBoxName);
            ui->spinBox_4->setAccessibleDescription(e4.spinBoxDescription);

            ui->label_5->setText(e5.label);
            ui->dial_5->setAccessibleName(e5.dialName);
            ui->dial_5->setAccessibleDescription(e5.dialDescription);
            ui->spinBox_5->setAccessibleName(e5.spinBoxName);
            ui->spinBox_5->setAccessibleDescription(e5.spinBoxDescription);

            ui->label_6->setText(e6.label);
            ui->dial_6->setAccessibleName(e6.dialName);
            ui->dial_6->setAccessibleDescription(e6.dialDescription);
            ui->spinBox_6->setAccessibleName(e6.spinBoxName);
            ui->spinBox_6->setAccessibleDescription(e6.spinBoxDescription);
        }

    }

    DefaultEffects::DefaultEffects(QWidget* parent)
        : QDialog(parent),
          ui(std::make_unique<Ui::DefaultEffects>())
    {
        ui->setupUi(this);

        connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_fx(int)));
        connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(get_settings()));
        connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(save_default_effects()));
    }

    void DefaultEffects::choose_fx(int value)
    {
        // activate proper knobs and set their max values
        switch (static_cast<effects>(value))
        {
            case effects::EMPTY:
                ui->checkBox->setDisabled(true);
                if (sender() == ui->comboBox)
                {
                    ui->dial->setValue(0);
                    ui->dial_2->setValue(0);
                    ui->dial_3->setValue(0);
                    ui->dial_4->setValue(0);
                    ui->dial_5->setValue(0);
                    ui->dial_6->setValue(0);
                }
                ui->dial->setDisabled(true);
                ui->dial_2->setDisabled(true);
                ui->dial_3->setDisabled(true);
                ui->dial_4->setDisabled(true);
                ui->dial_5->setDisabled(true);
                ui->dial_6->setDisabled(true);
                ui->spinBox->setDisabled(true);
                ui->spinBox_2->setDisabled(true);
                ui->spinBox_3->setDisabled(true);
                ui->spinBox_4->setDisabled(true);
                ui->spinBox_5->setDisabled(true);
                ui->spinBox_6->setDisabled(true);
                break;

            case effects::SIMPLE_COMP:
                ui->checkBox->setDisabled(false);
                ui->dial->setMaximum(3);
                ui->spinBox->setMaximum(3);
                ui->dial_2->setValue(0);
                ui->dial_3->setValue(0);
                ui->dial_4->setValue(0);
                ui->dial_5->setValue(0);
                ui->dial_6->setValue(0);
                ui->dial->setDisabled(false);
                ui->dial_2->setDisabled(true);
                ui->dial_3->setDisabled(true);
                ui->dial_4->setDisabled(true);
                ui->dial_5->setDisabled(true);
                ui->dial_6->setDisabled(true);
                ui->spinBox->setDisabled(false);
                ui->spinBox_2->setDisabled(true);
                ui->spinBox_3->setDisabled(true);
                ui->spinBox_4->setDisabled(true);
                ui->spinBox_5->setDisabled(true);
                ui->spinBox_6->setDisabled(true);
                break;

            case effects::RING_MODULATOR:
                ui->checkBox->setDisabled(false);
                ui->dial->setMaximum(255);
                ui->spinBox->setMaximum(255);
                ui->dial_4->setMaximum(1);
                ui->spinBox_4->setMaximum(1);
                ui->dial_5->setMaximum(255);
                ui->spinBox_5->setMaximum(255);
                ui->dial_6->setValue(0);
                ui->dial->setDisabled(false);
                ui->dial_2->setDisabled(false);
                ui->dial_3->setDisabled(false);
                ui->dial_4->setDisabled(false);
                ui->dial_5->setDisabled(false);
                ui->dial_6->setDisabled(true);
                ui->spinBox->setDisabled(false);
                ui->spinBox_2->setDisabled(false);
                ui->spinBox_3->setDisabled(false);
                ui->spinBox_4->setDisabled(false);
                ui->spinBox_5->setDisabled(false);
                ui->spinBox_6->setDisabled(true);
                break;

            case effects::PHASER:
                ui->checkBox->setDisabled(false);
                ui->dial->setMaximum(255);
                ui->spinBox->setMaximum(255);
                ui->dial_4->setMaximum(255);
                ui->spinBox_4->setMaximum(255);
                ui->dial_5->setMaximum(1);
                ui->spinBox_5->setMaximum(1);
                ui->dial_6->setValue(0);
                ui->dial->setDisabled(false);
                ui->dial_2->setDisabled(false);
                ui->dial_3->setDisabled(false);
                ui->dial_4->setDisabled(false);
                ui->dial_5->setDisabled(false);
                ui->dial_6->setDisabled(true);
                ui->spinBox->setDisabled(false);
                ui->spinBox_2->setDisabled(false);
                ui->spinBox_3->setDisabled(false);
                ui->spinBox_4->setDisabled(false);
                ui->spinBox_5->setDisabled(false);
                ui->spinBox_6->setDisabled(true);
                break;

            case effects::MONO_ECHO_FILTER:
            case effects::STEREO_ECHO_FILTER:
            case effects::TAPE_DELAY:
            case effects::STEREO_TAPE_DELAY:
                ui->checkBox->setDisabled(false);
                ui->dial->setMaximum(255);
                ui->spinBox->setMaximum(255);
                ui->dial_4->setMaximum(255);
                ui->spinBox_4->setMaximum(255);
                ui->dial_5->setMaximum(255);
                ui->spinBox_5->setMaximum(255);
                ui->dial->setDisabled(false);
                ui->dial_2->setDisabled(false);
                ui->dial_3->setDisabled(false);
                ui->dial_4->setDisabled(false);
                ui->dial_5->setDisabled(false);
                ui->dial_6->setDisabled(false);
                ui->spinBox->setDisabled(false);
                ui->spinBox_2->setDisabled(false);
                ui->spinBox_3->setDisabled(false);
                ui->spinBox_4->setDisabled(false);
                ui->spinBox_5->setDisabled(false);
                ui->spinBox_6->setDisabled(false);
                break;

            default:
                ui->checkBox->setDisabled(false);
                ui->dial->setMaximum(255);
                ui->spinBox->setMaximum(255);
                ui->dial_4->setMaximum(255);
                ui->spinBox_4->setMaximum(255);
                ui->dial_5->setMaximum(255);
                ui->spinBox_5->setMaximum(255);
                ui->dial_6->setValue(0);
                ui->dial->setDisabled(false);
                ui->dial_2->setDisabled(false);
                ui->dial_3->setDisabled(false);
                ui->dial_4->setDisabled(false);
                ui->dial_5->setDisabled(false);
                ui->dial_6->setDisabled(true);
                ui->spinBox->setDisabled(false);
                ui->spinBox_2->setDisabled(false);
                ui->spinBox_3->setDisabled(false);
                ui->spinBox_4->setDisabled(false);
                ui->spinBox_5->setDisabled(false);
                ui->spinBox_6->setDisabled(true);
                break;
        }

        // set knobs labels
        switch (static_cast<effects>(value))
        {
            case effects::EMPTY:
                setTexts(ui.get(),
                         UIText{
                             tr(""),
                             tr("Default effect's dial 1"),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Default effect's box 1"),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Default effect's dial 2"),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Default effect's box 2"),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Default effect's dial 3"),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Default effect's box 3"),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Default effect's dial 4"),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Default effect's box 4"),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Default effect's dial 5"),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Default effect's box 5"),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Default effect's dial 6"),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Default effect's box 6"),
                             tr("When you choose an effect you can set precise value of a parameter here")});
                break;
            case effects::OVERDRIVE:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Gain"),
                             tr("Default effect's \"Gain\" dial"),
                             tr("Allows you to set \"Gain\" parameter of this effect"),
                             tr("Default effect's \"Gain\" box"),
                             tr("Allows you to precisely set \"Gain\" parameter of this effect")},
                         UIText{
                             tr("L&ow"),
                             tr("Default effect's \"Low tones\" dial"),
                             tr("Allows you to set \"Low tones\" parameter of this effect"),
                             tr("Default effect's \"Low tones\" box"),
                             tr("Allows you to precisely set \"Low tones\" parameter of this effect")},
                         UIText{
                             tr("&Medium"),
                             tr("Default effect's \"Medium tones\" dial"),
                             tr("Allows you to set \"Medium tones\" parameter of this effect"),
                             tr("Default effect's \"Medium tones\" box"),
                             tr("Allows you to precisely set \"Medium tones\" parameter of this effect")},
                         UIText{
                             tr("&High"),
                             tr("Default effect's \"Hight tones\" dial"),
                             tr("Allows you to set \"High tones\" parameter of this effect"),
                             tr("Default effect's \"High tones\" box"),
                             tr("Allows you to precisely set \"High tones\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::WAH:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Frequency"),
                             tr("Default effect's \"Frequency\" dial"),
                             tr("Allows you to set \"Frequency\" parameter of this effect"),
                             tr("Default effect's \"Frequency\" box"),
                             tr("Allows you to precisely set \"Frequency\" parameter of this effect")},
                         UIText{
                             tr("Mi&n Freq"),
                             tr("Default effect's \"Minimum Frequency\" dial"),
                             tr("Allows you to set \"Minimum Frequency\" parameter of this effect"),
                             tr("Default effect's \"Minimum Frequency\" box"),
                             tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect")},
                         UIText{
                             tr("Ma&x Freq"),
                             tr("Default effect's \"Maximum Frequency\" dial"),
                             tr("Allows you to set \"Maximum Frequency\" parameter of this effect"),
                             tr("Default effect's \"Maximum Frequency\" box"),
                             tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Q"),
                             tr("Default effect's \"Q\" dial"),
                             tr("Allows you to set \"Q\" parameter of this effect"),
                             tr("Default effect's \"Q\" box"),
                             tr("Allows you to precisely set \"Q\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::TOUCH_WAH:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Sensivity"),
                             tr("Default effect's \"Sensivity\" dial"),
                             tr("Allows you to set \"Sensivity\" parameter of this effect"),
                             tr("Default effect's \"Sensivity\" box"),
                             tr("Allows you to precisely set \"Sensivity\" parameter of this effect")},
                         UIText{
                             tr("Mi&n Freq"),
                             tr("Default effect's \"Minimum Frequency\" dial"),
                             tr("Allows you to set \"Minimum Frequency\" parameter of this effect"),
                             tr("Default effect's \"Minimum Frequency\" box"),
                             tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect")},
                         UIText{
                             tr("Ma&x Freq"),
                             tr("Default effect's \"Maximum Frequency\" dial"),
                             tr("Allows you to set \"Maximum Frequency\" parameter of this effect"),
                             tr("Default effect's \"Maximum Frequency\" box"),
                             tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Q"),
                             tr("Default effect's \"Q\" dial"),
                             tr("Allows you to set \"Q\" parameter of this effect"),
                             tr("Default effect's \"Q\" box"),
                             tr("Allows you to precisely set \"Q\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::FUZZ:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Gain"),
                             tr("Default effect's \"Gain\" dial"),
                             tr("Allows you to set \"Gain\" parameter of this effect"),
                             tr("Default effect's \"Gain\" box"),
                             tr("Allows you to precisely set \"Gain\" parameter of this effect")},
                         UIText{
                             tr("&Octave"),
                             tr("Default effect's \"Octave\" dial"),
                             tr("Allows you to set \"Octave\" parameter of this effect"),
                             tr("Default effect's \"Octave\" box"),
                             tr("Allows you to precisely set \"Octave\" parameter of this effect")},
                         UIText{
                             tr("L&ow"),
                             tr("Default effect's \"Low tones\" dial"),
                             tr("Allows you to set \"Low tones\" parameter of this effect"),
                             tr("Default effect's \"Low tones\" box"),
                             tr("Allows you to precisely set \"Low tones\" parameter of this effect")},
                         UIText{
                             tr("&High"),
                             tr("Default effect's \"Hight tones\" dial"),
                             tr("Allows you to set \"High tones\" parameter of this effect"),
                             tr("Default effect's \"High tones\" box"),
                             tr("Allows you to precisely set \"High tones\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::FUZZ_TOUCH_WAH:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Gain"),
                             tr("Default effect's \"Gain\" dial"),
                             tr("Allows you to set \"Gain\" parameter of this effect"),
                             tr("Default effect's \"Gain\" box"),
                             tr("Allows you to precisely set \"Gain\" parameter of this effect")},
                         UIText{
                             tr("&Sensivity"),
                             tr("Default effect's \"Sensivity\" dial"),
                             tr("Allows you to set \"Sensivity\" parameter of this effect"),
                             tr("Default effect's \"Sensivity\" box"),
                             tr("Allows you to precisely set \"Sensivity\" parameter of this effect")},
                         UIText{
                             tr("&Octave"),
                             tr("Default effect's \"Octave\" dial"),
                             tr("Allows you to set \"Octave\" parameter of this effect"),
                             tr("Default effect's \"Octave\" box"),
                             tr("Allows you to precisely set \"Octave\" parameter of this effect")},
                         UIText{
                             tr("&Peak"),
                             tr("Default effect's \"Peak\" dial"),
                             tr("Allows you to set \"Peak\" parameter of this effect"),
                             tr("Default effect's \"Peak\" box"),
                             tr("Allows you to precisely set \"Peak\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::SIMPLE_COMP:
                setTexts(ui.get(),
                         UIText{
                             tr("&Type"),
                             tr("Default effect's \"Type\" dial"),
                             tr("Allows you to set \"Type\" parameter of this effect"),
                             tr("Default effect's \"Type\" box"),
                             tr("Allows you to precisely set \"Type\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::COMPRESSOR:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Threshold"),
                             tr("Default effect's \"Threshold\" dial"),
                             tr("Allows you to set \"Threshold\" parameter of this effect"),
                             tr("Default effect's \"Threshold\" box"),
                             tr("Allows you to precisely set \"Threshold\" parameter of this effect")},
                         UIText{
                             tr("&Ratio"),
                             tr("Default effect's \"Ratio\" dial"),
                             tr("Allows you to set \"Ratio\" parameter of this effect"),
                             tr("Default effect's \"Ratio\" box"),
                             tr("Allows you to precisely set \"Ratio\" parameter of this effect")},
                         UIText{
                             tr("Atta&ck"),
                             tr("Default effect's \"Attack\" dial"),
                             tr("Allows you to set \"Attack\" parameter of this effect"),
                             tr("Default effect's \"Attack\" box"),
                             tr("Allows you to precisely set \"Attack\" parameter of this effect")},
                         UIText{
                             tr("&Release"),
                             tr("Default effect's \"Release\" dial"),
                             tr("Allows you to set \"Release\" parameter of this effect"),
                             tr("Default effect's \"Release\" box"),
                             tr("Allows you to precisely set \"Release\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::SINE_CHORUS:
            case effects::TRIANGLE_CHORUS:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Default effect's \"Rate\" dial"),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Default effect's \"Rate\" box"),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Default effect's \"Depth\" dial"),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Default effect's \"Depth\" box"),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("A&vr Delay"),
                             tr("Default effect's \"Average Delay\" dial"),
                             tr("Allows you to set \"Average Delay\" parameter of this effect"),
                             tr("Default effect's \"Average Delay\" box"),
                             tr("Allows you to precisely set \"Average Delay\" parameter of this effect")},
                         UIText{
                             tr("LR &Phase"),
                             tr("Default effect's \"LR Phase\" dial"),
                             tr("Allows you to set \"LR Phase\" parameter of this effect"),
                             tr("Default effect's \"LR Phase\" box"),
                             tr("Allows you to precisely set \"LR Phase\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::SINE_FLANGER:
            case effects::TRIANGLE_FLANGER:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Default effect's \"Rate\" dial"),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Default effect's \"Rate\" box"),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Default effect's \"Depth\" dial"),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Default effect's \"Depth\" box"),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("LR &Phase"),
                             tr("Default effect's \"LR Phase\" dial"),
                             tr("Allows you to set \"LR Phase\" parameter of this effect"),
                             tr("Default effect's \"LR Phase\" box"),
                             tr("Allows you to precisely set \"LR Phase\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::VIBRATONE:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rotor"),
                             tr("Default effect's \"Rotor\" dial"),
                             tr("Allows you to set \"Rotor\" parameter of this effect"),
                             tr("Default effect's \"Rotor\" box"),
                             tr("Allows you to precisely set \"Rotor\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Default effect's \"Depth\" dial"),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Default effect's \"Depth\" box"),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("LR &Phase"),
                             tr("Default effect's \"LR Phase\" dial"),
                             tr("Allows you to set \"LR Phase\" parameter of this effect"),
                             tr("Default effect's \"LR Phase\" box"),
                             tr("Allows you to precisely set \"LR Phase\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::VINTAGE_TREMOLO:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Default effect's \"Rate\" dial"),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Default effect's \"Rate\" box"),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Duty Cycle"),
                             tr("Default effect's \"Duty Cycle\" dial"),
                             tr("Allows you to set \"Duty Cycle\" parameter of this effect"),
                             tr("Default effect's \"Duty Cycle\" box"),
                             tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect")},
                         UIText{
                             tr("Atta&ck"),
                             tr("Default effect's \"Attack\" dial"),
                             tr("Allows you to set \"Attack\" parameter of this effect"),
                             tr("Default effect's \"Attack\" box"),
                             tr("Allows you to precisely set \"Attack\" parameter of this effect")},
                         UIText{
                             tr("Relea&se"),
                             tr("Default effect's \"Release\" dial"),
                             tr("Allows you to set \"Release\" parameter of this effect"),
                             tr("Default effect's \"Release\" box"),
                             tr("Allows you to precisely set \"Release\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::SINE_TREMOLO:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Default effect's \"Rate\" dial"),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Default effect's \"Rate\" box"),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Duty Cycle"),
                             tr("Default effect's \"Duty Cycle\" dial"),
                             tr("Allows you to set \"Duty Cycle\" parameter of this effect"),
                             tr("Default effect's \"Duty Cycle\" box"),
                             tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect")},
                         UIText{
                             tr("LFO &Clipping"),
                             tr("Default effect's \"LFO Clipping\" dial"),
                             tr("Allows you to set \"LFO Clipping\" parameter of this effect"),
                             tr("Default effect's \"LFO Clipping\" box"),
                             tr("Allows you to precisely set \"LFO Clipping\" parameter of this effect")},
                         UIText{
                             tr("&Tri Shaping"),
                             tr("Default effect's \"Tri Shaping\" dial"),
                             tr("Allows you to set \"Tri Shaping\" parameter of this effect"),
                             tr("Default effect's \"Tri Shaping\" box"),
                             tr("Allows you to precisely set \"Tri Shaping\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::RING_MODULATOR:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Frequency"),
                             tr("Default effect's \"Frequency\" dial"),
                             tr("Allows you to set \"Frequency\" parameter of this effect"),
                             tr("Default effect's \"Frequency\" box"),
                             tr("Allows you to precisely set \"Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Default effect's \"Depth\" dial"),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Default effect's \"Depth\" box"),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("LFO &Shape"),
                             tr("Default effect's \"LFO Shape\" dial"),
                             tr("Allows you to set \"LFO Shape\" parameter of this effect"),
                             tr("Default effect's \"LFO Shape\" box"),
                             tr("Allows you to precisely set \"LFO Shape\" parameter of this effect")},
                         UIText{
                             tr("LFO &Phase"),
                             tr("Default effect's \"LFO Phase\" dial"),
                             tr("Allows you to set \"LFO Pase\" parameter of this effect"),
                             tr("Default effect's \"LFO Phase\" box"),
                             tr("Allows you to precisely set \"LFO Phase\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::STEP_FILTER:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Default effect's \"Rate\" dial"),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Default effect's \"Rate\" box"),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("Re&sonance"),
                             tr("Default effect's \"Resonance\" dial"),
                             tr("Allows you to set \"Resonance\" parameter of this effect"),
                             tr("Default effect's \"Resonance\" box"),
                             tr("Allows you to precisely set \"Resonance\" parameter of this effect")},
                         UIText{
                             tr("Mi&n Freq"),
                             tr("Default effect's \"Minimum Frequency\" dial"),
                             tr("Allows you to set \"Minimum Frequency\" parameter of this effect"),
                             tr("Default effect's \"Minimum Frequency\" box"),
                             tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect")},
                         UIText{
                             tr("Ma&x Freq"),
                             tr("Default effect's \"Maximum Frequency\" dial"),
                             tr("Allows you to set \"Maximum Frequency\" parameter of this effect"),
                             tr("Default effect's \"Maximum Frequency\" box"),
                             tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::PHASER:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Default effect's \"Rate\" dial"),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Default effect's \"Rate\" box"),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Default effect's \"Depth\" dial"),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Default effect's \"Depth\" box"),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("LFO &Shape"),
                             tr("Default effect's \"LFO Shape\" dial"),
                             tr("Allows you to set \"LFO Shape\" parameter of this effect"),
                             tr("Default effect's \"LFO Shape\" box"),
                             tr("Allows you to precisely set \"LFO Shape\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::PITCH_SHIFTER:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Pitch"),
                             tr("Default effect's \"Pitch\" dial"),
                             tr("Allows you to set \"Pitch\" parameter of this effect"),
                             tr("Default effect's \"Pitch\" box"),
                             tr("Allows you to precisely set \"Pitch\" parameter of this effect")},
                         UIText{
                             tr("&Detune"),
                             tr("Default effect's \"Detune\" dial"),
                             tr("Allows you to set \"Detune\" parameter of this effect"),
                             tr("Default effect's \"Detune\" box"),
                             tr("Allows you to precisely set \"Detune\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("P&redelay"),
                             tr("Default effect's \"Predelay\" dial"),
                             tr("Allows you to set \"Predelay\" parameter of this effect"),
                             tr("Default effect's \"Predelay\" box"),
                             tr("Allows you to precisely set \"Predelay\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::MONO_ECHO_FILTER:
            case effects::STEREO_ECHO_FILTER:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Default effect's \"Delay\" dial"),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Default effect's \"Delay\" box"),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("Fre&quency"),
                             tr("Default effect's \"Frequency\" dial"),
                             tr("Allows you to set \"Frequency\" parameter of this effect"),
                             tr("Default effect's \"Frequency\" box"),
                             tr("Allows you to precisely set \"Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Ressonance"),
                             tr("Default effect's \"Resonance\" dial"),
                             tr("Allows you to set \"Resonance\" parameter of this effect"),
                             tr("Default effect's \"Resonance\" box"),
                             tr("Allows you to precisely set \"Resonance\" parameter of this effect")},
                         UIText{
                             tr("&In Level"),
                             tr("Default effect's \"In Level\" dial"),
                             tr("Allows you to set \"In Level\" parameter of this effect"),
                             tr("Default effect's \"In Level\" box"),
                             tr("Allows you to precisely set \"In Level\" parameter of this effect")});
                break;
            case effects::MONO_DELAY:
            case effects::MULTITAP_DELAY:
            case effects::REVERSE_DELAY:
            case effects::PING_PONG_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Default effect's \"Delay\" dial"),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Default effect's \"Delay\" box"),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Default effect's \"Brightness\" dial"),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Default effect's \"Brightness\" box"),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")},
                         UIText{
                             tr("A&ttenuation"),
                             tr("Default effect's \"Attenuation\" dial"),
                             tr("Allows you to set \"Attenuation\" parameter of this effect"),
                             tr("Default effect's \"Attenuation\" box"),
                             tr("Allows you to precisely set \"Attenuation\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::DUCKING_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Default effect's \"Delay\" dial"),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Default effect's \"Delay\" box"),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Release"),
                             tr("Default effect's \"Release\" dial"),
                             tr("Allows you to set \"Release\" parameter of this effect"),
                             tr("Default effect's \"Release\" box"),
                             tr("Allows you to precisely set \"Release\" parameter of this effect")},
                         UIText{
                             tr("&Threshold"),
                             tr("Default effect's \"Threshold\" dial"),
                             tr("Allows you to set \"Threshold\" parameter of this effect"),
                             tr("Default effect's \"Threshold\" box"),
                             tr("Allows you to precisely set \"Threshold\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::TAPE_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Default effect's \"Delay\" dial"),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Default effect's \"Delay\" box"),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("Fl&utter"),
                             tr("Default effect's \"Flutter\" dial"),
                             tr("Allows you to set \"Flutter\" parameter of this effect"),
                             tr("Default effect's \"Flutter\" box"),
                             tr("Allows you to precisely set \"Flutter\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Default effect's \"Brightness\" dial"),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Default effect's \"Brightness\" box"),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")},
                         UIText{
                             tr("&Stereo"),
                             tr("Default effect's \"Stereo\" dial"),
                             tr("Allows you to set \"Stereo\" parameter of this effect"),
                             tr("Default effect's \"Stereo\" box"),
                             tr("Allows you to precisely set \"Stereo\" parameter of this effect")});
                break;
            case effects::STEREO_TAPE_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Default effect's \"Delay\" dial"),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Default effect's \"Delay\" box"),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Default effect's \"Feedback\" dial"),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Default effect's \"Feedback\" box"),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("Fl&utter"),
                             tr("Default effect's \"Flutter\" dial"),
                             tr("Allows you to set \"Flutter\" parameter of this effect"),
                             tr("Default effect's \"Flutter\" box"),
                             tr("Allows you to precisely set \"Flutter\" parameter of this effect")},
                         UIText{
                             tr("&Separation"),
                             tr("Default effect's \"Separation\" dial"),
                             tr("Allows you to set \"Separation\" parameter of this effect"),
                             tr("Default effect's \"Separation\" box"),
                             tr("Allows you to precisely set \"Separation\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Default effect's \"Brightness\" dial"),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Default effect's \"Brightness\" box"),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")});
                break;
            case effects::SMALL_HALL_REVERB:
            case effects::LARGE_HALL_REVERB:
            case effects::SMALL_ROOM_REVERB:
            case effects::LARGE_ROOM_REVERB:
            case effects::SMALL_PLATE_REVERB:
            case effects::LARGE_PLATE_REVERB:
            case effects::AMBIENT_REVERB:
            case effects::ARENA_REVERB:
            case effects::FENDER_63_SPRING_REVERB:
            case effects::FENDER_65_SPRING_REVERB:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Default effect's \"Level\" dial"),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Default effect's \"Level\" box"),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Decay"),
                             tr("Default effect's \"Decay\" dial"),
                             tr("Allows you to set \"Decay\" parameter of this effect"),
                             tr("Default effect's \"Decay\" box"),
                             tr("Allows you to precisely set \"Decay\" parameter of this effect")},
                         UIText{
                             tr("D&well"),
                             tr("Default effect's \"Dwell\" dial"),
                             tr("Allows you to set \"Dwell\" parameter of this effect"),
                             tr("Default effect's \"Dwell\" box"),
                             tr("Allows you to precisely set \"Dwell\" parameter of this effect")},
                         UIText{
                             tr("D&iffusion"),
                             tr("Default effect's \"Diffusion\" dial"),
                             tr("Allows you to set \"Diffusion\" parameter of this effect"),
                             tr("Default effect's \"Diffusion\" box"),
                             tr("Allows you to precisely set \"Diffusion\" parameter of this effect")},
                         UIText{
                             tr("&Tone"),
                             tr("Default effect's \"Tone\" dial"),
                             tr("Allows you to set \"Tone\" parameter of this effect"),
                             tr("Default effect's \"Tone\" box"),
                             tr("Allows you to precisely set \"Tone\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
        }
    }

    void DefaultEffects::get_settings()
    {
        std::array<fx_pedal_settings, 4> settings_data{};
        dynamic_cast<MainWindow*>(parent())->get_settings(nullptr, settings_data.data());

        const std::size_t index = static_cast<std::size_t>(ui->comboBox_2->currentIndex());
        ui->comboBox->setCurrentIndex(value(settings_data[index].effect_num));
        ui->dial->setValue(settings_data[index].knob1);
        ui->dial_2->setValue(settings_data[index].knob2);
        ui->dial_3->setValue(settings_data[index].knob3);
        ui->dial_4->setValue(settings_data[index].knob4);
        ui->dial_5->setValue(settings_data[index].knob5);
        ui->dial_6->setValue(settings_data[index].knob6);
        ui->checkBox->setChecked(settings_data[index].position == Position::effectsLoop);
    }

    void DefaultEffects::save_default_effects()
    {
        QSettings settings;

        settings.setValue(QString("DefaultEffects/Effect%1/Effect").arg(ui->comboBox_3->currentIndex()), ui->comboBox->currentIndex());
        settings.setValue(QString("DefaultEffects/Effect%1/Knob1").arg(ui->comboBox_3->currentIndex()), ui->dial->value());
        settings.setValue(QString("DefaultEffects/Effect%1/Knob2").arg(ui->comboBox_3->currentIndex()), ui->dial_2->value());
        settings.setValue(QString("DefaultEffects/Effect%1/Knob3").arg(ui->comboBox_3->currentIndex()), ui->dial_3->value());
        settings.setValue(QString("DefaultEffects/Effect%1/Knob4").arg(ui->comboBox_3->currentIndex()), ui->dial_4->value());
        settings.setValue(QString("DefaultEffects/Effect%1/Knob5").arg(ui->comboBox_3->currentIndex()), ui->dial_5->value());
        settings.setValue(QString("DefaultEffects/Effect%1/Knob6").arg(ui->comboBox_3->currentIndex()), ui->dial_6->value());
        settings.setValue(QString("DefaultEffects/Effect%1/Post amp").arg(ui->comboBox_3->currentIndex()), ui->checkBox->checkState() != 0u);
    }
}

#include "ui/moc_defaulteffects.moc"
