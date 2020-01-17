/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2020  offa
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

#include "ui/mainwindow.h"
#include "ui/amplifier.h"
#include "ui/defaulteffects.h"
#include "ui/effect.h"
#include "ui/library.h"
#include "ui/loadfromamp.h"
#include "ui/loadfromfile.h"
#include "ui/quickpresets.h"
#include "ui/save_effects.h"
#include "ui/saveonamp.h"
#include "ui/savetofile.h"
#include "ui/settings.h"
#include "com/Mustang.h"
#include "com/ConnectionFactory.h"
#include "com/CommunicationException.h"
#include "com/MustangUpdater.h"
#include "ui_defaulteffects.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>
#include <QDebug>

namespace plug
{
    namespace
    {
        constexpr int check_fx_family(effects value)
        {
            if (value == effects::EMPTY)
                return 0;
            if ((value >= effects::OVERDRIVE) && (value <= effects::COMPRESSOR))
                return 1;
            if ((value >= effects::SINE_CHORUS) && (value <= effects::PITCH_SHIFTER))
                return 2;
            if ((value >= effects::MONO_DELAY) && (value <= effects::STEREO_TAPE_DELAY))
                return 3;
            if ((value >= effects::SMALL_HALL_REVERB) && (value <= effects::FENDER_65_SPRING_REVERB))
                return 4;

            return 0;
        }

    }


    MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent),
          ui(std::make_unique<Ui::MainWindow>()),
          presetNames(100, ""),
          amp_ops(nullptr)
    {
        ui->setupUi(this);

        // load window size
        QSettings settings;
        restoreGeometry(settings.value("Windows/mainWindowGeometry").toByteArray());
        restoreState(settings.value("Windows/mainWindowState").toByteArray());

        // setting default values if there was none
        if (!settings.contains("Settings/connectOnStartup"))
            settings.setValue("Settings/connectOnStartup", true);
        if (!settings.contains("Settings/oneSetToSetThemAll"))
            settings.setValue("Settings/oneSetToSetThemAll", false);
        if (!settings.contains("Settings/keepWindowsOpen"))
            settings.setValue("Settings/keepWindowsOpen", false);
        if (!settings.contains("Settings/popupChangedWindows"))
            settings.setValue("Settings/popupChangedWindows", true);
        if (!settings.contains("Settings/defaultEffectValues"))
            settings.setValue("Settings/defaultEffectValues", true);

        // create child objects
        amp = new Amplifier(this);
        effect1 = new Effect(this, 0);
        effect2 = new Effect(this, 1);
        effect3 = new Effect(this, 2);
        effect4 = new Effect(this, 3);

        save = new SaveOnAmp(this);
        load = new LoadFromAmp(this);
        seffects = new SaveEffects(this);
        settings_win = new Settings(this);
        saver = new SaveToFile(this);
        quickpres = new QuickPresets(this);

        connected = false;

        // connect buttons to slots
        connect(ui->Amplifier, SIGNAL(clicked()), amp, SLOT(showAndActivate()));
        connect(ui->EffectButton1, SIGNAL(clicked()), effect1, SLOT(showAndActivate()));
        connect(ui->EffectButton2, SIGNAL(clicked()), effect2, SLOT(showAndActivate()));
        connect(ui->EffectButton3, SIGNAL(clicked()), effect3, SLOT(showAndActivate()));
        connect(ui->EffectButton4, SIGNAL(clicked()), effect4, SLOT(showAndActivate()));
        connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(start_amp()));
        connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(stop_amp()));
        connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
        connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
        connect(ui->actionSave_to_amplifier, SIGNAL(triggered()), save, SLOT(show()));
        connect(ui->action_Load_from_amplifier, SIGNAL(triggered()), load, SLOT(show()));
        connect(ui->actionSave_effects, SIGNAL(triggered()), seffects, SLOT(open()));
        connect(ui->action_Options, SIGNAL(triggered()), settings_win, SLOT(show()));
        connect(ui->actionL_oad_from_file, SIGNAL(triggered()), this, SLOT(loadfile()));
        connect(ui->actionS_ave_to_file, SIGNAL(triggered()), saver, SLOT(show()));
        connect(ui->action_Library_view, SIGNAL(triggered()), this, SLOT(show_library()));
        connect(ui->action_Update_firmware, SIGNAL(triggered()), this, SLOT(update_firmware()));
        connect(ui->action_Default_effects, SIGNAL(triggered()), this, SLOT(show_default_effects()));
        connect(ui->action_Quick_presets, SIGNAL(triggered()), quickpres, SLOT(show()));

        // shortcuts to activate effect windows
        QShortcut* showfx1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_1), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* showfx2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_2), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* showfx3 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_3), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* showfx4 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_4), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* showamp = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_5), this, nullptr, nullptr, Qt::ApplicationShortcut);
        connect(showfx1, SIGNAL(activated()), this, SLOT(show_fx1()));
        connect(showfx2, SIGNAL(activated()), this, SLOT(show_fx2()));
        connect(showfx3, SIGNAL(activated()), this, SLOT(show_fx3()));
        connect(showfx4, SIGNAL(activated()), this, SLOT(show_fx4()));
        connect(showamp, SIGNAL(activated()), this, SLOT(show_amp()));

        // shortcuts for quick loading presets
        QShortcut* loadpres0 = new QShortcut(QKeySequence(Qt::Key_0), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres1 = new QShortcut(QKeySequence(Qt::Key_1), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres2 = new QShortcut(QKeySequence(Qt::Key_2), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres3 = new QShortcut(QKeySequence(Qt::Key_3), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres4 = new QShortcut(QKeySequence(Qt::Key_4), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres5 = new QShortcut(QKeySequence(Qt::Key_5), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres6 = new QShortcut(QKeySequence(Qt::Key_6), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres7 = new QShortcut(QKeySequence(Qt::Key_7), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres8 = new QShortcut(QKeySequence(Qt::Key_8), this, nullptr, nullptr, Qt::ApplicationShortcut);
        QShortcut* loadpres9 = new QShortcut(QKeySequence(Qt::Key_9), this, nullptr, nullptr, Qt::ApplicationShortcut);
        connect(loadpres0, SIGNAL(activated()), this, SLOT(load_presets0()));
        connect(loadpres1, SIGNAL(activated()), this, SLOT(load_presets1()));
        connect(loadpres2, SIGNAL(activated()), this, SLOT(load_presets2()));
        connect(loadpres3, SIGNAL(activated()), this, SLOT(load_presets3()));
        connect(loadpres4, SIGNAL(activated()), this, SLOT(load_presets4()));
        connect(loadpres5, SIGNAL(activated()), this, SLOT(load_presets5()));
        connect(loadpres6, SIGNAL(activated()), this, SLOT(load_presets6()));
        connect(loadpres7, SIGNAL(activated()), this, SLOT(load_presets7()));
        connect(loadpres8, SIGNAL(activated()), this, SLOT(load_presets8()));
        connect(loadpres9, SIGNAL(activated()), this, SLOT(load_presets9()));

        // shortcut to activate buttons
        QShortcut* shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A), this);
        connect(shortcut, SIGNAL(activated()), this, SLOT(enable_buttons()));

        // connect the functions if needed
        if (settings.value("Settings/connectOnStartup").toBool())
        {
            connect(this, SIGNAL(started()), this, SLOT(start_amp()));
        }

        this->show();
        this->repaint();

        emit started();
    }

    MainWindow::~MainWindow()
    {
        QSettings settings;
        settings.setValue("Windows/mainWindowGeometry", saveGeometry());
        settings.setValue("Windows/mainWindowState", saveState());
    }

    void MainWindow::about()
    {
        const QString title{tr("About %1").arg(QCoreApplication::applicationName())};
        const QString text{tr("<center>"
                              "<h2>%1</h2>"
                              "<i>v%2</i>"
                              "<p>%1 is a GPLv3 licensed program designed as a replacement for Fender FUSE to operate Fender Mustang amplifier and possibly some other models.</p>"
                              "<p><a href=\"https://github.com/offa/plug/\">Plug (GitHub)</a> / <a href=\"https://gitlab.com/offa/plug/\">Plug (GitLab)</a></p>"
                              "<p><a href=\"http://piorekf.org/plug/\">piorekf.org/plug</a></p>"
                              "<hr>"
                              "<p>Copyright © 2017-2020 offa</p>"
                              "<p>Copyright © 2010-2016 piorekf <piorek@piorekf.org></p>"
                              "<p>License: <a href=\"http://www.gnu.org/licenses/gpl.txt\">GPLv3</a></p>"
                              "</center>")
                               .arg(QCoreApplication::applicationName(),
                                    QCoreApplication::applicationVersion())};

        QMessageBox::about(this, title, text);
    }


    void MainWindow::start_amp()
    {
        QSettings settings;
        amp_settings amplifier_set{};
        std::array<fx_pedal_settings, 4> effects_set{{}};
        QString name;

        ui->statusBar->showMessage(tr("Connecting..."));
        this->repaint(); // this should not be needed!

        try
        {
            amp_ops = std::make_unique<com::Mustang>(com::createUsbConnection());
            const auto [signalChain, presets] = amp_ops->start_amp();
            name = QString::fromStdString(signalChain.name());
            amplifier_set = signalChain.amp();
            effects_set = signalChain.effects();
            presetNames = presets;
        }
        catch (const std::exception& ex)
        {
            qWarning() << "ERROR: " << ex.what();
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
            return;
        }

        load->load_names(presetNames);
        save->load_names(presetNames);
        quickpres->load_names(presetNames);

        if (name.isEmpty() == true)
        {
            setWindowTitle(QString(tr("PLUG: NONE")));
            setAccessibleName(QString(tr("Main window: NONE")));
        }
        else
        {
            setWindowTitle(QString(tr("PLUG: %1")).arg(name));
            setAccessibleName(QString(tr("Main window: %1")).arg(name));
        }

        current_name = name;

        amp->load(amplifier_set);
        if (settings.value("Settings/popupChangedWindows").toBool())
        {
            amp->show();
        }
        for (int i = 0; i < 4; i++)
        {
            switch (effects_set[i].fx_slot)
            {
                case 0x00:
                case 0x04:
                    effect1->load(effects_set[i]);
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect1->show();
                    break;

                case 0x01:
                case 0x05:
                    effect2->load(effects_set[i]);
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect2->show();
                    break;

                case 0x02:
                case 0x06:
                    effect3->load(effects_set[i]);
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect3->show();
                    break;

                case 0x03:
                case 0x07:
                    effect4->load(effects_set[i]);
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect4->show();
                    break;
            }
        }

        // activate buttons
        amp->enable_set_button(true);
        effect1->enable_set_button(true);
        effect2->enable_set_button(true);
        effect3->enable_set_button(true);
        effect4->enable_set_button(true);
        ui->actionConnect->setDisabled(true);
        ui->actionDisconnect->setDisabled(false);
        ui->actionSave_to_amplifier->setDisabled(false);
        ui->action_Load_from_amplifier->setDisabled(false);
        ui->actionSave_effects->setDisabled(false);
        ui->action_Library_view->setDisabled(false);
        ui->statusBar->showMessage(tr("Connected"), 3000);

        connected = true;
    }

    void MainWindow::stop_amp()
    {
        save->delete_items();
        load->delete_items();
        quickpres->delete_items();

        try
        {
            amp_ops->stop_amp();

            // deactivate buttons
            amp->enable_set_button(false);
            effect1->enable_set_button(false);
            effect2->enable_set_button(false);
            effect3->enable_set_button(false);
            effect4->enable_set_button(false);
            ui->actionConnect->setDisabled(false);
            ui->actionDisconnect->setDisabled(true);
            ui->actionSave_to_amplifier->setDisabled(true);
            ui->action_Load_from_amplifier->setDisabled(true);
            ui->actionSave_effects->setDisabled(true);
            ui->action_Library_view->setDisabled(true);
            setWindowTitle(QString(tr("PLUG")));
            setAccessibleName(QString(tr("Main window: None")));
            ui->statusBar->showMessage(tr("Disconnected"), 5000);

            connected = false;
        }
        catch (const std::exception& ex)
        {
            qWarning() << "ERROR: " << ex.what();
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
            return;
        }
    }

    // pass the message to the amp
    void MainWindow::set_effect(fx_pedal_settings pedal)
    {
        if (!connected)
        {
            return;
        }

        QSettings settings;

        if (!settings.value("Settings/oneSetToSetThemAll").toBool())
        {
            try
            {
                amp_ops->set_effect(pedal);
            }
            catch (const std::exception& ex)
            {
                qWarning() << "ERROR: " << ex.what();
                ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
                return;
            }
        }
        amp->send_amp();
    }

    void MainWindow::set_amplifier(amp_settings amp_settings)
    {

        if (!connected)
        {
            return;
        }

        QSettings settings;

        try
        {
            if (settings.value("Settings/oneSetToSetThemAll").toBool())
            {
                fx_pedal_settings pedal{};

                if (effect1->get_changed())
                {
                    effect1->get_settings(pedal);
                    amp_ops->set_effect(pedal);
                }
                if (effect2->get_changed())
                {
                    effect2->get_settings(pedal);
                    amp_ops->set_effect(pedal);
                }
                if (effect3->get_changed())
                {
                    effect3->get_settings(pedal);
                    amp_ops->set_effect(pedal);
                }
                if (effect4->get_changed())
                {
                    effect4->get_settings(pedal);
                    amp_ops->set_effect(pedal);
                }
            }

            amp_ops->set_amplifier(amp_settings);
        }
        catch (const std::exception& ex)
        {
            qWarning() << "ERROR: " << ex.what();
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
            return;
        }
    }

    void MainWindow::save_on_amp(char* name, int slot)
    {
        if (connected == false)
        {
            return;
        }

        try
        {
            amp_ops->save_on_amp(name, static_cast<std::uint8_t>(slot));
        }
        catch (const std::exception& ex)
        {
            qWarning() << "ERROR: " << ex.what();
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
            return;
        }

        if (name[0] == 0x00)
        {
            setWindowTitle(QString(tr("PLUG: NONE")));
            setAccessibleName(QString(tr("Main window: NONE")));
        }
        else
        {
            setWindowTitle(QString(tr("PLUG: %1")).arg(name));
            setAccessibleName(QString(tr("Main window: %1")).arg(name));
        }

        current_name = name;
        presetNames[slot] = current_name.toStdString();
    }

    void MainWindow::load_from_amp(int slot)
    {
        if (!connected)
        {
            return;
        }

        QSettings settings;
        try
        {
            const auto signalChain = amp_ops->load_memory_bank(static_cast<std::uint8_t>(slot));
            const QString bankName = QString::fromStdString(signalChain.name());


            if (bankName.isEmpty())
            {
                setWindowTitle(QString(tr("PLUG: NONE")));
                setAccessibleName(QString(tr("Main window: NONE")));
            }
            else
            {
                setWindowTitle(QString(tr("PLUG: %1")).arg(bankName));
                setAccessibleName(QString(tr("Main window: %1")).arg(bankName));
            }

            current_name = bankName;

            amp->load(signalChain.amp());
            if (settings.value("Settings/popupChangedWindows").toBool())
            {
                amp->show();
            }

            const auto effects_set = signalChain.effects();
            for (std::size_t i = 0; i < 4; i++)
            {
                switch (effects_set[i].fx_slot)
                {
                    case 0x00:
                    case 0x04:
                        effect1->load(effects_set[i]);
                        if (effects_set[i].effect_num != effects::EMPTY)
                            if (settings.value("Settings/popupChangedWindows").toBool())
                                effect1->show();
                        break;

                    case 0x01:
                    case 0x05:
                        effect2->load(effects_set[i]);
                        if (effects_set[i].effect_num != effects::EMPTY)
                            if (settings.value("Settings/popupChangedWindows").toBool())
                                effect2->show();
                        break;

                    case 0x02:
                    case 0x06:
                        effect3->load(effects_set[i]);
                        if (effects_set[i].effect_num != effects::EMPTY)
                            if (settings.value("Settings/popupChangedWindows").toBool())
                                effect3->show();
                        break;

                    case 0x03:
                    case 0x07:
                        effect4->load(effects_set[i]);
                        if (effects_set[i].effect_num != effects::EMPTY)
                        {
                            if (settings.value("Settings/popupChangedWindows").toBool())
                            {
                                effect4->show();
                            }
                        }
                        break;
                }
            }
        }
        catch (const std::exception& ex)
        {
            qWarning() << "ERROR: " << ex.what();
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
            return;
        }
    }

    // activate buttons
    void MainWindow::enable_buttons()
    {
        amp->enable_set_button(true);
        effect1->enable_set_button(true);
        effect2->enable_set_button(true);
        effect3->enable_set_button(true);
        effect4->enable_set_button(true);
        ui->actionConnect->setDisabled(false);
        ui->actionDisconnect->setDisabled(false);
        ui->actionSave_to_amplifier->setDisabled(false);
        ui->action_Load_from_amplifier->setDisabled(false);
        ui->actionSave_effects->setDisabled(false);
        ui->action_Library_view->setDisabled(false);
    }

    void MainWindow::change_name(int slot, QString* name)
    {
        load->change_name(slot, name);
        quickpres->change_name(slot, name);
    }

    void MainWindow::set_index(int value)
    {
        save->change_index(value, current_name);
    }

    void MainWindow::save_effects(int slot, char* name, int fx_num, bool mod, bool dly, bool rev)
    {
        std::vector<fx_pedal_settings> effects(static_cast<std::size_t>(fx_num));

        if (fx_num == 1)
        {
            if (mod)
            {
                effect2->get_settings(effects[0]);
                set_effect(effects[0]);
            }
            else if (dly)
            {
                effect3->get_settings(effects[0]);
                set_effect(effects[0]);
            }
            else if (rev)
            {
                effect4->get_settings(effects[0]);
                set_effect(effects[0]);
            }
            else
            {
                // Empty
            }
        }
        else
        {
            effect3->get_settings(effects[0]);
            set_effect(effects[0]);
            effect4->get_settings(effects[1]);
            set_effect(effects[1]);
        }

        try
        {
            amp_ops->save_effects(static_cast<std::uint8_t>(slot), name, effects);
        }
        catch (const std::exception& ex)
        {
            qWarning() << "ERROR: " << ex.what();
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(ex.what()), 5000);
            return;
        }
    }

    void MainWindow::loadfile(QString filename)
    {
        QSettings settings;

        if (filename.isEmpty())
        {
            filename = QFileDialog::getOpenFileName(this, tr("Open..."), settings.value("LoadFile/lastDirectory", QDir::homePath()).toString(), tr("FUSE files (*.fuse *.xml)"));
        }

        if (filename.isEmpty())
        {
            return;
        }

        settings.setValue("LoadFile/lastDirectory", QFileInfo(filename).absolutePath());
        auto file = std::make_unique<QFile>(filename, this);

        if (file->exists())
        {
            if (!file->open(QFile::ReadOnly | QFile::Text))
            {
                QMessageBox::critical(this, tr("Error!"), tr("Could not open file"));
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Error!"), tr("No such file"));
            return;
        }

        amp_settings amplifier_set{};
        fx_pedal_settings effects_set[4];
        QString name;
        auto loader = std::make_unique<LoadFromFile>(file.get(), &name, &amplifier_set, effects_set);
        loader->loadfile();
        file->close();

        change_title(name);

        amp->load(amplifier_set);
        if (connected)
        {
            amp->send_amp();
        }
        if (settings.value("Settings/popupChangedWindows").toBool())
        {
            amp->show();
        }

        for (int i = 0; i < 4; i++)
        {
            switch (effects_set[i].fx_slot)
            {
                case 0x00:
                    effect1->load(effects_set[i]);
                    if (connected)
                        effect1->send_fx();
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect1->show();
                    break;

                case 0x01:
                    effect2->load(effects_set[i]);
                    if (connected)
                        effect2->send_fx();
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect2->show();
                    break;

                case 0x02:
                    effect3->load(effects_set[i]);
                    if (connected)
                        effect3->send_fx();
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect3->show();
                    break;

                case 0x03:
                    effect4->load(effects_set[i]);
                    if (connected)
                        effect4->send_fx();
                    if (effects_set[i].effect_num != effects::EMPTY)
                        if (settings.value("Settings/popupChangedWindows").toBool())
                            effect4->show();
                    break;
            }
        }
    }

    void MainWindow::get_settings(amp_settings* amplifier_settings, fx_pedal_settings fx_settings[4])
    {
        if (amplifier_settings != nullptr)
        {
            amp->get_settings(amplifier_settings);
        }
        if (fx_settings != nullptr)
        {
            effect1->get_settings(fx_settings[0]);
            effect2->get_settings(fx_settings[1]);
            effect3->get_settings(fx_settings[2]);
            effect4->get_settings(fx_settings[3]);
        }
    }

    void MainWindow::change_title(const QString& name)
    {
        current_name = name;

        if (current_name.isEmpty())
        {
            setWindowTitle(QString(tr("PLUG: NONE")));
            setAccessibleName(QString(tr("Main window: NONE")));
        }
        else
        {
            setWindowTitle(QString(tr("PLUG: %1")).arg(current_name));
            setAccessibleName(QString(tr("Main window: %1")).arg(name));
        }
    }

    void MainWindow::show_fx1()
    {
        if (!effect1->isVisible())
        {
            effect1->show();
        }
        effect1->activateWindow();
    }

    void MainWindow::show_fx2()
    {
        if (!effect2->isVisible())
        {
            effect2->show();
        }
        effect2->activateWindow();
    }
    void MainWindow::show_fx3()
    {
        if (!effect3->isVisible())
        {
            effect3->show();
        }
        effect3->activateWindow();
    }
    void MainWindow::show_fx4()
    {
        if (!effect4->isVisible())
        {
            effect4->show();
        }
        effect4->activateWindow();
    }

    void MainWindow::show_amp()
    {
        if (!amp->isVisible())
        {
            amp->show();
        }
        amp->activateWindow();
    }

    void MainWindow::show_library()
    {
        QSettings settings;
        bool previous = settings.value("Settings/popupChangedWindows").toBool();

        settings.setValue("Settings/popupChangedWindows", false);

        library = std::make_unique<Library>(presetNames, this);
        effect1->close();
        effect2->close();
        effect3->close();
        effect4->close();
        amp->close();
        this->close();
        library->exec();

        settings.setValue("Settings/popupChangedWindows", previous);
        this->show();
    }

    void MainWindow::update_firmware()
    {
        QString filename;
        int ret = 0;

        QMessageBox::information(this, "Prepare", R"(Please power off the amplifier, then power it back on while holding down:<ul><li>The "Save" button (Mustang I and II)</li><li>The Data Wheel (Mustang III, IV and IV)</li></ul>After pressing "OK" choose firmware file and then update will begin.It will take about one minute. You will be notified when it's finished.)");

        filename = QFileDialog::getOpenFileName(this, tr("Open..."), QDir::homePath(), tr("Mustang firmware (*.upd)"));
        if (filename.isEmpty())
        {
            return;
        }

        if (connected)
        {
            this->stop_amp();
        }

        ui->statusBar->showMessage("Updating firmware. Please wait...");
        ui->centralWidget->setDisabled(true);
        ui->menuBar->setDisabled(true);
        this->repaint();
        ret = com::updateFirmware(filename.toLatin1().constData());
        ui->centralWidget->setDisabled(false);
        ui->menuBar->setDisabled(false);
        ui->statusBar->showMessage("", 1);
        if (ret == -100)
        {
            ui->statusBar->showMessage(tr("Error: Suitable device not found!"), 5000);
            return;
        }
        if (ret != 0)
        {
            ui->statusBar->showMessage(QString(tr("Communication error: %1")).arg(ret), 5000);
            return;
        }
        QMessageBox::information(this, "Update finished", R"(<b>Update finished</b><br>If "Exit" button is lit - update was succesful<br>If "Save" button is lit - update failed<br><br>Power off the amplifier and then back on to finish the process.)");
    }

    void MainWindow::show_default_effects()
    {
        deffx = std::make_unique<DefaultEffects>(this);
        deffx->exec();
        deffx.reset();
    }

    void MainWindow::empty_other(int value, Effect* caller)
    {
        const int fx_family = check_fx_family(static_cast<effects>(value));
        fx_pedal_settings settings{};

        if (caller != effect1)
        {
            effect1->get_settings(settings);
            if (check_fx_family(settings.effect_num) == fx_family)
            {
                effect1->choose_fx(0);
                effect1->send_fx();
            }
        }

        if (caller != effect2)
        {
            effect2->get_settings(settings);
            if (check_fx_family(settings.effect_num) == fx_family)
            {
                effect2->choose_fx(0);
                effect2->send_fx();
            }
        }

        if (caller != effect3)
        {
            effect3->get_settings(settings);
            if (check_fx_family(settings.effect_num) == fx_family)
            {
                effect3->choose_fx(0);
                effect3->send_fx();
            }
        }

        if (caller != effect4)
        {
            effect4->get_settings(settings);
            if (check_fx_family(settings.effect_num) == fx_family)
            {
                effect4->choose_fx(0);
                effect4->send_fx();
            }
        }
    }

    void MainWindow::load_presets0()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset0"))
            load_from_amp(settings.value("DefaultPresets/Preset0").toInt());
    }

    void MainWindow::load_presets1()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset1"))
            load_from_amp(settings.value("DefaultPresets/Preset1").toInt());
    }

    void MainWindow::load_presets2()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset2"))
            load_from_amp(settings.value("DefaultPresets/Preset2").toInt());
    }

    void MainWindow::load_presets3()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset3"))
            load_from_amp(settings.value("DefaultPresets/Preset3").toInt());
    }

    void MainWindow::load_presets4()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset4"))
            load_from_amp(settings.value("DefaultPresets/Preset4").toInt());
    }

    void MainWindow::load_presets5()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset5"))
            load_from_amp(settings.value("DefaultPresets/Preset5").toInt());
    }

    void MainWindow::load_presets6()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset6"))
            load_from_amp(settings.value("DefaultPresets/Preset6").toInt());
    }

    void MainWindow::load_presets7()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset7"))
            load_from_amp(settings.value("DefaultPresets/Preset7").toInt());
    }

    void MainWindow::load_presets8()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset8"))
            load_from_amp(settings.value("DefaultPresets/Preset8").toInt());
    }

    void MainWindow::load_presets9()
    {
        QSettings settings;

        if (settings.contains("DefaultPresets/Preset9"))
            load_from_amp(settings.value("DefaultPresets/Preset9").toInt());
    }
}

#include "ui/moc_mainwindow.moc"
