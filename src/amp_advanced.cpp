#include "amp_advanced.h"
#include "ui_amp_advanced.h"

Amp_Advanced::Amp_Advanced(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Amp_Advanced)
{
    ui->setupUi(this);

    // load window size
    QSettings settings;
    restoreGeometry(settings.value("Windows/amplifierAdvancedWindowGeometry").toByteArray());

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), parent, SLOT(set_cabinet(int)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), parent, SLOT(set_noise_gate(int)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(activate_custom_ng(int)));
    connect(ui->dial, SIGNAL(valueChanged(int)), parent, SLOT(set_master_vol(int)));
    connect(ui->dial_2, SIGNAL(valueChanged(int)), parent, SLOT(set_gain2(int)));
    connect(ui->dial_3, SIGNAL(valueChanged(int)), parent, SLOT(set_presence(int)));
    connect(ui->dial_4, SIGNAL(valueChanged(int)), parent, SLOT(set_depth(int)));
    connect(ui->dial_5, SIGNAL(valueChanged(int)), parent, SLOT(set_threshold(int)));
    connect(ui->dial_6, SIGNAL(valueChanged(int)), parent, SLOT(set_bias(int)));
    connect(ui->dial_7, SIGNAL(valueChanged(int)), parent, SLOT(set_sag(int)));
    connect(ui->dial_8, SIGNAL(valueChanged(int)), parent, SLOT(set_usb_gain(int)));
    connect(ui->checkBox, SIGNAL(toggled(bool)), parent, SLOT(set_brightness(bool)));
}

Amp_Advanced::~Amp_Advanced()
{
    QSettings settings;
    settings.setValue("Windows/amplifierAdvancedWindowGeometry", saveGeometry());
    delete ui;
}

void Amp_Advanced::change_cabinet(int value)
{
    ui->comboBox->setCurrentIndex(value);
}

void Amp_Advanced::change_noise_gate(int value)
{
    ui->comboBox_2->setCurrentIndex(value);
}

void Amp_Advanced::activate_custom_ng(int value)
{
    if(value == 5)
    {
        ui->dial_5->setDisabled(false);
        ui->spinBox_5->setDisabled(false);
        ui->dial_4->setDisabled(false);
        ui->spinBox_4->setDisabled(false);
    }
    else
    {
        ui->dial_5->setDisabled(true);
        ui->spinBox_5->setDisabled(true);
        ui->dial_4->setDisabled(true);
        ui->spinBox_4->setDisabled(true);
    }
}

void Amp_Advanced::set_master_vol(int value)
{
    ui->dial->setValue(value);
}

void Amp_Advanced::set_gain2(int value)
{
    ui->dial_2->setValue(value);
}

void Amp_Advanced::set_presence(int value)
{
    ui->dial_3->setValue(value);
}

void Amp_Advanced::set_depth(int value)
{
    ui->dial_4->setValue(value);
}

void Amp_Advanced::set_threshold(int value)
{
    ui->dial_5->setValue(value);
}

void Amp_Advanced::set_bias(int value)
{
    ui->dial_6->setValue(value);
}

void Amp_Advanced::set_sag(int value)
{
    ui->dial_7->setValue(value);
}

void Amp_Advanced::set_usb_gain(int value)
{
    ui->dial_8->setValue(value);
}

void Amp_Advanced::set_brightness(bool value)
{
    ui->checkBox->setChecked(value);
}
