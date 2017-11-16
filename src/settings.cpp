#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) : QDialog(parent),
                                    ui(std::make_unique<Ui::Settings>())
{
    QSettings settings;

    ui->setupUi(this);

    ui->checkBox->setChecked(settings.value("Settings/checkForUpdates").toBool());
    ui->checkBox_2->setChecked(settings.value("Settings/connectOnStartup").toBool());
    ui->checkBox_3->setChecked(settings.value("Settings/oneSetToSetThemAll").toBool());
    ui->checkBox_4->setChecked(settings.value("Settings/keepWindowsOpen").toBool());
    ui->checkBox_5->setChecked(settings.value("Settings/popupChangedWindows").toBool());
    ui->checkBox_6->setChecked(settings.value("Settings/defaultEffectValues").toBool());

    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(change_updates(bool)));
    connect(ui->checkBox_2, SIGNAL(toggled(bool)), this, SLOT(change_connect(bool)));
    connect(ui->checkBox_3, SIGNAL(toggled(bool)), this, SLOT(change_oneset(bool)));
    connect(ui->checkBox_4, SIGNAL(toggled(bool)), this, SLOT(change_keepopen(bool)));
    connect(ui->checkBox_5, SIGNAL(toggled(bool)), this, SLOT(change_popupwindows(bool)));
    connect(ui->checkBox_6, SIGNAL(toggled(bool)), this, SLOT(change_effectvalues(bool)));
}

void Settings::change_updates(bool value)
{
    QSettings settings;

    settings.setValue("Settings/checkForUpdates", value);
}

void Settings::change_connect(bool value)
{
    QSettings settings;

    settings.setValue("Settings/connectOnStartup", value);
}

void Settings::change_oneset(bool value)
{
    QSettings settings;

    settings.setValue("Settings/oneSetToSetThemAll", value);
}

void Settings::change_keepopen(bool value)
{
    QSettings settings;

    settings.setValue("Settings/keepWindowsOpen", value);
}

void Settings::change_popupwindows(bool value)
{
    QSettings settings;

    settings.setValue("Settings/popupChangedWindows", value);
}

void Settings::change_effectvalues(bool value)
{
    QSettings settings;

    settings.setValue("Settings/defaultEffectValues", value);
}


#include "moc_settings.moc"
