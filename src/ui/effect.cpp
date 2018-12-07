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

#include "ui/effect.h"
#include "ui/mainwindow.h"
#include "ui_effect.h"

namespace plug
{

    Effect::Effect(QWidget* parent, int number)
        : QMainWindow(parent),
          ui(std::make_unique<Ui::Effect>()),
          fx_slot(static_cast<std::uint8_t>(number)),
          effect_num(effects::EMPTY),
          knob1(0),
          knob2(0),
          knob3(0),
          knob4(0),
          knob5(0),
          knob6(0),
          position(Position::input),
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
                setWindowTitle(tr("FX%1: EMPTY").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: EMPTY").arg(fx_slot + 1));
                break;

            case effects::OVERDRIVE:
                setWindowTitle(tr("FX%1: Overdrive").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Overdrive").arg(fx_slot + 1));
                break;

            case effects::WAH:
                setWindowTitle(tr("FX%1: Wah").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Wah").arg(fx_slot + 1));
                break;

            case effects::TOUCH_WAH:
                setWindowTitle(tr("FX%1: Touch Wah").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Touch Wah").arg(fx_slot + 1));
                break;

            case effects::FUZZ:
                setWindowTitle(tr("FX%1: Fuzz").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Fuzz").arg(fx_slot + 1));
                break;

            case effects::FUZZ_TOUCH_WAH:
                setWindowTitle(tr("FX%1: Fuzz Touch Wah").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Fuzz Touch Wah").arg(fx_slot + 1));
                break;

            case effects::SIMPLE_COMP:
                setWindowTitle(tr("FX%1: Simple Compressor").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Simple Compressor").arg(fx_slot + 1));
                break;

            case effects::COMPRESSOR:
                setWindowTitle(tr("FX%1: Compressor").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Compressor").arg(fx_slot + 1));
                break;

            case effects::SINE_CHORUS:
                setWindowTitle(tr("FX%1: Sine Chorus").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Sine Chorus").arg(fx_slot + 1));
                break;

            case effects::TRIANGLE_CHORUS:
                setWindowTitle(tr("FX%1: Triangle Chorus").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Triangle Chorus").arg(fx_slot + 1));
                break;

            case effects::SINE_FLANGER:
                setWindowTitle(tr("FX%1: Sine Flanger").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Sine Flanger").arg(fx_slot + 1));
                break;

            case effects::TRIANGLE_FLANGER:
                setWindowTitle(tr("FX%1: Triangle Flanger").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Triangle Flanger").arg(fx_slot + 1));
                break;

            case effects::VIBRATONE:
                setWindowTitle(tr("FX%1: Vibratone").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Vibratone").arg(fx_slot + 1));
                break;

            case effects::VINTAGE_TREMOLO:
                setWindowTitle(tr("FX%1: Vintage Tremolo").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Vintage Tremolo").arg(fx_slot + 1));
                break;

            case effects::SINE_TREMOLO:
                setWindowTitle(tr("FX%1: Sine Tremolo").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Sine Tremolo").arg(fx_slot + 1));
                break;

            case effects::RING_MODULATOR:
                setWindowTitle(tr("FX%1: Ring Modulator").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Ring Modulator").arg(fx_slot + 1));
                break;

            case effects::STEP_FILTER:
                setWindowTitle(tr("FX%1: Step Filter").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Step Filter").arg(fx_slot + 1));
                break;

            case effects::PHASER:
                setWindowTitle(tr("FX%1: Phaser").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Phaser").arg(fx_slot + 1));
                break;

            case effects::PITCH_SHIFTER:
                setWindowTitle(tr("FX%1: Pitch Shifter").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Pitch Shifter").arg(fx_slot + 1));
                break;

            case effects::MONO_DELAY:
                setWindowTitle(tr("FX%1: Mono Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Mono Delay").arg(fx_slot + 1));
                break;

            case effects::MONO_ECHO_FILTER:
                setWindowTitle(tr("FX%1: Mono Echo Filter").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Mono Echo Filter").arg(fx_slot + 1));
                break;

            case effects::STEREO_ECHO_FILTER:
                setWindowTitle(tr("FX%1: Stereo Echo Filter").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Stereo Echo Filter").arg(fx_slot + 1));
                break;

            case effects::MULTITAP_DELAY:
                setWindowTitle(tr("FX%1: Multitap Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Multitap Delay").arg(fx_slot + 1));
                break;

            case effects::PING_PONG_DELAY:
                setWindowTitle(tr("FX%1: Ping-Pong Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Ping-Pong Delay").arg(fx_slot + 1));
                break;

            case effects::DUCKING_DELAY:
                setWindowTitle(tr("FX%1: Ducking Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Ducking Delay").arg(fx_slot + 1));
                break;

            case effects::REVERSE_DELAY:
                setWindowTitle(tr("FX%1: Reverse Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Reverse Delay").arg(fx_slot + 1));
                break;

            case effects::TAPE_DELAY:
                setWindowTitle(tr("FX%1: Tape Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Tape Delay").arg(fx_slot + 1));
                break;

            case effects::STEREO_TAPE_DELAY:
                setWindowTitle(tr("FX%1: Stereo Tape Delay").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Stereo Tape Delay").arg(fx_slot + 1));
                break;

            case effects::SMALL_HALL_REVERB:
                setWindowTitle(tr("FX%1: Small Hall Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Small Hall Reverb").arg(fx_slot + 1));
                break;

            case effects::LARGE_HALL_REVERB:
                setWindowTitle(tr("FX%1: Large Hall Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Large Hall Reverb").arg(fx_slot + 1));
                break;

            case effects::SMALL_ROOM_REVERB:
                setWindowTitle(tr("FX%1: Small Room Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Small Room Reverb").arg(fx_slot + 1));
                break;

            case effects::LARGE_ROOM_REVERB:
                setWindowTitle(tr("FX%1: Large Room Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Large Room Reverb").arg(fx_slot + 1));
                break;

            case effects::SMALL_PLATE_REVERB:
                setWindowTitle(tr("FX%1: Small Plate Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Small Plate Reverb").arg(fx_slot + 1));
                break;

            case effects::LARGE_PLATE_REVERB:
                setWindowTitle(tr("FX%1: Large Plate Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Large Plate Reverb").arg(fx_slot + 1));
                break;

            case effects::AMBIENT_REVERB:
                setWindowTitle(tr("FX%1: Ambient Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Ambient Reverb").arg(fx_slot + 1));
                break;

            case effects::ARENA_REVERB:
                setWindowTitle(tr("FX%1: Arena Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Arena Reverb").arg(fx_slot + 1));
                break;

            case effects::FENDER_63_SPRING_REVERB:
                setWindowTitle(tr("FX%1: Fender '63 Spring Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Fender '63 Spring Reverb").arg(fx_slot + 1));
                break;

            case effects::FENDER_65_SPRING_REVERB:
                setWindowTitle(tr("FX%1: Fender '65 Spring Reverb").arg(fx_slot + 1));
                setAccessibleName(tr("Effect's %1 window: Fender '65 Spring Reverb").arg(fx_slot + 1));
                break;
        }

        // set knobs labels and accessibility informations
        switch (static_cast<effects>(value))
        {
            case effects::EMPTY:
                ui->label->setText(tr(""));
                ui->dial->setAccessibleName(tr("Effect's %1 dial 1").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 box 1").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

                ui->label_2->setText(tr(""));
                ui->dial_2->setAccessibleName(tr("Effect's %1 dial 2").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 box 2").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

                ui->label_3->setText(tr(""));
                ui->dial_3->setAccessibleName(tr("Effect's %1 dial 3").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 box 3").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

                ui->label_4->setText(tr(""));
                ui->dial_4->setAccessibleName(tr("Effect's %1 dial 4").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 box 4").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

                ui->label_5->setText(tr(""));
                ui->dial_5->setAccessibleName(tr("Effect's %1 dial 5").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 box 5").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Effect's %1 dial 6").arg(fx_slot + 1));
                ui->dial_6->setAccessibleDescription(tr("When you choose an effect you can set value of a parameter here"));
                ui->spinBox_6->setAccessibleName(tr("Effect's %1 box 6").arg(fx_slot + 1));
                ui->spinBox_6->setAccessibleDescription(tr("When you choose an effect you can set precise value of a parameter here"));
                break;

            case effects::OVERDRIVE:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Gain"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Gain\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Gain\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Gain\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Gain\" parameter of this effect"));

                ui->label_3->setText(tr("L&ow"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Low tones\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Low tones\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Low tones\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Low tones\" parameter of this effect"));

                ui->label_4->setText(tr("&Medium"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Medium tones\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Medium tones\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Medium tones\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Medium tones\" parameter of this effect"));

                ui->label_5->setText(tr("&High"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Hight tones\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"High tones\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"High tones\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"High tones\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::WAH:
                ui->label->setText(tr("&Mix"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Mix\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Mix\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Mix\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Mix\" parameter of this effect"));

                ui->label_2->setText(tr("&Frequency"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Frequency\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Frequency\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Frequency\" parameter of this effect"));

                ui->label_3->setText(tr("&Heel Freq"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Heel Frequency\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Heel Frequency\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Heel Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Heel Frequency\" parameter of this effect"));

                ui->label_4->setText(tr("&Toe Freq"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Toe Frequency\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Toe Frequency\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Toe Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Toe Frequency\" parameter of this effect"));

                ui->label_5->setText(tr("High &Q"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"High Q\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"High Q\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"High Q\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"High Q\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::TOUCH_WAH:
                ui->label->setText(tr("&Mix"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Mix\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Mix\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Mix\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Mix\" parameter of this effect"));

                ui->label_2->setText(tr("&Sensivity"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Sensivity\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Sensivity\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Sensivity\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Sensivity\" parameter of this effect"));

                ui->label_3->setText(tr("&Heel Freq"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Heel Frequency\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Heel Frequency\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Heel Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Heel Frequency\" parameter of this effect"));

                ui->label_4->setText(tr("&Toe Freq"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Toe Frequency\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Toe Frequency\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Toe Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Toe Frequency\" parameter of this effect"));

                ui->label_5->setText(tr("High &Q"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"High Q\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"High Q\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"High Q\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"High Q\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::FUZZ:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Gain"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Gain\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Gain\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Gain\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Gain\" parameter of this effect"));

                ui->label_3->setText(tr("&Octave"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Octave\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Octave\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Octave\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Octave\" parameter of this effect"));

                ui->label_4->setText(tr("L&ow"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Low tones\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Low tones\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Low tones\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Low tones\" parameter of this effect"));

                ui->label_5->setText(tr("&High"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Hight tones\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"High tones\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"High tones\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"High tones\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::FUZZ_TOUCH_WAH:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Gain"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Gain\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Gain\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Gain\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Gain\" parameter of this effect"));

                ui->label_3->setText(tr("&Sensivity"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Sensivity\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Sensivity\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Sensivity\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Sensivity\" parameter of this effect"));

                ui->label_4->setText(tr("&Octave"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Octave\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Octave\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Octave\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Octave\" parameter of this effect"));

                ui->label_5->setText(tr("&Peak"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Peak\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Peak\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Peak\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Peak\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::SIMPLE_COMP:
                ui->label->setText(tr("&Type"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Type\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Type\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Type\" box").arg(fx_slot + 1));
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
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Threshold"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Threshold\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Threshold\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Threshold\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Threshold\" parameter of this effect"));

                ui->label_3->setText(tr("&Ratio"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Ratio\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Ratio\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Ratio\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Ratio\" parameter of this effect"));

                ui->label_4->setText(tr("Atta&ck"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Attack\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Attack\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Attack\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Attack\" parameter of this effect"));

                ui->label_5->setText(tr("&Release"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Release\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Release\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Release\" box").arg(fx_slot + 1));
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
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rate"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

                ui->label_3->setText(tr("&Depth"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

                ui->label_4->setText(tr("A&vr Delay"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Average Delay\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Average Delay\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Average Delay\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Average Delay\" parameter of this effect"));

                ui->label_5->setText(tr("LR &Phase"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"LR Phase\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LR Phase\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"LR Phase\" box").arg(fx_slot + 1));
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
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rate"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

                ui->label_3->setText(tr("&Depth"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

                ui->label_4->setText(tr("&Feedback"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_5->setText(tr("LR &Phase"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"LR Phase\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LR Phase\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"LR Phase\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LR Phase\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::VIBRATONE:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rotor"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rotor\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rotor\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rotor\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rotor\" parameter of this effect"));

                ui->label_3->setText(tr("&Depth"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

                ui->label_4->setText(tr("&Feedback"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_5->setText(tr("LR &Phase"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"LR Phase\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"LR Phase\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"LR Phase\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"LR Phase\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::VINTAGE_TREMOLO:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rate"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

                ui->label_3->setText(tr("&Duty Cycle"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Duty Cycle\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Duty Cycle\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Duty Cycle\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect"));

                ui->label_4->setText(tr("Atta&ck"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Attack\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Attack\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Attack\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Attack\" parameter of this effect"));

                ui->label_5->setText(tr("Relea&se"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Release\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Release\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Release\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Release\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::SINE_TREMOLO:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rate"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

                ui->label_3->setText(tr("&Duty Cycle"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Duty Cycle\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Duty Cycle\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Duty Cycle\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Duty Cycle\" parameter of this effect"));

                ui->label_4->setText(tr("LFO &Clipping"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"LFO Clipping\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"LFO Clipping\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"LFO Clipping\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"LFO Clipping\" parameter of this effect"));

                ui->label_5->setText(tr("&Shape"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Shape\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Shape\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Shape\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Shape\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::RING_MODULATOR:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Frequency"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Frequency\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Frequency\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Frequency\" parameter of this effect"));

                ui->label_3->setText(tr("&Depth"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

                ui->label_4->setText(tr("&Shape"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Shape\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Shape\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Shape\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Shape\" parameter of this effect"));

                ui->label_5->setText(tr("&Phase"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Phase\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Pase\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Phase\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Phase\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::STEP_FILTER:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rate"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

                ui->label_3->setText(tr("Re&sonance"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Resonance\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Resonance\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Resonance\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Resonance\" parameter of this effect"));

                ui->label_4->setText(tr("Mi&n Freq"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Minimum Frequency\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Minimum Frequency\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Minimum Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Minimum Frequency\" parameter of this effect"));

                ui->label_5->setText(tr("Ma&x Freq"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Maximum Frequency\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Maximum Frequency\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Maximum Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Maximum Frequency\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::PHASER:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Rate"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Rate\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Rate\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Rate\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Rate\" parameter of this effect"));

                ui->label_3->setText(tr("&Depth"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Depth\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Depth\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Depth\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Depth\" parameter of this effect"));

                ui->label_4->setText(tr("&Feedback"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_5->setText(tr("&Shape"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Shape\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Shape\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Shape\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Shape\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::PITCH_SHIFTER:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Pitch"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Pitch\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Pitch\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Pitch\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Pitch\" parameter of this effect"));

                ui->label_3->setText(tr("&Detune"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Detune\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Detune\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Detune\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Detune\" parameter of this effect"));

                ui->label_4->setText(tr("&Feedback"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_5->setText(tr("P&redelay"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Predelay\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Predelay\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Predelay\" box").arg(fx_slot + 1));
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
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("Fre&quency"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Frequency\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Frequency\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Frequency\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Frequency\" parameter of this effect"));

                ui->label_5->setText(tr("&Ressonance"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Resonance\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Resonance\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Resonance\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Resonance\" parameter of this effect"));

                ui->label_6->setText(tr("&In Level"));
                ui->dial_6->setAccessibleName(tr("Effect's %1 \"In Level\" dial").arg(fx_slot + 1));
                ui->dial_6->setAccessibleDescription(tr("Allows you to set \"In Level\" parameter of this effect"));
                ui->spinBox_6->setAccessibleName(tr("Effect's %1 \"In Level\" box").arg(fx_slot + 1));
                ui->spinBox_6->setAccessibleDescription(tr("Allows you to precisely set \"In Level\" parameter of this effect"));
                break;

            case effects::MONO_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("&Brightness"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));

                ui->label_5->setText(tr("A&ttenuation"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Attenuation\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Attenuation\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Attenuation\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Attenuation\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::MULTITAP_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("&Brightness"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));

                ui->label_5->setText(tr("&Mode"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Mode\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Mode\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Mode\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Mode\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::PING_PONG_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("&Brightness"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));

                ui->label_5->setText(tr("&Stereo"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Stereo\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Stereo\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Stereo\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Stereo\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::REVERSE_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("&RFDBK"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"RFDBK\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"RFDBK\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"RFDBK\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"RFDBK\" parameter of this effect"));

                ui->label_5->setText(tr("&Tone"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Tone\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Tone\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Tone\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Tone\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::DUCKING_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("&Release"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Release\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Release\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Release\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Release\" parameter of this effect"));

                ui->label_5->setText(tr("&Threshold"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Threshold\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Threshold\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Threshold\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Threshold\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;

            case effects::TAPE_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("Fl&utter"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Flutter\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Flutter\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Flutter\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Flutter\" parameter of this effect"));

                ui->label_5->setText(tr("&Brightness"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Brightness\" parameter of this effect"));

                ui->label_6->setText(tr("&Stereo"));
                ui->dial_6->setAccessibleName(tr("Effect's %1 \"Stereo\" dial").arg(fx_slot + 1));
                ui->dial_6->setAccessibleDescription(tr("Allows you to set \"Stereo\" parameter of this effect"));
                ui->spinBox_6->setAccessibleName(tr("Effect's %1 \"Stereo\" box").arg(fx_slot + 1));
                ui->spinBox_6->setAccessibleDescription(tr("Allows you to precisely set \"Stereo\" parameter of this effect"));
                break;

            case effects::STEREO_TAPE_DELAY:
                ui->label->setText(tr("&Level"));
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Delay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Delay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Delay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Delay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Delay\" parameter of this effect"));

                ui->label_3->setText(tr("&Feedback"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Feedback\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Feedback\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Feedback\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Feedback\" parameter of this effect"));

                ui->label_4->setText(tr("Fl&utter"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Flutter\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Flutter\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Flutter\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Flutter\" parameter of this effect"));

                ui->label_5->setText(tr("&Separation"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Separation\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Separation\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Separation\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Separation\" parameter of this effect"));

                ui->label_6->setText(tr("&Brightness"));
                ui->dial_6->setAccessibleName(tr("Effect's %1 \"Brightness\" dial").arg(fx_slot + 1));
                ui->dial_6->setAccessibleDescription(tr("Allows you to set \"Brightness\" parameter of this effect"));
                ui->spinBox_6->setAccessibleName(tr("Effect's %1 \"Brightness\" box").arg(fx_slot + 1));
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
                ui->dial->setAccessibleName(tr("Effect's %1 \"Level\" dial").arg(fx_slot + 1));
                ui->dial->setAccessibleDescription(tr("Allows you to set \"Level\" parameter of this effect"));
                ui->spinBox->setAccessibleName(tr("Effect's %1 \"Level\" box").arg(fx_slot + 1));
                ui->spinBox->setAccessibleDescription(tr("Allows you to precisely set \"Level\" parameter of this effect"));

                ui->label_2->setText(tr("&Decay"));
                ui->dial_2->setAccessibleName(tr("Effect's %1 \"Decay\" dial").arg(fx_slot + 1));
                ui->dial_2->setAccessibleDescription(tr("Allows you to set \"Decay\" parameter of this effect"));
                ui->spinBox_2->setAccessibleName(tr("Effect's %1 \"Decay\" box").arg(fx_slot + 1));
                ui->spinBox_2->setAccessibleDescription(tr("Allows you to precisely set \"Decay\" parameter of this effect"));

                ui->label_3->setText(tr("D&well"));
                ui->dial_3->setAccessibleName(tr("Effect's %1 \"Dwell\" dial").arg(fx_slot + 1));
                ui->dial_3->setAccessibleDescription(tr("Allows you to set \"Dwell\" parameter of this effect"));
                ui->spinBox_3->setAccessibleName(tr("Effect's %1 \"Dwell\" box").arg(fx_slot + 1));
                ui->spinBox_3->setAccessibleDescription(tr("Allows you to precisely set \"Dwell\" parameter of this effect"));

                ui->label_4->setText(tr("D&iffusion"));
                ui->dial_4->setAccessibleName(tr("Effect's %1 \"Diffusion\" dial").arg(fx_slot + 1));
                ui->dial_4->setAccessibleDescription(tr("Allows you to set \"Diffusion\" parameter of this effect"));
                ui->spinBox_4->setAccessibleName(tr("Effect's %1 \"Diffusion\" box").arg(fx_slot + 1));
                ui->spinBox_4->setAccessibleDescription(tr("Allows you to precisely set \"Diffusion\" parameter of this effect"));

                ui->label_5->setText(tr("&Tone"));
                ui->dial_5->setAccessibleName(tr("Effect's %1 \"Tone\" dial").arg(fx_slot + 1));
                ui->dial_5->setAccessibleDescription(tr("Allows you to set \"Tone\" parameter of this effect"));
                ui->spinBox_5->setAccessibleName(tr("Effect's %1 \"Tone\" box").arg(fx_slot + 1));
                ui->spinBox_5->setAccessibleDescription(tr("Allows you to precisely set \"Tone\" parameter of this effect"));

                ui->label_6->setText(tr(""));
                ui->dial_6->setAccessibleName(tr("Disabled dial"));
                ui->dial_6->setAccessibleDescription(tr("This dial is disabled in this effect"));
                ui->spinBox_6->setAccessibleName(tr("Disabled box"));
                ui->spinBox_6->setAccessibleDescription(tr("This box is disabled in this effect"));
                break;
        }

        if (settings.value("Settings/defaultEffectValues").toBool())
        {
            switch (static_cast<effects>(value))
            {
                case effects::EMPTY:
                    break;

                case effects::OVERDRIVE:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::WAH:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x00);
                    ui->dial_4->setValue(0xff);
                    ui->dial_5->setValue(0x00);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::TOUCH_WAH:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x00);
                    ui->dial_4->setValue(0xff);
                    ui->dial_5->setValue(0x00);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::FUZZ:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::FUZZ_TOUCH_WAH:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x80);
                    break;

                case effects::SIMPLE_COMP:
                    ui->dial->setValue(0x01);
                    ui->dial_2->setValue(0x00);
                    ui->dial_3->setValue(0x00);
                    ui->dial_4->setValue(0x00);
                    ui->dial_5->setValue(0x00);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::COMPRESSOR:
                    ui->dial->setValue(0x8d);
                    ui->dial_2->setValue(0x0f);
                    ui->dial_3->setValue(0x4f);
                    ui->dial_4->setValue(0x7f);
                    ui->dial_5->setValue(0x7f);
                    ui->dial_6->setValue(0x00);
                    break;


                case effects::SINE_CHORUS:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x0e);
                    ui->dial_3->setValue(0x19);
                    ui->dial_4->setValue(0x19);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::TRIANGLE_CHORUS:
                    ui->dial->setValue(0x5d);
                    ui->dial_2->setValue(0x0e);
                    ui->dial_3->setValue(0x19);
                    ui->dial_4->setValue(0x19);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::SINE_FLANGER:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x0e);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::TRIANGLE_FLANGER:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x00);
                    ui->dial_3->setValue(0xff);
                    ui->dial_4->setValue(0x33);
                    ui->dial_5->setValue(0x41);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::VIBRATONE:
                    ui->dial->setValue(0xf4);
                    ui->dial_2->setValue(0xff);
                    ui->dial_3->setValue(0x27);
                    ui->dial_4->setValue(0xad);
                    ui->dial_5->setValue(0x82);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::VINTAGE_TREMOLO:
                    ui->dial->setValue(0xdb);
                    ui->dial_2->setValue(0xad);
                    ui->dial_3->setValue(0x63);
                    ui->dial_4->setValue(0xf4);
                    ui->dial_5->setValue(0xf1);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::SINE_TREMOLO:
                    ui->dial->setValue(0xdb);
                    ui->dial_2->setValue(0x99);
                    ui->dial_3->setValue(0x7d);
                    ui->dial_4->setValue(0x00);
                    ui->dial_5->setValue(0x00);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::RING_MODULATOR:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::STEP_FILTER:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::PHASER:
                    ui->dial->setValue(0xfd);
                    ui->dial_2->setValue(0x00);
                    ui->dial_3->setValue(0xfd);
                    ui->dial_4->setValue(0xb8);
                    ui->dial_5->setValue(0x00);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::PITCH_SHIFTER:
                    ui->dial->setValue(0xc7);
                    ui->dial_2->setValue(0x3e);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x00);
                    ui->dial_5->setValue(0x00);
                    ui->dial_6->setValue(0x00);
                    break;


                case effects::MONO_ECHO_FILTER:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x80);
                    break;

                case effects::STEREO_ECHO_FILTER:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0xb3);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x80);
                    break;

                case effects::MONO_DELAY:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::MULTITAP_DELAY:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x66);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::REVERSE_DELAY:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::PING_PONG_DELAY:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::TAPE_DELAY:
                    ui->dial->setValue(0x7d);
                    ui->dial_2->setValue(0x1c);
                    ui->dial_3->setValue(0x00);
                    ui->dial_4->setValue(0x63);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::STEREO_TAPE_DELAY:
                    ui->dial->setValue(0x7d);
                    ui->dial_2->setValue(0x88);
                    ui->dial_3->setValue(0x1c);
                    ui->dial_4->setValue(0x63);
                    ui->dial_5->setValue(0xff);
                    ui->dial_6->setValue(0x80);
                    break;

                case effects::DUCKING_DELAY:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;


                case effects::SMALL_HALL_REVERB:
                    ui->dial->setValue(0x6e);
                    ui->dial_2->setValue(0x5d);
                    ui->dial_3->setValue(0x6e);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x91);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::LARGE_HALL_REVERB:
                    ui->dial->setValue(0x4f);
                    ui->dial_2->setValue(0x3e);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x05);
                    ui->dial_5->setValue(0xb0);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::SMALL_ROOM_REVERB:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::LARGE_ROOM_REVERB:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::SMALL_PLATE_REVERB:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::LARGE_PLATE_REVERB:
                    ui->dial->setValue(0x38);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x91);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0xb6);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::AMBIENT_REVERB:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::ARENA_REVERB:
                    ui->dial->setValue(0xff);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
                    break;

                case effects::FENDER_63_SPRING_REVERB:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x80);
                    ui->dial_3->setValue(0x80);
                    ui->dial_4->setValue(0x80);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0);
                    break;

                case effects::FENDER_65_SPRING_REVERB:
                    ui->dial->setValue(0x80);
                    ui->dial_2->setValue(0x8b);
                    ui->dial_3->setValue(0x49);
                    ui->dial_4->setValue(0xff);
                    ui->dial_5->setValue(0x80);
                    ui->dial_6->setValue(0x00);
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
            effect_num = effects::EMPTY;
            temp1 = windowTitle();
            temp2 = accessibleName();
            setWindowTitle(tr("FX%1: OFF").arg(fx_slot + 1));
            setAccessibleName(tr("Effect's %1 window: OFF").arg(fx_slot + 1));
        }
        else
        {
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

    bool Effect::get_changed()
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
}

#include "ui/moc_effect.moc"
