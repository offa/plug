#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // load window size
    QSettings settings;
    restoreGeometry(settings.value("Windows/mainWindowGeometry").toByteArray());
    restoreState(settings.value("Windows/mainWindowState").toByteArray());

    // setting default values if there was none
    if(!settings.contains("Settings/checkForUpdates"))
        settings.setValue("Settings/checkForUpdates", true);
    if(!settings.contains("Settings/connectOnStartup"))
        settings.setValue("Settings/connectOnStartup", true);
    if(!settings.contains("Settings/oneSetToSetThemAll"))
        settings.setValue("Settings/oneSetToSetThemAll", false);
    if(!settings.contains("Settings/keepWindowsOpen"))
        settings.setValue("Settings/keepWindowsOpen", false);
    if(!settings.contains("Settings/popupChangedWindows"))
        settings.setValue("Settings/popupChangedWindows", true);
    if(!settings.contains("Settings/defaultEffectValues"))
        settings.setValue("Settings/defaultEffectValues", true);

    // create child objects
    amp_ops = new Mustang();
    amp = new Amplifier(this);
    effect1 = new Effect(this, 0);
    effect2 = new Effect(this, 1);
    effect3 = new Effect(this, 2);
    effect4 = new Effect(this, 3);

    about_window = new About(this);
    save = new SaveOnAmp(this);
    load = new LoadFromAmp(this);
    seffects = new Save_effects(this);
    settings_win = new Settings(this);
    saver = new SaveToFile(this);

    this->show();

    // connect buttons to slots
    connect(ui->Amplifier, SIGNAL(clicked()), amp, SLOT(show()));
    connect(ui->EffectButton1, SIGNAL(clicked()), effect1, SLOT(show()));
    connect(ui->EffectButton2, SIGNAL(clicked()), effect2, SLOT(show()));
    connect(ui->EffectButton3, SIGNAL(clicked()), effect3, SLOT(show()));
    connect(ui->EffectButton4, SIGNAL(clicked()), effect4, SLOT(show()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(start_amp()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(stop_amp()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), about_window, SLOT(open()));
    connect(ui->actionSave_to_amplifier, SIGNAL(triggered()), save, SLOT(show()));
    connect(ui->action_Load_from_amplifier, SIGNAL(triggered()), load, SLOT(show()));
    connect(ui->actionSave_effects, SIGNAL(triggered()), seffects, SLOT(open()));
    connect(ui->actionCheck_for_Updates, SIGNAL(triggered()), this, SLOT(check_for_updates()));
    connect(ui->action_Options, SIGNAL(triggered()), settings_win, SLOT(show()));
    connect(ui->actionL_oad_from_file, SIGNAL(triggered()), this, SLOT(loadfile()));
    connect(ui->actionS_ave_to_file, SIGNAL(triggered()), saver, SLOT(show()));

    // shortcuts to activate effect windows
    QShortcut *showfx1 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_1), this, 0, 0, Qt::ApplicationShortcut);
    QShortcut *showfx2 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_2), this, 0, 0, Qt::ApplicationShortcut);
    QShortcut *showfx3 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_3), this, 0, 0, Qt::ApplicationShortcut);
    QShortcut *showfx4 = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_4), this, 0, 0, Qt::ApplicationShortcut);
    QShortcut *showamp = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_5), this, 0, 0, Qt::ApplicationShortcut);
    connect(showfx1, SIGNAL(activated()), this, SLOT(show_fx1()));
    connect(showfx2, SIGNAL(activated()), this, SLOT(show_fx2()));
    connect(showfx3, SIGNAL(activated()), this, SLOT(show_fx3()));
    connect(showfx4, SIGNAL(activated()), this, SLOT(show_fx4()));
    connect(showamp, SIGNAL(activated()), this, SLOT(show_amp()));

    // shortcut to activate buttons
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(enable_buttons()));

    // connect the functions if needed
    if(settings.value("Settings/checkForUpdates").toBool())
        connect(this, SIGNAL(started()), this, SLOT(check_for_updates()));
    if(settings.value("Settings/connectOnStartup").toBool())
        connect(this, SIGNAL(started()), this, SLOT(start_amp()));

    emit started();
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.setValue("Windows/mainWindowGeometry", saveGeometry());
    settings.setValue("Windows/mainWindowState", saveState());
    delete amp_ops;    // stop the communication before exiting
    delete ui;
}

void MainWindow::start_amp()
{
    QSettings settings;
    int x;
    struct amp_settings amplifier_set;
    struct fx_pedal_settings effects_set[4];
    char name[32], names[24][32];

    ui->statusBar->showMessage(tr("Connecting..."));
    this->repaint();  // this should not be needed!
    x = amp_ops->start_amp(names, name, &amplifier_set, effects_set);    // request initialization of communication

    if(x != 0)    // if request succeded
    {
        if(x == -100)
            ui->statusBar->showMessage(tr("Suitable device not found!"), 5000);
        else
            ui->statusBar->showMessage(QString(tr("Error: %1")).arg(x), 5000);
        return;
    }

    load->load_names(names);
    save->load_names(names);

    if(name[0] == 0x00)
        setWindowTitle(QString(tr("PLUG: NONE")));
    else
        setWindowTitle(QString(tr("PLUG: %1")).arg(name));

    current_name = name;

    amp->load(amplifier_set);
    if(settings.value("Settings/popupChangedWindows").toBool())
        amp->show();
    for(int i = 0; i < 4; i++)
    {
        switch(effects_set[i].fx_slot)
        {
        case 0x00:
        case 0x04:
            effect1->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect1->show();
            break;

        case 0x01:
        case 0x05:
            effect2->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect2->show();
            break;

        case 0x02:
        case 0x06:
            effect3->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect3->show();
            break;

        case 0x03:
        case 0x07:
            effect4->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect4->show();
            break;
        }
    }

    // activate buttons
    ui->Amplifier->setDisabled(false);
    ui->EffectButton1->setDisabled(false);
    ui->EffectButton2->setDisabled(false);
    ui->EffectButton3->setDisabled(false);
    ui->EffectButton4->setDisabled(false);
    ui->actionConnect->setDisabled(true);
    ui->actionDisconnect->setDisabled(false);
    ui->actionSave_to_amplifier->setDisabled(false);
    ui->action_Load_from_amplifier->setDisabled(false);
    ui->actionSave_effects->setDisabled(false);
    ui->actionL_oad_from_file->setDisabled(false);
    ui->actionS_ave_to_file->setDisabled(false);
    ui->statusBar->showMessage(tr("Connected"), 3000);    // show message on the status bar
}

void MainWindow::stop_amp()
{
    int x;

    x = amp_ops->stop_amp();
    if(x == 0)    // if request succeded
    {
        // deactivate buttons
        ui->Amplifier->setDisabled(true);
        ui->EffectButton1->setDisabled(true);
        ui->EffectButton2->setDisabled(true);
        ui->EffectButton3->setDisabled(true);
        ui->EffectButton4->setDisabled(true);
        ui->actionConnect->setDisabled(false);
        ui->actionDisconnect->setDisabled(true);
        ui->actionSave_to_amplifier->setDisabled(true);
        ui->action_Load_from_amplifier->setDisabled(true);
        ui->actionSave_effects->setDisabled(true);
        ui->actionL_oad_from_file->setDisabled(true);
        ui->actionS_ave_to_file->setDisabled(true);
        setWindowTitle(QString(tr("PLUG")));
        ui->statusBar->showMessage(tr("Disconnected"), 5000);    // show message on the status bar
    }
    else    // if request failed
        ui->statusBar->showMessage(QString(tr("Error: %1")).arg(x), 5000);
}

// pass the message to the amp
int MainWindow::set_effect(struct fx_pedal_settings pedal)
{
    QSettings settings;

    if(!settings.value("Settings/oneSetToSetThemAll").toBool())
        return amp_ops->set_effect(pedal);
    amp->send_amp();
    return 0;
}

int MainWindow::set_amplifier(struct amp_settings amp_settings)
{
    QSettings settings;

    if(settings.value("Settings/oneSetToSetThemAll").toBool())
    {
        struct fx_pedal_settings pedal;

        if(effect1->get_changed())
        {
            effect1->get_settings(pedal);
            amp_ops->set_effect(pedal);
        }
        if(effect2->get_changed())
        {
            effect2->get_settings(pedal);
            amp_ops->set_effect(pedal);
        }
        if(effect3->get_changed())
        {
            effect3->get_settings(pedal);
            amp_ops->set_effect(pedal);
        }
        if(effect4->get_changed())
        {
            effect4->get_settings(pedal);
            amp_ops->set_effect(pedal);
        }
    }

    return amp_ops->set_amplifier(amp_settings);
}

int MainWindow::save_on_amp(char *name, int slot)
{
    int ret;

    ret = amp_ops->save_on_amp(name, slot);

    if(name[0] == 0x00)
        setWindowTitle(QString(tr("PLUG: NONE")));
    else
        setWindowTitle(QString(tr("PLUG: %1")).arg(name));

    current_name=name;

    return ret;
}

int MainWindow::load_from_amp(int slot)
{
    QSettings settings;
    struct amp_settings amplifier_set;
    struct fx_pedal_settings effects_set[4];
    char name[32];

    amp_ops->load_memory_bank(slot, name, &amplifier_set, effects_set);

    if(name[0] == 0x00)
        setWindowTitle(QString(tr("PLUG: NONE")));
    else
        setWindowTitle(QString(tr("PLUG: %1")).arg(name));

    current_name=name;

    amp->load(amplifier_set);
    if(settings.value("Settings/popupChangedWindows").toBool())
        amp->show();
    for(int i = 0; i < 4; i++)
    {
        switch(effects_set[i].fx_slot)
        {
        case 0x00:
        case 0x04:
            effect1->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect1->show();
            break;

        case 0x01:
        case 0x05:
            effect2->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect2->show();
            break;

        case 0x02:
        case 0x06:
            effect3->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect3->show();
            break;

        case 0x03:
        case 0x07:
            effect4->load(effects_set[i]);
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect4->show();
            break;
        }
    }

    return 0;
}

// activate buttons
void MainWindow::enable_buttons(void)
{
    ui->Amplifier->setDisabled(false);
    ui->EffectButton1->setDisabled(false);
    ui->EffectButton2->setDisabled(false);
    ui->EffectButton3->setDisabled(false);
    ui->EffectButton4->setDisabled(false);
    ui->actionConnect->setDisabled(false);
    ui->actionDisconnect->setDisabled(false);
    ui->actionSave_to_amplifier->setDisabled(false);
    ui->action_Load_from_amplifier->setDisabled(false);
    ui->actionSave_effects->setDisabled(false);
    ui->actionL_oad_from_file->setDisabled(false);
    ui->actionS_ave_to_file->setDisabled(false);
}

void MainWindow::check_for_updates()
{
    if(sender() == ui->actionCheck_for_Updates)
        manual_check = true;
    else
        manual_check = false;

    QNetworkAccessManager *qnam = new QNetworkAccessManager(this);
    reply = qnam->get(QNetworkRequest((QUrl)"http://piorekf.org/plug/VERSION"));
    connect(reply, SIGNAL(finished()), this, SLOT(httpReadyRead()));
}

void MainWindow::httpReadyRead()
{
    if(reply->readAll() > VERSION)
    {
        QLabel *label = new QLabel(tr("<b>Update available!</b>"), this);
        ui->statusBar->addWidget(label);
//        QMessageBox *msgbx = new QMessageBox(this);
//        msgbx->setWindowTitle(tr("Update"));
//        msgbx->setText(tr("<b>Update available!</b>"));
//        msgbx->setInformativeText(tr("Check homepage for new version"));
//        msgbx->setIcon(QMessageBox::Information);
//        msgbx->exec();
        QMessageBox::information(this, "Update", "<b>Update available!</b><br><br>Check homepage for new version.");
    }
    else if(manual_check)
        ui->statusBar->showMessage(tr("You are using the newest version"), 5000);
}

void MainWindow::change_name(int slot, QString *name)
{
    load->change_name(slot, name);
}

void MainWindow::set_index(int value)
{
    save->change_index(value, current_name);
}

void MainWindow::save_effects(int slot, char *name, int fx_num, bool mod, bool dly, bool rev)
{
    struct fx_pedal_settings effects[2];

    if(fx_num == 1)
    {
        if(mod)
        {
            effect2->get_settings(effects[0]);
            set_effect(effects[0]);
        }
        else if(dly)
        {
            effect3->get_settings(effects[0]);
            set_effect(effects[0]);
        }
        else if(rev)
        {
            effect4->get_settings(effects[0]);
            set_effect(effects[0]);
        }
    }
    else
    {
        effect3->get_settings(effects[0]);
        set_effect(effects[0]);
        effect4->get_settings(effects[1]);
        set_effect(effects[1]);
    }

    amp_ops->save_effects(slot, name, fx_num, effects);
}

void MainWindow::loadfile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open..."), QDir::homePath(), tr("FUSE files (*.fuse *.xml)"));

    if(filename.isEmpty())
        return;

    QFile *file = new QFile(filename, this);

    if (!file->open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, tr("Error!"), tr("Could not open file"));
        return;
    }

    QSettings settings;
    struct amp_settings amplifier_set;
    struct fx_pedal_settings effects_set[4];
    QString name;
    LoadFromFile *loader = new LoadFromFile(file, &name, &amplifier_set, effects_set);

    loader->loadfile();
    file->close();
    delete loader;
    delete file;

    change_title(name);

    amp->load(amplifier_set);
    amp->send_amp();
    if(settings.value("Settings/popupChangedWindows").toBool())
        amp->show();
    for(int i = 0; i < 4; i++)
    {
        switch(effects_set[i].fx_slot)
        {
        case 0x00:
        case 0x04:
            effect1->load(effects_set[i]);
            effect1->send_fx();
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect1->show();
            break;

        case 0x01:
        case 0x05:
            effect2->load(effects_set[i]);
            effect2->send_fx();
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect2->show();
            break;

        case 0x02:
        case 0x06:
            effect3->load(effects_set[i]);
            effect3->send_fx();
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect3->show();
            break;

        case 0x03:
        case 0x07:
            effect4->load(effects_set[i]);
            effect4->send_fx();
            if(effects_set[i].effect_num)
                if(settings.value("Settings/popupChangedWindows").toBool())
                    effect4->show();
            break;
        }
    }
}

void MainWindow::get_settings(struct amp_settings *amplifier_settings, struct fx_pedal_settings fx_settings[4])
{
    amp->get_settings(amplifier_settings);
    effect1->get_settings(fx_settings[0]);
    effect2->get_settings(fx_settings[1]);
    effect3->get_settings(fx_settings[2]);
    effect4->get_settings(fx_settings[3]);
}

void MainWindow::change_title(QString name)
{
    current_name = name;

    if(current_name.isEmpty())
        setWindowTitle(QString(tr("PLUG: NONE")));
    else
        setWindowTitle(QString(tr("PLUG: %1")).arg(current_name));
}

void MainWindow::show_fx1()
{
    if(!effect1->isVisible())
        effect1->show();
    effect1->activateWindow();
}

void MainWindow::show_fx2()
{
    if(!effect2->isVisible())
        effect2->show();
    effect2->activateWindow();
}
void MainWindow::show_fx3()
{
    if(!effect3->isVisible())
        effect3->show();
    effect3->activateWindow();
}
void MainWindow::show_fx4()
{
    if(!effect4->isVisible())
        effect4->show();
    effect4->activateWindow();
}

void MainWindow::show_amp()
{
    if(!amp->isVisible())
        amp->show();
    amp->activateWindow();
}
