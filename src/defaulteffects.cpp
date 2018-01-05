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

#include "defaulteffects.h"
#include "ui_defaulteffects.h"

#include "mainwindow.h"

DefaultEffects::DefaultEffects(QWidget *parent) :
    QDialog(parent),
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
            if(sender() == ui->comboBox)
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
            ui->label->setText(tr(""));
            ui->dial->setAccessibleName(tr("Default effect's dial 1"));
            ui->dial->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
            ui->spinBox->setAccessibleName(tr("Default effect's box 1"));
            ui->spinBox->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

            ui->label_2->setText(tr(""));
            ui->dial_2->setAccessibleName(tr("Default effect's dial 2"));
            ui->dial_2->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's box 2"));
            ui->spinBox_2->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

            ui->label_3->setText(tr(""));
            ui->dial_3->setAccessibleName(tr("Default effect's dial 3"));
            ui->dial_3->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's box 3"));
            ui->spinBox_3->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

            ui->label_4->setText(tr(""));
            ui->dial_4->setAccessibleName(tr("Default effect's dial 4"));
            ui->dial_4->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's box 4"));
            ui->spinBox_4->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

            ui->label_5->setText(tr(""));
            ui->dial_5->setAccessibleName(tr("Default effect's dial 5"));
            ui->dial_5->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's box 5"));
            ui->spinBox_5->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Default effect's dial 6"));
            ui->dial_6->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
            ui->spinBox_6->setAccessibleName(tr("Default effect's box 6"));
            ui->spinBox_6->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));
            break;

        case effects::OVERDRIVE:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Gain"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Gain\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Gain\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Gain\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Gain\" parameter of this effect"));

            ui->label_3->setText(tr("L&ow"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Low tones\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Low tones\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Low tones\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Low tones\" parameter of this effect"));

            ui->label_4->setText(tr("&Medium"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Medium tones\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Medium tones\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Medium tones\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Medium tones\" parameter of this effect"));

            ui->label_5->setText(tr("&High"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Hight tones\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"High tones\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"High tones\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"High tones\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::WAH:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Frequency"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Frequency\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Frequency\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Frequency\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Frequency\" parameter of this effect"));

            ui->label_3->setText(tr("Mi&n Freq"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Minimum Frequency\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Minimum Frequency\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Minimum Frequency\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect"));

            ui->label_4->setText(tr("Ma&x Freq"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Maximum Frequency\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Maximum Frequency\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Maximum Frequency\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect"));

            ui->label_5->setText(tr("&Q"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Q\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Q\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Q\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Q\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::TOUCH_WAH:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Sensivity"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Sensivity\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Sensivity\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Sensivity\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Sensivity\" parameter of this effect"));

            ui->label_3->setText(tr("Mi&n Freq"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Minimum Frequency\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Minimum Frequency\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Minimum Frequency\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect"));

            ui->label_4->setText(tr("Ma&x Freq"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Maximum Frequency\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Maximum Frequency\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Maximum Frequency\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect"));

            ui->label_5->setText(tr("&Q"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Q\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Q\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Q\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Q\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::FUZZ:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Gain"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Gain\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Gain\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Gain\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Gain\" parameter of this effect"));

            ui->label_3->setText(tr("&Octave"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Octave\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Octave\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Octave\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Octave\" parameter of this effect"));

            ui->label_4->setText(tr("L&ow"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Low tones\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Low tones\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Low tones\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Low tones\" parameter of this effect"));

            ui->label_5->setText(tr("&High"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Hight tones\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"High tones\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"High tones\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"High tones\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::FUZZ_TOUCH_WAH:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Gain"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Gain\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Gain\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Gain\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Gain\" parameter of this effect"));

            ui->label_3->setText(tr("&Sensivity"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Sensivity\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Sensivity\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Sensivity\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Sensivity\" parameter of this effect"));

            ui->label_4->setText(tr("&Octave"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Octave\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Octave\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Octave\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Octave\" parameter of this effect"));

            ui->label_5->setText(tr("&Peak"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Peak\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Peak\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Peak\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Peak\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::SIMPLE_COMP:
            ui->label->setText(tr("&Type"));
            ui->dial->setAccessibleName(tr("Default effect's \"Type\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Type\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Type\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Type\" parameter of this effect"));

            ui->label_2->setText(tr(""));
            ui->dial_2->setAccessibleName(tr("Disabled dial"));
            ui->dial_2->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_2->setAccessibleName(tr("Disabled box"));
            ui->spinBox_2->setAccessibleDescription(tr("This box is disabled in this effect"));

            ui->label_3->setText(tr(""));
            ui->dial_3->setAccessibleName(tr("Disabled dial"));
            ui->dial_3->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_3->setAccessibleName(tr("Disabled box"));
            ui->spinBox_3->setAccessibleDescription(tr("This box is disabled in this effect"));

            ui->label_4->setText(tr(""));
            ui->dial_4->setAccessibleName(tr("Disabled dial"));
            ui->dial_4->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_4->setAccessibleName(tr("Disabled box"));
            ui->spinBox_4->setAccessibleDescription(tr("This box is disabled in this effect"));

            ui->label_5->setText(tr(""));
            ui->dial_5->setAccessibleName(tr("Disabled dial"));
            ui->dial_5->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_5->setAccessibleName(tr("Disabled box"));
            ui->spinBox_5->setAccessibleDescription(tr("This box is disabled in this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::COMPRESSOR:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Threshold"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Threshold\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Threshold\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Threshold\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Threshold\" parameter of this effect"));

            ui->label_3->setText(tr("&Ratio"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Ratio\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Ratio\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Ratio\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Ratio\" parameter of this effect"));

            ui->label_4->setText(tr("Atta&ck"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Attack\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Attack\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Attack\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Attack\" parameter of this effect"));

            ui->label_5->setText(tr("&Release"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Release\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Release\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Release\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Release\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::SINE_CHORUS:
        case effects::TRIANGLE_CHORUS:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rate"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rate\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rate\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

            ui->label_3->setText(tr("&Depth"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Depth\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Depth\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

            ui->label_4->setText(tr("A&vr Delay"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Average Delay\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Average Delay\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Average Delay\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Average Delay\" parameter of this effect"));

            ui->label_5->setText(tr("LR &Phase"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"LR Phase\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LR Phase\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"LR Phase\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LR Phase\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::SINE_FLANGER:
        case effects::TRIANGLE_FLANGER:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rate"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rate\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rate\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

            ui->label_3->setText(tr("&Depth"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Depth\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Depth\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

            ui->label_4->setText(tr("&Feedback"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_5->setText(tr("LR &Phase"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"LR Phase\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LR Phase\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"LR Phase\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LR Phase\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::VIBRATONE:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rotor"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rotor\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rotor\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rotor\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rotor\" parameter of this effect"));

            ui->label_3->setText(tr("&Depth"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Depth\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Depth\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

            ui->label_4->setText(tr("&Feedback"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_5->setText(tr("LR &Phase"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"LR Phase\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LR Phase\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"LR Phase\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LR Phase\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::VINTAGE_TREMOLO:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rate"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rate\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rate\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

            ui->label_3->setText(tr("&Duty Cycle"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Duty Cycle\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Duty Cycle\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Duty Cycle\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect"));

            ui->label_4->setText(tr("Atta&ck"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Attack\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Attack\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Attack\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Attack\" parameter of this effect"));

            ui->label_5->setText(tr("Relea&se"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Release\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Release\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Release\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Release\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::SINE_TREMOLO:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rate"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rate\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rate\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

            ui->label_3->setText(tr("&Duty Cycle"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Duty Cycle\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Duty Cycle\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Duty Cycle\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect"));

            ui->label_4->setText(tr("LFO &Clipping"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"LFO Clipping\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"LFO Clipping\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"LFO Clipping\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"LFO Clipping\" parameter of this effect"));

            ui->label_5->setText(tr("&Tri Shaping"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Tri Shaping\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Tri Shaping\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Tri Shaping\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Tri Shaping\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::RING_MODULATOR:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Frequency"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Frequency\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Frequency\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Frequency\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Frequency\" parameter of this effect"));

            ui->label_3->setText(tr("&Depth"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Depth\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Depth\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

            ui->label_4->setText(tr("LFO &Shape"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"LFO Shape\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"LFO Shape\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"LFO Shape\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"LFO Shape\" parameter of this effect"));

            ui->label_5->setText(tr("LFO &Phase"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"LFO Phase\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LFO Pase\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"LFO Phase\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LFO Phase\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::STEP_FILTER:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rate"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rate\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rate\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

            ui->label_3->setText(tr("Re&sonance"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Resonance\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Resonance\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Resonance\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Resonance\" parameter of this effect"));

            ui->label_4->setText(tr("Mi&n Freq"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Minimum Frequency\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Minimum Frequency\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Minimum Frequency\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect"));

            ui->label_5->setText(tr("Ma&x Freq"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Maximum Frequency\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Maximum Frequency\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Maximum Frequency\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::PHASER:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Rate"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Rate\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Rate\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

            ui->label_3->setText(tr("&Depth"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Depth\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Depth\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

            ui->label_4->setText(tr("&Feedback"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_5->setText(tr("LFO &Shape"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"LFO Shape\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LFO Shape\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"LFO Shape\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LFO Shape\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::PITCH_SHIFTER:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Pitch"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Pitch\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Pitch\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Pitch\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Pitch\" parameter of this effect"));

            ui->label_3->setText(tr("&Detune"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Detune\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Detune\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Detune\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Detune\" parameter of this effect"));

            ui->label_4->setText(tr("&Feedback"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_5->setText(tr("P&redelay"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Predelay\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Predelay\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Predelay\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Predelay\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::MONO_ECHO_FILTER:
        case effects::STEREO_ECHO_FILTER:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Delay"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Delay\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Delay\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

            ui->label_3->setText(tr("&Feedback"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_4->setText(tr("Fre&quency"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Frequency\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Frequency\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Frequency\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Frequency\" parameter of this effect"));

            ui->label_5->setText(tr("&Ressonance"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Resonance\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Resonance\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Resonance\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Resonance\" parameter of this effect"));

            ui->label_6->setText(tr("&In Level"));
            ui->dial_6->setAccessibleName(tr("Default effect's \"In Level\" dial"));
            ui->dial_6->setAccessibleDescription(tr("Allows you to set \"In Level\" parameter of this effect"));
            ui->spinBox_6->setAccessibleName(tr("Default effect's \"In Level\" box"));
            ui->spinBox_6->setAccessibleDescription(tr("Allows you to precisely set \"In Level\" parameter of this effect"));
            break;

        case effects::MONO_DELAY:
        case effects::MULTITAP_DELAY:
        case effects::REVERSE_DELAY:
        case effects::PING_PONG_DELAY:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Delay"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Delay\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Delay\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

            ui->label_3->setText(tr("&Feedback"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_4->setText(tr("&Brightness"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Brightness\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Brightness\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));

            ui->label_5->setText(tr("A&ttenuation"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Attenuation\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Attenuation\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Attenuation\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Attenuation\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::DUCKING_DELAY:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Delay"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Delay\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Delay\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

            ui->label_3->setText(tr("&Feedback"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_4->setText(tr("&Release"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Release\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Release\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Release\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Release\" parameter of this effect"));

            ui->label_5->setText(tr("&Threshold"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Threshold\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Threshold\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Threshold\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Threshold\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;

        case effects::TAPE_DELAY:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Delay"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Delay\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Delay\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

            ui->label_3->setText(tr("&Feedback"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_4->setText(tr("Fl&utter"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Flutter\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Flutter\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Flutter\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Flutter\" parameter of this effect"));

            ui->label_5->setText(tr("&Brightness"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Brightness\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Brightness\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));

            ui->label_6->setText(tr("&Stereo"));
            ui->dial_6->setAccessibleName(tr("Default effect's \"Stereo\" dial"));
            ui->dial_6->setAccessibleDescription(tr("Allows you to set \"Stereo\" parameter of this effect"));
            ui->spinBox_6->setAccessibleName(tr("Default effect's \"Stereo\" box"));
            ui->spinBox_6->setAccessibleDescription(tr("Allows you to precisely set \"Stereo\" parameter of this effect"));
            break;

        case effects::STEREO_TAPE_DELAY:
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Delay"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Delay\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Delay\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

            ui->label_3->setText(tr("&Feedback"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Feedback\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Feedback\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

            ui->label_4->setText(tr("Fl&utter"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Flutter\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Flutter\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Flutter\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Flutter\" parameter of this effect"));

            ui->label_5->setText(tr("&Separation"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Separation\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Separation\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Separation\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Separation\" parameter of this effect"));

            ui->label_6->setText(tr("&Brightness"));
            ui->dial_6->setAccessibleName(tr("Default effect's \"Brightness\" dial"));
            ui->dial_6->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
            ui->spinBox_6->setAccessibleName(tr("Default effect's \"Brightness\" box"));
            ui->spinBox_6->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));
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
            ui->label->setText(tr("&Level"));
            ui->dial->setAccessibleName(tr("Default effect's \"Level\" dial"));
            ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
            ui->spinBox->setAccessibleName(tr("Default effect's \"Level\" box"));
            ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

            ui->label_2->setText(tr("&Decay"));
            ui->dial_2->setAccessibleName(tr("Default effect's \"Decay\" dial"));
            ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Decay\" parameter of this effect"));
            ui->spinBox_2->setAccessibleName(tr("Default effect's \"Decay\" box"));
            ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Decay\" parameter of this effect"));

            ui->label_3->setText(tr("D&well"));
            ui->dial_3->setAccessibleName(tr("Default effect's \"Dwell\" dial"));
            ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Dwell\" parameter of this effect"));
            ui->spinBox_3->setAccessibleName(tr("Default effect's \"Dwell\" box"));
            ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Dwell\" parameter of this effect"));

            ui->label_4->setText(tr("D&iffusion"));
            ui->dial_4->setAccessibleName(tr("Default effect's \"Diffusion\" dial"));
            ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Diffusion\" parameter of this effect"));
            ui->spinBox_4->setAccessibleName(tr("Default effect's \"Diffusion\" box"));
            ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Diffusion\" parameter of this effect"));

            ui->label_5->setText(tr("&Tone"));
            ui->dial_5->setAccessibleName(tr("Default effect's \"Tone\" dial"));
            ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Tone\" parameter of this effect"));
            ui->spinBox_5->setAccessibleName(tr("Default effect's \"Tone\" box"));
            ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Tone\" parameter of this effect"));

            ui->label_6->setText(tr(""));
            ui->dial_6->setAccessibleName(tr("Disabled dial"));
            ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
            ui->spinBox_6->setAccessibleName(tr("Disabled box"));
            ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
            break;
    }
}

void DefaultEffects::get_settings()
{
    fx_pedal_settings settings_data[4];

    dynamic_cast<MainWindow*>(parent())->get_settings(nullptr, settings_data);

    ui->comboBox->setCurrentIndex(settings_data[ui->comboBox_2->currentIndex()].effect_num);
    ui->dial->setValue(settings_data[ui->comboBox_2->currentIndex()].knob1);
    ui->dial_2->setValue(settings_data[ui->comboBox_2->currentIndex()].knob2);
    ui->dial_3->setValue(settings_data[ui->comboBox_2->currentIndex()].knob3);
    ui->dial_4->setValue(settings_data[ui->comboBox_2->currentIndex()].knob4);
    ui->dial_5->setValue(settings_data[ui->comboBox_2->currentIndex()].knob5);
    ui->dial_6->setValue(settings_data[ui->comboBox_2->currentIndex()].knob6);
    ui->checkBox->setChecked(settings_data[ui->comboBox_2->currentIndex()].put_post_amp);
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
    settings.setValue(QString("DefaultEffects/Effect%1/Post amp").arg(ui->comboBox_3->currentIndex()), ui->checkBox->checkState()?true:false);
}

#include "moc_defaulteffects.moc"
