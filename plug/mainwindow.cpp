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
        settings.setValue("Settings/connectOnStartup", false);
    if(!settings.contains("Settings/oneSetToSetThemAll"))
        settings.setValue("Settings/oneSetToSetThemAll", false);
    if(!settings.contains("Settings/keepWindowsOpen"))
        settings.setValue("Settings/keepWindowsOpen", false);
    if(!settings.contains("Settings/popupChangedWindows"))
        settings.setValue("Settings/popupChangedWindows", false);

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

    this->show();

    ui->actionSave_effects->setShortcutContext(Qt::ApplicationShortcut);
    ui->actionSave_to_amplifier->setShortcutContext(Qt::ApplicationShortcut);
    ui->action_Load_from_amplifier->setShortcutContext(Qt::ApplicationShortcut);
    ui->actionExit->setShortcutContext(Qt::ApplicationShortcut);

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
    connect(ui->action_Configure, SIGNAL(triggered()), settings_win, SLOT(show()));

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

    ui->statusBar->showMessage(tr("Connecting..."), 0);
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
    ui->actionSave_to_amplifier->setDisabled(false);
    ui->action_Load_from_amplifier->setDisabled(false);
    ui->actionSave_effects->setDisabled(false);
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
        ui->actionSave_to_amplifier->setDisabled(true);
        ui->action_Load_from_amplifier->setDisabled(true);
        ui->actionSave_effects->setDisabled(true);
        ui->statusBar->showMessage(tr("Disconnected"), 3000);    // show message on the status bar
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
    ui->actionSave_to_amplifier->setDisabled(false);
    ui->action_Load_from_amplifier->setDisabled(false);
    ui->actionSave_effects->setDisabled(false);
}

void MainWindow::check_for_updates()
{
    QNetworkAccessManager *qnam = new QNetworkAccessManager(this);
    reply = qnam->get(QNetworkRequest((QUrl)"http://piorekf.org/plug/VERSION"));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
}

void MainWindow::httpReadyRead()
{
    if(reply->readAll()>VERSION)
    {
        QLabel *label = new QLabel("Update available!", this);
        ui->statusBar->addWidget(label);
        QMessageBox::information(this, "Update", "Update available!\nCheck homepage for new version.");
    }
    else
        ui->statusBar->showMessage("You are using the newest version", 5000);
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
