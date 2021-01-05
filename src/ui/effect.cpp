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

#include "ui/effect.h"
#include "ui/mainwindow.h"
#include "ui_effect.h"
#include <QShortcut>
#include <QSettings>

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

        void setTexts(const Ui::Effect* ui, const UIText& e1, const UIText& e2, const UIText& e3, const UIText& e4, const UIText& e5, const UIText& e6)
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


    Effect::Effect(QWidget* parent, std::uint8_t fxSlot)
        : QMainWindow(parent),
          ui(std::make_unique<Ui::Effect>()),
          fx_slot(fxSlot),
          effect_num(effects::EMPTY),
          knob1(0),
          knob2(0),
          knob3(0),
          knob4(0),
          knob5(0),
          knob6(0),
          position(Position::input),
          enabled(true),
          changed(false)
    {
        ui->setupUi(this);
        effect_num = static_cast<effects>(ui->comboBox->currentIndex());

        // load window size
        QSettings settings;
        restoreGeometry(settings.value(QString("Windows/Effect%1WindowGeometry").arg(fx_slot)).toByteArray());

        // set window title
        setWindowTitle(tr("FX%1: EMPTY").arg(fx_slot + 1));

        setAccessibleName(tr("Effect's %1 window: EMPTY").arg(fx_slot + 1));
        setAccessibleDescription(tr("Here you can choose which effect should be emulated on this slot and it's parameters"));
        ui->checkBox->setAccessibleName(tr("Put effect %1 after amplifier").arg(fx_slot + 1));
        ui->checkBox->setAccessibleDescription(tr("Virtually put this effect after amplifier's emulator"));
        ui->setButton->setAccessibleName(tr("Effect's %1 set button").arg(fx_slot + 1));
        ui->setButton->setAccessibleDescription(tr("Send effect's %1 settings to the amplifier").arg(fx_slot + 1));
        ui->pushButton->setAccessibleName(tr("Effect's %1 On/Off button").arg(fx_slot + 1));
        ui->pushButton->setAccessibleDescription(tr("Set effect %1 on or off").arg(fx_slot + 1));
        ui->comboBox->setAccessibleName(tr("Choose effect's %1 effect").arg(fx_slot + 1));
        ui->comboBox->setAccessibleDescription(tr("Allows you to choose which effect should be emulated on this slot"));

        // connect elements to slots
        connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_fx(int)));
        connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(set_post_amp(bool)));
        connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(set_knob1(int)));
        connect(ui->dial_2, SIGNAL(valueChanged(int)), this, SLOT(set_knob2(int)));
        connect(ui->dial_3, SIGNAL(valueChanged(int)), this, SLOT(set_knob3(int)));
        connect(ui->dial_4, SIGNAL(valueChanged(int)), this, SLOT(set_knob4(int)));
        connect(ui->dial_5, SIGNAL(valueChanged(int)), this, SLOT(set_knob5(int)));
        connect(ui->dial_6, SIGNAL(valueChanged(int)), this, SLOT(set_knob6(int)));
        connect(ui->setButton, SIGNAL(clicked()), this, SLOT(send_fx()));
        connect(ui->pushButton, SIGNAL(toggled(bool)), this, SLOT(off_switch(bool)));

        QShortcut* close = new QShortcut(QKeySequence(Qt::Key_Escape), this);
        connect(close, SIGNAL(activated()), this, SLOT(close()));

        QShortcut* off = new QShortcut(QKeySequence(QString("F%1").arg(fx_slot + 1)), this, nullptr, nullptr, Qt::ApplicationShortcut);
        connect(off, SIGNAL(activated()), ui->pushButton, SLOT(toggle()));

        QShortcut* default_fx = new QShortcut(QKeySequence(QString("F%1").arg(fx_slot + 5)), this, nullptr, nullptr, Qt::ApplicationShortcut);
        connect(default_fx, SIGNAL(activated()), this, SLOT(load_default_fx()));
    }

    Effect::~Effect()
    {
        QSettings settings;
        settings.setValue(QString("Windows/Effect%1WindowGeometry").arg(fx_slot), saveGeometry());
    }

    // functions setting variables
    void Effect::set_post_amp(bool value)
    {
        position = (value == true ? Position::effectsLoop : Position::input);
        set_changed(true);
    }

    void Effect::set_knob1(int value)
    {
        knob1 = static_cast<std::uint8_t>(value);
        set_changed(true);
    }

    void Effect::set_knob2(int value)
    {
        knob2 = static_cast<std::uint8_t>(value);
        set_changed(true);
    }

    void Effect::set_knob3(int value)
    {
        knob3 = static_cast<std::uint8_t>(value);
        set_changed(true);
    }

    void Effect::set_knob4(int value)
    {
        knob4 = static_cast<std::uint8_t>(value);
        set_changed(true);
    }

    void Effect::set_knob5(int value)
    {
        knob5 = static_cast<std::uint8_t>(value);
        set_changed(true);
    }

    void Effect::set_knob6(int value)
    {
        knob6 = static_cast<std::uint8_t>(value);
        set_changed(true);
    }

    void Effect::choose_fx(int value)
    {
        QSettings settings;
        effect_num = static_cast<effects>(value);
        set_changed(true);

        ui->comboBox->setCurrentIndex(value);
        if (value != 0)
        {
            dynamic_cast<MainWindow*>(parent())->empty_other(value, this);
        }

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

            case effects::MULTITAP_DELAY:
                ui->checkBox->setDisabled(false);
                ui->dial->setMaximum(255);
                ui->spinBox->setMaximum(255);
                ui->dial_4->setMaximum(255);
                ui->spinBox_4->setMaximum(255);
                ui->dial_5->setMaximum(3);
                ui->spinBox_5->setMaximum(3);
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

        // change window title
        switch (static_cast<effects>(value))
        {
            case effects::EMPTY:
                setTitleTexts(fx_slot + 1, "EMPTY");
                break;

            case effects::OVERDRIVE:
                setTitleTexts(fx_slot + 1, "Overdrive");
                break;

            case effects::WAH:
                setTitleTexts(fx_slot + 1, "Wah");
                break;

            case effects::TOUCH_WAH:
                setTitleTexts(fx_slot + 1, "Touch Wah");
                break;

            case effects::FUZZ:
                setTitleTexts(fx_slot + 1, "Fuzz");
                break;

            case effects::FUZZ_TOUCH_WAH:
                setTitleTexts(fx_slot + 1, "Fuzz Touch Wah");
                break;

            case effects::SIMPLE_COMP:
                setTitleTexts(fx_slot + 1, "Simple Compressor");
                break;

            case effects::COMPRESSOR:
                setTitleTexts(fx_slot + 1, "Compressor");
                break;

            case effects::SINE_CHORUS:
                setTitleTexts(fx_slot + 1, "Sine Chorus");
                break;

            case effects::TRIANGLE_CHORUS:
                setTitleTexts(fx_slot + 1, "Triangle Chorus");
                break;

            case effects::SINE_FLANGER:
                setTitleTexts(fx_slot + 1, "Sine Flanger");
                break;

            case effects::TRIANGLE_FLANGER:
                setTitleTexts(fx_slot + 1, "Triangle Flanger");
                break;

            case effects::VIBRATONE:
                setTitleTexts(fx_slot + 1, "Vibratone");
                break;

            case effects::VINTAGE_TREMOLO:
                setTitleTexts(fx_slot + 1, "Vintage Tremolo");
                break;

            case effects::SINE_TREMOLO:
                setTitleTexts(fx_slot + 1, "Sine Tremolo");
                break;

            case effects::RING_MODULATOR:
                setTitleTexts(fx_slot + 1, "Ring Modulator");
                break;

            case effects::STEP_FILTER:
                setTitleTexts(fx_slot + 1, "Step Filter");
                break;

            case effects::PHASER:
                setTitleTexts(fx_slot + 1, "Phaser");
                break;

            case effects::PITCH_SHIFTER:
                setTitleTexts(fx_slot + 1, "Pitch Shifter");
                break;

            case effects::MONO_DELAY:
                setTitleTexts(fx_slot + 1, "Mono Delay");
                break;

            case effects::MONO_ECHO_FILTER:
                setTitleTexts(fx_slot + 1, "Mono Echo Filter");
                break;

            case effects::STEREO_ECHO_FILTER:
                setTitleTexts(fx_slot + 1, "Stereo Echo Filter");
                break;

            case effects::MULTITAP_DELAY:
                setTitleTexts(fx_slot + 1, "Multitap Delay");
                break;

            case effects::PING_PONG_DELAY:
                setTitleTexts(fx_slot + 1, "Ping-Pong Delay");
                break;

            case effects::DUCKING_DELAY:
                setTitleTexts(fx_slot + 1, "Ducking Delay");
                break;

            case effects::REVERSE_DELAY:
                setTitleTexts(fx_slot + 1, "Reverse Delay");
                break;

            case effects::TAPE_DELAY:
                setTitleTexts(fx_slot + 1, "Tape Delay");
                break;

            case effects::STEREO_TAPE_DELAY:
                setTitleTexts(fx_slot + 1, "Stereo Tape Delay");
                break;

            case effects::SMALL_HALL_REVERB:
                setTitleTexts(fx_slot + 1, "Small Hall Reverb");
                break;

            case effects::LARGE_HALL_REVERB:
                setTitleTexts(fx_slot + 1, "Large Hall Reverb");
                break;

            case effects::SMALL_ROOM_REVERB:
                setTitleTexts(fx_slot + 1, "Small Room Reverb");
                break;

            case effects::LARGE_ROOM_REVERB:
                setTitleTexts(fx_slot + 1, "Large Room Reverb");
                break;

            case effects::SMALL_PLATE_REVERB:
                setTitleTexts(fx_slot + 1, "Small Plate Reverb");
                break;

            case effects::LARGE_PLATE_REVERB:
                setTitleTexts(fx_slot + 1, "Large Plate Reverb");
                break;

            case effects::AMBIENT_REVERB:
                setTitleTexts(fx_slot + 1, "Ambient Reverb");
                break;

            case effects::ARENA_REVERB:
                setTitleTexts(fx_slot + 1, "Arena Reverb");
                break;

            case effects::FENDER_63_SPRING_REVERB:
                setTitleTexts(fx_slot + 1, "Fender '63 Spring Reverb");
                break;

            case effects::FENDER_65_SPRING_REVERB:
                setTitleTexts(fx_slot + 1, "Fender '65 Spring Reverb");
                break;
        }

        // set knobs labels and accessibility informations
        switch (static_cast<effects>(value))
        {
            case effects::EMPTY:
                setTexts(ui.get(),
                         UIText{
                             tr(""),
                             tr("Effect's %1 dial 1").arg(fx_slot + 1),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Effect's %1 box 1").arg(fx_slot + 1),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Effect's %1 dial 2").arg(fx_slot + 1),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Effect's %1 box 2").arg(fx_slot + 1),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Effect's %1 dial 3").arg(fx_slot + 1),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Effect's %1 box 3").arg(fx_slot + 1),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Effect's %1 dial 4").arg(fx_slot + 1),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Effect's %1 box 4").arg(fx_slot + 1),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Effect's %1 dial 5").arg(fx_slot + 1),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Effect's %1 box 5").arg(fx_slot + 1),
                             tr("When you choose an effect you can set precise value of a parameter here")},
                         UIText{
                             tr(""),
                             tr("Effect's %1 dial 6").arg(fx_slot + 1),
                             tr("When you choose an effect you can set value of a parameter here"),
                             tr("Effect's %1 box 6").arg(fx_slot + 1),
                             tr("When you choose an effect you can set precise value of a parameter here")});
                break;
            case effects::OVERDRIVE:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Gain"),
                             tr("Effect's %1 \"Gain\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Gain\" parameter of this effect"),
                             tr("Effect's %1 \"Gain\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Gain\" parameter of this effect")},
                         UIText{
                             tr("L&ow"),
                             tr("Effect's %1 \"Low tones\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Low tones\" parameter of this effect"),
                             tr("Effect's %1 \"Low tones\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Low tones\" parameter of this effect")},
                         UIText{
                             tr("&Medium"),
                             tr("Effect's %1 \"Medium tones\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Medium tones\" parameter of this effect"),
                             tr("Effect's %1 \"Medium tones\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Medium tones\" parameter of this effect")},
                         UIText{
                             tr("&High"),
                             tr("Effect's %1 \"Hight tones\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"High tones\" parameter of this effect"),
                             tr("Effect's %1 \"High tones\" box").arg(fx_slot + 1),
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
                             tr("&Mix"),
                             tr("Effect's %1 \"Mix\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Mix\" parameter of this effect"),
                             tr("Effect's %1 \"Mix\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Mix\" parameter of this effect")},
                         UIText{
                             tr("&Frequency"),
                             tr("Effect's %1 \"Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Heel Freq"),
                             tr("Effect's %1 \"Heel Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Heel Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Heel Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Heel Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Toe Freq"),
                             tr("Effect's %1 \"Toe Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Toe Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Toe Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Toe Frequency\" parameter of this effect")},
                         UIText{
                             tr("High &Q"),
                             tr("Effect's %1 \"High Q\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"High Q\" parameter of this effect"),
                             tr("Effect's %1 \"High Q\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"High Q\" parameter of this effect")},
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
                             tr("&Mix"),
                             tr("Effect's %1 \"Mix\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Mix\" parameter of this effect"),
                             tr("Effect's %1 \"Mix\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Mix\" parameter of this effect")},
                         UIText{
                             tr("&Sensivity"),
                             tr("Effect's %1 \"Sensivity\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Sensivity\" parameter of this effect"),
                             tr("Effect's %1 \"Sensivity\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Sensivity\" parameter of this effect")},
                         UIText{
                             tr("&Heel Freq"),
                             tr("Effect's %1 \"Heel Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Heel Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Heel Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Heel Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Toe Freq"),
                             tr("Effect's %1 \"Toe Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Toe Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Toe Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Toe Frequency\" parameter of this effect")},
                         UIText{
                             tr("High &Q"),
                             tr("Effect's %1 \"High Q\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"High Q\" parameter of this effect"),
                             tr("Effect's %1 \"High Q\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"High Q\" parameter of this effect")},
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Gain"),
                             tr("Effect's %1 \"Gain\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Gain\" parameter of this effect"),
                             tr("Effect's %1 \"Gain\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Gain\" parameter of this effect")},
                         UIText{
                             tr("&Octave"),
                             tr("Effect's %1 \"Octave\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Octave\" parameter of this effect"),
                             tr("Effect's %1 \"Octave\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Octave\" parameter of this effect")},
                         UIText{
                             tr("L&ow"),
                             tr("Effect's %1 \"Low tones\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Low tones\" parameter of this effect"),
                             tr("Effect's %1 \"Low tones\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Low tones\" parameter of this effect")},
                         UIText{
                             tr("&High"),
                             tr("Effect's %1 \"Hight tones\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"High tones\" parameter of this effect"),
                             tr("Effect's %1 \"High tones\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Gain"),
                             tr("Effect's %1 \"Gain\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Gain\" parameter of this effect"),
                             tr("Effect's %1 \"Gain\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Gain\" parameter of this effect")},
                         UIText{
                             tr("&Sensivity"),
                             tr("Effect's %1 \"Sensivity\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Sensivity\" parameter of this effect"),
                             tr("Effect's %1 \"Sensivity\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Sensivity\" parameter of this effect")},
                         UIText{
                             tr("&Octave"),
                             tr("Effect's %1 \"Octave\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Octave\" parameter of this effect"),
                             tr("Effect's %1 \"Octave\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Octave\" parameter of this effect")},
                         UIText{
                             tr("&Peak"),
                             tr("Effect's %1 \"Peak\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Peak\" parameter of this effect"),
                             tr("Effect's %1 \"Peak\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Type\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Type\" parameter of this effect"),
                             tr("Effect's %1 \"Type\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Threshold"),
                             tr("Effect's %1 \"Threshold\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Threshold\" parameter of this effect"),
                             tr("Effect's %1 \"Threshold\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Threshold\" parameter of this effect")},
                         UIText{
                             tr("&Ratio"),
                             tr("Effect's %1 \"Ratio\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Ratio\" parameter of this effect"),
                             tr("Effect's %1 \"Ratio\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Ratio\" parameter of this effect")},
                         UIText{
                             tr("Atta&ck"),
                             tr("Effect's %1 \"Attack\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Attack\" parameter of this effect"),
                             tr("Effect's %1 \"Attack\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Attack\" parameter of this effect")},
                         UIText{
                             tr("&Release"),
                             tr("Effect's %1 \"Release\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Release\" parameter of this effect"),
                             tr("Effect's %1 \"Release\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("A&vr Delay"),
                             tr("Effect's %1 \"Average Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Average Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Average Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Average Delay\" parameter of this effect")},
                         UIText{
                             tr("LR &Phase"),
                             tr("Effect's %1 \"LR Phase\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"LR Phase\" parameter of this effect"),
                             tr("Effect's %1 \"LR Phase\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("LR &Phase"),
                             tr("Effect's %1 \"LR Phase\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"LR Phase\" parameter of this effect"),
                             tr("Effect's %1 \"LR Phase\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rotor"),
                             tr("Effect's %1 \"Rotor\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rotor\" parameter of this effect"),
                             tr("Effect's %1 \"Rotor\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rotor\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("LR &Phase"),
                             tr("Effect's %1 \"LR Phase\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"LR Phase\" parameter of this effect"),
                             tr("Effect's %1 \"LR Phase\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Duty Cycle"),
                             tr("Effect's %1 \"Duty Cycle\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Duty Cycle\" parameter of this effect"),
                             tr("Effect's %1 \"Duty Cycle\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect")},
                         UIText{
                             tr("Atta&ck"),
                             tr("Effect's %1 \"Attack\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Attack\" parameter of this effect"),
                             tr("Effect's %1 \"Attack\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Attack\" parameter of this effect")},
                         UIText{
                             tr("Relea&se"),
                             tr("Effect's %1 \"Release\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Release\" parameter of this effect"),
                             tr("Effect's %1 \"Release\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Duty Cycle"),
                             tr("Effect's %1 \"Duty Cycle\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Duty Cycle\" parameter of this effect"),
                             tr("Effect's %1 \"Duty Cycle\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect")},
                         UIText{
                             tr("LFO &Clipping"),
                             tr("Effect's %1 \"LFO Clipping\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"LFO Clipping\" parameter of this effect"),
                             tr("Effect's %1 \"LFO Clipping\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"LFO Clipping\" parameter of this effect")},
                         UIText{
                             tr("&Shape"),
                             tr("Effect's %1 \"Shape\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Shape\" parameter of this effect"),
                             tr("Effect's %1 \"Shape\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Shape\" parameter of this effect")},
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Frequency"),
                             tr("Effect's %1 \"Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Shape"),
                             tr("Effect's %1 \"Shape\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Shape\" parameter of this effect"),
                             tr("Effect's %1 \"Shape\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Shape\" parameter of this effect")},
                         UIText{
                             tr("&Phase"),
                             tr("Effect's %1 \"Phase\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Pase\" parameter of this effect"),
                             tr("Effect's %1 \"Phase\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Phase\" parameter of this effect")},
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("Re&sonance"),
                             tr("Effect's %1 \"Resonance\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Resonance\" parameter of this effect"),
                             tr("Effect's %1 \"Resonance\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Resonance\" parameter of this effect")},
                         UIText{
                             tr("Mi&n Freq"),
                             tr("Effect's %1 \"Minimum Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Minimum Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Minimum Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect")},
                         UIText{
                             tr("Ma&x Freq"),
                             tr("Effect's %1 \"Maximum Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Maximum Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Maximum Frequency\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Rate"),
                             tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Rate\" parameter of this effect"),
                             tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Rate\" parameter of this effect")},
                         UIText{
                             tr("&Depth"),
                             tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Depth\" parameter of this effect"),
                             tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Depth\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Shape"),
                             tr("Effect's %1 \"Shape\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Shape\" parameter of this effect"),
                             tr("Effect's %1 \"Shape\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Shape\" parameter of this effect")},
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Pitch"),
                             tr("Effect's %1 \"Pitch\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Pitch\" parameter of this effect"),
                             tr("Effect's %1 \"Pitch\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Pitch\" parameter of this effect")},
                         UIText{
                             tr("&Detune"),
                             tr("Effect's %1 \"Detune\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Detune\" parameter of this effect"),
                             tr("Effect's %1 \"Detune\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Detune\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("P&redelay"),
                             tr("Effect's %1 \"Predelay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Predelay\" parameter of this effect"),
                             tr("Effect's %1 \"Predelay\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("Fre&quency"),
                             tr("Effect's %1 \"Frequency\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Frequency\" parameter of this effect"),
                             tr("Effect's %1 \"Frequency\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Frequency\" parameter of this effect")},
                         UIText{
                             tr("&Ressonance"),
                             tr("Effect's %1 \"Resonance\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Resonance\" parameter of this effect"),
                             tr("Effect's %1 \"Resonance\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Resonance\" parameter of this effect")},
                         UIText{
                             tr("&In Level"),
                             tr("Effect's %1 \"In Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"In Level\" parameter of this effect"),
                             tr("Effect's %1 \"In Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"In Level\" parameter of this effect")});
                break;
            case effects::MONO_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")},
                         UIText{
                             tr("A&ttenuation"),
                             tr("Effect's %1 \"Attenuation\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Attenuation\" parameter of this effect"),
                             tr("Effect's %1 \"Attenuation\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Attenuation\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::MULTITAP_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")},
                         UIText{
                             tr("&Mode"),
                             tr("Effect's %1 \"Mode\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Mode\" parameter of this effect"),
                             tr("Effect's %1 \"Mode\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Mode\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::PING_PONG_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")},
                         UIText{
                             tr("&Stereo"),
                             tr("Effect's %1 \"Stereo\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Stereo\" parameter of this effect"),
                             tr("Effect's %1 \"Stereo\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Stereo\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
            case effects::REVERSE_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&RFDBK"),
                             tr("Effect's %1 \"RFDBK\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"RFDBK\" parameter of this effect"),
                             tr("Effect's %1 \"RFDBK\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"RFDBK\" parameter of this effect")},
                         UIText{
                             tr("&Tone"),
                             tr("Effect's %1 \"Tone\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Tone\" parameter of this effect"),
                             tr("Effect's %1 \"Tone\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Tone\" parameter of this effect")},
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("&Release"),
                             tr("Effect's %1 \"Release\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Release\" parameter of this effect"),
                             tr("Effect's %1 \"Release\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Release\" parameter of this effect")},
                         UIText{
                             tr("&Threshold"),
                             tr("Effect's %1 \"Threshold\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Threshold\" parameter of this effect"),
                             tr("Effect's %1 \"Threshold\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("Fl&utter"),
                             tr("Effect's %1 \"Flutter\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Flutter\" parameter of this effect"),
                             tr("Effect's %1 \"Flutter\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Flutter\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Brightness\" parameter of this effect")},
                         UIText{
                             tr("&Stereo"),
                             tr("Effect's %1 \"Stereo\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Stereo\" parameter of this effect"),
                             tr("Effect's %1 \"Stereo\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Stereo\" parameter of this effect")});
                break;
            case effects::STEREO_TAPE_DELAY:
                setTexts(ui.get(),
                         UIText{
                             tr("&Level"),
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Delay"),
                             tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Delay\" parameter of this effect"),
                             tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Delay\" parameter of this effect")},
                         UIText{
                             tr("&Feedback"),
                             tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Feedback\" parameter of this effect"),
                             tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Feedback\" parameter of this effect")},
                         UIText{
                             tr("Fl&utter"),
                             tr("Effect's %1 \"Flutter\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Flutter\" parameter of this effect"),
                             tr("Effect's %1 \"Flutter\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Flutter\" parameter of this effect")},
                         UIText{
                             tr("&Separation"),
                             tr("Effect's %1 \"Separation\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Separation\" parameter of this effect"),
                             tr("Effect's %1 \"Separation\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Separation\" parameter of this effect")},
                         UIText{
                             tr("&Brightness"),
                             tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Brightness\" parameter of this effect"),
                             tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1),
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
                             tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Level\" parameter of this effect"),
                             tr("Effect's %1 \"Level\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Level\" parameter of this effect")},
                         UIText{
                             tr("&Decay"),
                             tr("Effect's %1 \"Decay\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Decay\" parameter of this effect"),
                             tr("Effect's %1 \"Decay\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Decay\" parameter of this effect")},
                         UIText{
                             tr("D&well"),
                             tr("Effect's %1 \"Dwell\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Dwell\" parameter of this effect"),
                             tr("Effect's %1 \"Dwell\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Dwell\" parameter of this effect")},
                         UIText{
                             tr("D&iffusion"),
                             tr("Effect's %1 \"Diffusion\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Diffusion\" parameter of this effect"),
                             tr("Effect's %1 \"Diffusion\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Diffusion\" parameter of this effect")},
                         UIText{
                             tr("&Tone"),
                             tr("Effect's %1 \"Tone\" dial").arg(fx_slot + 1),
                             tr("Allows you to set \"Tone\" parameter of this effect"),
                             tr("Effect's %1 \"Tone\" box").arg(fx_slot + 1),
                             tr("Allows you to precisely set \"Tone\" parameter of this effect")},
                         UIText{
                             tr(""),
                             tr("Disabled dial"),
                             tr("This dial is disabled in this effect"),
                             tr("Disabled box"),
                             tr("This box is disabled in this effect")});
                break;
        }

        if (settings.value("Settings/defaultEffectValues").toBool())
        {
            switch (static_cast<effects>(value))
            {
                case effects::EMPTY:
                    break;
                case effects::OVERDRIVE:
                    setDialValues(0x80, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::WAH:
                case effects::TOUCH_WAH:
                    setDialValues(0xff, 0x80, 0x00, 0xff, 0x00, 0x00);
                    break;
                case effects::FUZZ:
                    setDialValues(0x80, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::FUZZ_TOUCH_WAH:
                    setDialValues(0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
                    break;
                case effects::SIMPLE_COMP:
                    setDialValues(0x01, 0x00, 0x00, 0x00, 0x00, 0x00);
                    break;
                case effects::COMPRESSOR:
                    setDialValues(0x8d, 0x0f, 0x4f, 0x7f, 0x7f, 0x00);
                    break;
                case effects::SINE_CHORUS:
                    setDialValues(0xff, 0x0e, 0x19, 0x19, 0x80, 0x00);
                    break;
                case effects::TRIANGLE_CHORUS:
                    setDialValues(0x5d, 0x0e, 0x19, 0x19, 0x80, 0x00);
                    break;
                case effects::SINE_FLANGER:
                    setDialValues(0xff, 0x0e, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::TRIANGLE_FLANGER:
                    setDialValues(0xff, 0x00, 0xff, 0x33, 0x41, 0x00);
                    break;
                case effects::VIBRATONE:
                    setDialValues(0xf4, 0xff, 0x27, 0xad, 0x82, 0x00);
                    break;
                case effects::VINTAGE_TREMOLO:
                    setDialValues(0xdb, 0xad, 0x63, 0xf4, 0xf1, 0x00);
                    break;
                case effects::SINE_TREMOLO:
                    setDialValues(0xdb, 0x99, 0x7d, 0x00, 0x00, 0x00);
                    break;
                case effects::RING_MODULATOR:
                case effects::STEP_FILTER:
                    setDialValues(0xff, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::PHASER:
                    setDialValues(0xfd, 0x00, 0xfd, 0xb8, 0x00, 0x00);
                    break;
                case effects::PITCH_SHIFTER:
                    setDialValues(0xc7, 0x3e, 0x80, 0x00, 0x00, 0x00);
                    break;
                case effects::MONO_ECHO_FILTER:
                    setDialValues(0xff, 0x80, 0x80, 0x80, 0x80, 0x80);
                    break;
                case effects::STEREO_ECHO_FILTER:
                    setDialValues(0x80, 0xb3, 0x80, 0x80, 0x80, 0x80);
                    break;
                case effects::MONO_DELAY:
                    setDialValues(0xff, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::MULTITAP_DELAY:
                    setDialValues(0xff, 0x80, 0x66, 0x80, 0x80, 0x00);
                    break;
                case effects::REVERSE_DELAY:
                case effects::PING_PONG_DELAY:
                    setDialValues(0xff, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::TAPE_DELAY:
                    setDialValues(0x7d, 0x1c, 0x00, 0x63, 0x80, 0x00);
                    break;
                case effects::STEREO_TAPE_DELAY:
                    setDialValues(0x7d, 0x88, 0x1c, 0x63, 0xff, 0x80);
                    break;
                case effects::DUCKING_DELAY:
                    setDialValues(0xff, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::SMALL_HALL_REVERB:
                    setDialValues(0x6e, 0x5d, 0x6e, 0x80, 0x91, 0x00);
                    break;
                case effects::LARGE_HALL_REVERB:
                    setDialValues(0x4f, 0x3e, 0x80, 0x05, 0xb0, 0x00);
                    break;
                case effects::SMALL_ROOM_REVERB:
                case effects::LARGE_ROOM_REVERB:
                case effects::SMALL_PLATE_REVERB:
                    setDialValues(0x80, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::LARGE_PLATE_REVERB:
                    setDialValues(0x38, 0x80, 0x91, 0x80, 0xb6, 0x00);
                    break;
                case effects::AMBIENT_REVERB:
                case effects::ARENA_REVERB:
                    setDialValues(0xff, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::FENDER_63_SPRING_REVERB:
                    setDialValues(0x80, 0x80, 0x80, 0x80, 0x80, 0x00);
                    break;
                case effects::FENDER_65_SPRING_REVERB:
                    setDialValues(0x80, 0x8b, 0x49, 0xff, 0x80, 0x00);
                    break;
            }
        }
    }

    // send settings to the amplifier
    void Effect::send_fx()
    {
        fx_pedal_settings pedal{};

        if (!get_changed())
        {
            return;
        }
        set_changed(false);

        pedal.effect_num = effect_num;
        pedal.fx_slot = fx_slot;
        pedal.position = position;
        pedal.knob1 = knob1;
        pedal.knob2 = knob2;
        pedal.knob3 = knob3;
        pedal.knob4 = knob4;
        pedal.knob5 = knob5;
        pedal.knob6 = knob6;
        pedal.enabled = enabled;

        dynamic_cast<MainWindow*>(parent())->set_effect(pedal);
    }

    void Effect::load(fx_pedal_settings settings)
    {
        set_changed(true);

        ui->comboBox->setCurrentIndex(value(settings.effect_num));
        ui->dial->setValue(settings.knob1);
        ui->dial_2->setValue(settings.knob2);
        ui->dial_3->setValue(settings.knob3);
        ui->dial_4->setValue(settings.knob4);
        ui->dial_5->setValue(settings.knob5);
        ui->dial_6->setValue(settings.knob6);
        ui->checkBox->setChecked(settings.position == Position::effectsLoop);
    }

    void Effect::get_settings(fx_pedal_settings& pedal)
    {
        pedal.effect_num = effect_num;
        pedal.fx_slot = fx_slot;
        pedal.position = position;
        pedal.knob1 = knob1;
        pedal.knob2 = knob2;
        pedal.knob3 = knob3;
        pedal.knob4 = knob4;
        pedal.knob5 = knob5;
        pedal.knob6 = knob6;
    }

    void Effect::off_switch(bool value)
    {
        if (value)
        {
            enabled = false;
            ui->pushButton->setText(tr("On"));
            ui->comboBox->setDisabled(true);
            ui->setButton->setDisabled(true);
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
            ui->checkBox->setDisabled(true);
            ui->label->setDisabled(true);
            ui->label_2->setDisabled(true);
            ui->label_3->setDisabled(true);
            ui->label_4->setDisabled(true);
            ui->label_5->setDisabled(true);
            ui->label_6->setDisabled(true);
            ui->label_7->setDisabled(true);
            temp1 = windowTitle();
            temp2 = accessibleName();
            setWindowTitle(tr("FX%1: OFF").arg(fx_slot + 1));
            setAccessibleName(tr("Effect's %1 window: OFF").arg(fx_slot + 1));
        }
        else
        {
            enabled = true;
            ui->pushButton->setText(tr("Off"));
            ui->comboBox->setDisabled(false);
            ui->setButton->setDisabled(false);
            ui->checkBox->setDisabled(false);
            ui->label->setDisabled(false);
            ui->label_2->setDisabled(false);
            ui->label_3->setDisabled(false);
            ui->label_4->setDisabled(false);
            ui->label_5->setDisabled(false);
            ui->label_6->setDisabled(false);
            ui->label_7->setDisabled(false);
            ui->dial->setDisabled(false);
            ui->spinBox->setDisabled(false);

            if (effect_num != effects::SIMPLE_COMP)
            {
                ui->dial_2->setDisabled(false);
                ui->dial_3->setDisabled(false);
                ui->dial_4->setDisabled(false);
                ui->dial_5->setDisabled(false);
                ui->spinBox_2->setDisabled(false);
                ui->spinBox_3->setDisabled(false);
                ui->spinBox_4->setDisabled(false);
                ui->spinBox_5->setDisabled(false);
                if (effect_num == effects::MONO_ECHO_FILTER || effect_num == effects::STEREO_ECHO_FILTER || effect_num == effects::TAPE_DELAY || effect_num == effects::STEREO_TAPE_DELAY)
                {
                    ui->dial_6->setDisabled(false);
                    ui->spinBox_6->setDisabled(false);
                }
            }

            setWindowTitle(temp1);
            setAccessibleName(temp2);
            activateWindow();
        }
        set_changed(true);
        send_fx();
    }

    void Effect::set_changed(bool value)
    {
        changed = value;
    }

    bool Effect::get_changed() const
    {
        return changed;
    }

    void Effect::enable_set_button(bool value)
    {
        ui->setButton->setEnabled(value);
    }

    void Effect::load_default_fx()
    {
        QSettings settings;

        if (!settings.contains(QString("DefaultEffects/Effect%1/Effect").arg(fx_slot)))
        {
            return;
        }

        ui->comboBox->setCurrentIndex(settings.value(QString("DefaultEffects/Effect%1/Effect").arg(fx_slot)).toInt());
        ui->dial->setValue(settings.value(QString("DefaultEffects/Effect%1/Knob1").arg(fx_slot)).toInt());
        ui->dial_2->setValue(settings.value(QString("DefaultEffects/Effect%1/Knob2").arg(fx_slot)).toInt());
        ui->dial_3->setValue(settings.value(QString("DefaultEffects/Effect%1/Knob3").arg(fx_slot)).toInt());
        ui->dial_4->setValue(settings.value(QString("DefaultEffects/Effect%1/Knob4").arg(fx_slot)).toInt());
        ui->dial_5->setValue(settings.value(QString("DefaultEffects/Effect%1/Knob5").arg(fx_slot)).toInt());
        ui->dial_6->setValue(settings.value(QString("DefaultEffects/Effect%1/Knob6").arg(fx_slot)).toInt());
        ui->checkBox->setChecked(settings.value(QString("DefaultEffects/Effect%1/Post amp").arg(fx_slot)).toBool());

        set_changed(true);
        this->send_fx();
    }

    void Effect::showAndActivate()
    {
        show();
        activateWindow();
    }

    void Effect::setTitleTexts(int slot, const QString& name)
    {
        setWindowTitle(tr("FX%1: %2").arg(slot + 1).arg(name));
        setAccessibleName(tr("Effect's %1 window: %2").arg(fx_slot + 1).arg(name));
    }

    void Effect::setDialValues(int d1, int d2, int d3, int d4, int d5, int d6)
    {
        ui->dial->setValue(d1);
        ui->dial_2->setValue(d2);
        ui->dial_3->setValue(d3);
        ui->dial_4->setValue(d4);
        ui->dial_5->setValue(d5);
        ui->dial_6->setValue(d6);
    }

}

#include "ui/moc_effect.moc"
