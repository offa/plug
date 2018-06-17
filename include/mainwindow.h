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

#pragma once

#include "amplifier.h"
#include "data_structs.h"
#include "defaulteffects.h"
#include "effect.h"
#include "library.h"
#include "loadfromamp.h"
#include "loadfromfile.h"
#include "mustang.h"
#include "quickpresets.h"
#include "save_effects.h"
#include "saveonamp.h"
#include "savetofile.h"
#include "settings.h"
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QtDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <memory>

namespace Ui
{
    class MainWindow;
}

namespace plug
{

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        MainWindow(const MainWindow&) = delete;
        ~MainWindow() override;

        MainWindow& operator=(const MainWindow&) = delete;

    public slots:
        void start_amp(); // initialize the communication
        void stop_amp();  // terminate communication
        int set_effect(fx_pedal_settings);
        int set_amplifier(amp_settings);
        int save_on_amp(char*, int);
        int load_from_amp(int);
        void enable_buttons();
        void change_name(int, QString*);
        void save_effects(int, char*, int, bool, bool, bool);
        void set_index(int);
        void loadfile(QString filename = QString());
        void get_settings(amp_settings*, fx_pedal_settings[4]);
        void change_title(const QString&);
        void update_firmware();
        void empty_other(int, Effect*);

    private:
        const std::unique_ptr<Ui::MainWindow> ui;

        // pointers to objects
        QString current_name;
        char names[100][32];
        bool manual_check, connected;
        const std::unique_ptr<Mustang> amp_ops;
        Amplifier* amp;
        Effect* effect1;
        Effect* effect2;
        Effect* effect3;
        Effect* effect4;
        SaveOnAmp* save;
        LoadFromAmp* load;
        SaveEffects* seffects;
        Settings* settings_win;
        SaveToFile* saver;
        std::unique_ptr<Library> library;
        std::unique_ptr<DefaultEffects> deffx;
        QuickPresets* quickpres;

        QNetworkReply* reply;

    private slots:
        void about();
        void httpReadyRead();
        void show_fx1();
        void show_fx2();
        void show_fx3();
        void show_fx4();
        void show_amp();
        void show_library();
        void show_default_effects();
        int check_fx_family(int);
        void load_presets0();
        void load_presets1();
        void load_presets2();
        void load_presets3();
        void load_presets4();
        void load_presets5();
        void load_presets6();
        void load_presets7();
        void load_presets8();
        void load_presets9();


    signals:
        void started();
    };
}
