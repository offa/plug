#include "save_effects.h"
#include "ui_save_effects.h"

#include "mainwindow.h"

SaveEffects::SaveEffects(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Save_effects)
{
    ui->setupUi(this);

    QSettings settings;
    restoreGeometry(settings.value("Windows/saveEffectPresetWindowGeometry").toByteArray());

    connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(select_checkbox()));
    connect(ui->checkBox_2, SIGNAL(clicked()), this, SLOT(select_checkbox()));
    connect(ui->checkBox_3, SIGNAL(clicked()), this, SLOT(select_checkbox()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(send()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
}

SaveEffects::~SaveEffects()
{
    QSettings settings;
    settings.setValue("Windows/saveEffectPresetWindowGeometry", saveGeometry());
    delete ui;
}

void SaveEffects::select_checkbox()
{
    if(!ui->checkBox->isChecked() && !ui->checkBox_2->isChecked() && !ui->checkBox_3->isChecked())
        ui->pushButton->setDisabled(true);
    else
        ui->pushButton->setDisabled(false);

    if(sender() == ui->checkBox)
    {
        ui->checkBox_2->setChecked(false);
        ui->checkBox_3->setChecked(false);
    }
    else
        ui->checkBox->setChecked(false);
}

void SaveEffects::send()
{
    int number = 0;

    if(ui->checkBox->isChecked())
        number = 1;
    else
    {
        if(ui->checkBox_2->isChecked())
            number++;
        if(ui->checkBox_3->isChecked())
            number++;
    }

    dynamic_cast<MainWindow*>(parent())->save_effects(ui->comboBox->currentIndex(), ui->lineEdit->text().toLatin1().data(), number,
                              ui->checkBox->isChecked(), ui->checkBox_2->isChecked(), ui->checkBox_3->isChecked());
    this->close();
}


#include "moc_save_effects.moc"
