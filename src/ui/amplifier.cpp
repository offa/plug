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

#include "ui/amplifier.h"
#include "ui/mainwindow.h"
#include "ui_amplifier.h"
#include "ui/amp_advanced.h"
#include <QSettings>
#include <QShortcut>

namespace plug
{

    Amplifier::Amplifier(QWidget* parent)
        : QMainWindow(parent),
          ui(std::make_unique<Ui::Amplifier>()),
          advanced(std::make_unique<Amp_Advanced>(this)),
          amp_num(amps::FENDER_57_DELUXE),
          gain(0),
          volume(0),
          treble(0),
          middle(0),
          bass(0),
          cabinet(cabinets::OFF),
          noise_gate(0),
          presence(128),
          gain2(128),
          master_vol(128),
          threshold(0),
          depth(128),
          bias(128),
          sag(1),
          usb_gain(0),
          changed(false),
          brightness(false)
    {
        ui->setupUi(this);

        // load window size
        QSettings settings;
        restoreGeometry(settings.value("Windows/amplifierWindowGeometry").toByteArray());

        connect(ui->advancedButton, SIGNAL(clicked()), advanced.get(), SLOT(open()));
        choose_amp(0);

        connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_amp(int)));
        connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(set_gain(int)));
        connect(ui->dial_2, SIGNAL(valueChanged(int)), this, SLOT(set_volume(int)));
        connect(ui->dial_3, SIGNAL(valueChanged(int)), this, SLOT(set_treble(int)));
        connect(ui->dial_4, SIGNAL(valueChanged(int)), this, SLOT(set_middle(int)));
        connect(ui->dial_5, SIGNAL(valueChanged(int)), this, SLOT(set_bass(int)));
        connect(ui->setButton, SIGNAL(clicked()), this, SLOT(send_amp()));

        QShortcut* close = new QShortcut(QKeySequence(Qt::Key_Escape), this);
        connect(close, SIGNAL(activated()), this, SLOT(close()));
    }

    Amplifier::~Amplifier()
    {
        QSettings settings;
        settings.setValue("Windows/amplifierWindowGeometry", saveGeometry());
    }

    void Amplifier::set_gain(int value)
    {
        gain = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_volume(int value)
    {
        volume = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_treble(int value)
    {
        treble = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_middle(int value)
    {
        middle = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_bass(int value)
    {
        bass = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_cabinet(int value)
    {
        cabinet = static_cast<cabinets>(value);
        changed = true;
    }

    void Amplifier::set_noise_gate(int value)
    {
        noise_gate = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_presence(int value)
    {
        presence = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_gain2(int value)
    {
        gain2 = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_master_vol(int value)
    {
        master_vol = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_threshold(int value)
    {
        threshold = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_depth(int value)
    {
        depth = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_bias(int value)
    {
        bias = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_sag(int value)
    {
        sag = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::set_brightness(bool value)
    {
        brightness = value;
        changed = true;
    }

    void Amplifier::set_usb_gain(int value)
    {
        usb_gain = static_cast<std::uint8_t>(value);
        changed = true;
    }

    void Amplifier::choose_amp(int ampValue)
    {
        amp_num = static_cast<amps>(ampValue);
        changed = true;

        // set properties
        switch (static_cast<amps>(ampValue))
        {
            case amps::FENDER_57_DELUXE:
                advanced->change_cabinet(value(cabinets::cab57DLX));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: Fender '57 Delux");
                setAccessibleName("Amplifier: Fender '57 Delux");
                break;

            case amps::FENDER_59_BASSMAN:
                advanced->change_cabinet(value(cabinets::cabBSSMN));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: Fender '59 Bassman");
                setAccessibleName("Amplifier: Fender '59 Bassman");
                break;

            case amps::FENDER_57_CHAMP:
                advanced->change_cabinet(value(cabinets::cabCHAMP));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: Fender '57 Champ");
                setAccessibleName("Amplifier: Fender '57 Champ");
                break;

            case amps::FENDER_65_DELUXE_REVERB:
                advanced->change_cabinet(value(cabinets::cab65DLX));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: Fender '65 Deluxe Reverb");
                setAccessibleName("Amplifier: Fender '65 Deluxe Reverb");
                break;

            case amps::FENDER_65_PRINCETON:
                advanced->change_cabinet(value(cabinets::cab65PRN));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: Fender '65 Princeton");
                setAccessibleName("Amplifier: Fender '65 Princeton");
                break;

            case amps::FENDER_65_TWIN_REVERB:
                advanced->change_cabinet(value(cabinets::cab65TWN));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: Fender '65 Twin Reverb");
                setAccessibleName("Amplifier: Fender '65 Twin Reverb");
                break;

            case amps::FENDER_SUPER_SONIC:
                advanced->change_cabinet(value(cabinets::cabSS112));
                advanced->change_noise_gate(2);
                setWindowTitle("Amplifier: Fender Super-Sonic");
                setAccessibleName("Amplifier: Fender Super-Sonic");
                break;

            case amps::BRITISH_60S:
                advanced->change_cabinet(value(cabinets::cab2x12C));
                advanced->change_noise_gate(0);
                setWindowTitle("Amplifier: British 60's");
                setAccessibleName("Amplifier: British 60's");
                break;

            case amps::BRITISH_70S:
                advanced->change_cabinet(value(cabinets::cab4x12G));
                advanced->change_noise_gate(1);
                setWindowTitle("Amplifier: British 70's");
                setAccessibleName("Amplifier: British 70's");
                break;

            case amps::BRITISH_80S:
                advanced->change_cabinet(value(cabinets::cab4x12M));
                advanced->change_noise_gate(1);
                setWindowTitle("Amplifier: British 80's");
                setAccessibleName("Amplifier: British 80's");
                break;

            case amps::AMERICAN_90S:
                advanced->change_cabinet(value(cabinets::cab4x12V));
                advanced->change_noise_gate(3);
                setWindowTitle("Amplifier: American 90's");
                setAccessibleName("Amplifier: American 90's");
                break;

            case amps::METAL_2000:
                advanced->change_cabinet(value(cabinets::cab4x12G));
                advanced->change_noise_gate(2);
                setWindowTitle("Amplifier: Metal 2000");
                setAccessibleName("Amplifier: Metal 2000");
                break;

            default:
                break;
        }
    }

    // send settings to the amplifier
    void Amplifier::send_amp()
    {
        amp_settings settings{};

        if (!changed)
        {
            return;
        }
        changed = false;

        settings.amp_num = amp_num;
        settings.gain = gain;
        settings.volume = volume;
        settings.treble = treble;
        settings.middle = middle;
        settings.bass = bass;
        settings.cabinet = cabinet;
        settings.noise_gate = noise_gate;
        settings.master_vol = master_vol;
        settings.gain2 = gain2;
        settings.presence = presence;
        settings.threshold = threshold;
        settings.depth = depth;
        settings.bias = bias;
        settings.sag = sag;
        settings.brightness = brightness;
        settings.usb_gain = usb_gain;

        dynamic_cast<MainWindow*>(parent())->set_amplifier(settings);
    }

    void Amplifier::load(amp_settings settings)
    {
        changed = true;

        ui->comboBox->setCurrentIndex(value(settings.amp_num));
        ui->dial->setValue(settings.gain);
        ui->dial_2->setValue(settings.volume);
        ui->dial_3->setValue(settings.treble);
        ui->dial_4->setValue(settings.middle);
        ui->dial_5->setValue(settings.bass);

        advanced->change_cabinet(value(settings.cabinet));
        advanced->change_noise_gate(settings.noise_gate);

        advanced->set_master_vol(settings.master_vol);
        advanced->set_gain2(settings.gain2);
        advanced->set_presence(settings.presence);
        advanced->set_depth(settings.depth);
        advanced->set_threshold(settings.threshold);
        advanced->set_bias(settings.bias);
        advanced->set_sag(settings.sag);
        advanced->set_brightness(settings.brightness);
        advanced->set_usb_gain(settings.usb_gain);
    }

    void Amplifier::get_settings(amp_settings* settings)
    {
        settings->amp_num = amp_num;
        settings->gain = gain;
        settings->volume = volume;
        settings->treble = treble;
        settings->middle = middle;
        settings->bass = bass;
        settings->cabinet = cabinet;
        settings->noise_gate = noise_gate;
        settings->master_vol = master_vol;
        settings->gain2 = gain2;
        settings->presence = presence;
        settings->threshold = threshold;
        settings->depth = depth;
        settings->bias = bias;
        settings->sag = sag;
        settings->brightness = brightness;
        settings->usb_gain = usb_gain;
    }

    void Amplifier::enable_set_button(bool value)
    {
        ui->setButton->setEnabled(value);
    }

    void Amplifier::showAndActivate()
    {
        show();
        activateWindow();
    }
}

#include "ui/moc_amplifier.moc"
