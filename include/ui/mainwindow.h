/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2023  offa
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
#include <QMainWindow>
#include <array>
#include <memory>

namespace Ui
{
    class MainWindow;
}

namespace plug
{
    class Effect;
    class Amplifier;
    class SaveEffects;
    class SaveToFile;
    class SaveOnAmp;
    class LoadFromAmp;
    class Settings;
    class QuickPresets;

    namespace com
    {
        class Mustang;
    }
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
        void start_amp();
        void stop_amp();
        void set_effect(fx_pedal_settings);
        void set_amplifier(amp_settings);
        void save_on_amp(char*, int);
        void load_from_amp(int);
        void enable_buttons();
        void change_name(int, QString*);
        void save_effects(int, char*, int, bool, bool, bool);
        void set_index(int);
        void loadfile(QString filename = QString());
        void get_settings(amp_settings*, std::vector<fx_pedal_settings>&);
        void change_title(const QString&);
        void update_firmware();
        void empty_other(int, Effect*);

    private:
        const std::unique_ptr<Ui::MainWindow> ui;

        QString current_name;
        std::vector<std::string> presetNames;
        bool connected;
        std::unique_ptr<com::Mustang> amp_ops;
        Amplifier* amp;
        std::array<Effect*, 8> effectComponents;
        SaveOnAmp* save;
        LoadFromAmp* load;
        SaveEffects* seffects;
        Settings* settings_win;
        SaveToFile* saver;
        QuickPresets* quickpres;

    private slots:
        void about();
        void showEffect(std::uint8_t slot);
        void show_amp();
        void show_library();
        void show_default_effects();
        void loadPreset(std::size_t number);


    signals:
        void started();
    };
}
