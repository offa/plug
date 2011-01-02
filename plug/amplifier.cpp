#include "amplifier.h"
#include "ui_amplifier.h"

#include "mainwindow.h"

Amplifier::Amplifier(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Amplifier)
{
    ui->setupUi(this);

    // initialize variables
    gain = 0;
    volume = 0;
    treble = 0;
    middle = 0;
    bass = 0;

    cabinet = 0;
    noise_gate = 0;
    master_vol = 128;
    gain2 = 128;
    presence = 128;
    threshold = 0;
    depth = 128;
    bias = 128;
    sag = 1;

    advanced = new Amp_Advanced(this);
    connect(ui->advancedButton, SIGNAL(clicked()), advanced, SLOT(open()));
    choose_amp(0);

    // disabled until I discover how it works
//    ui->dial->setDisabled(true);
//    ui->dial_2->setDisabled(true);
//    ui->dial_3->setDisabled(true);
//    ui->dial_4->setDisabled(true);
//    ui->dial_5->setDisabled(true);
//    ui->spinBox->setDisabled(true);
//    ui->spinBox_2->setDisabled(true);
//    ui->spinBox_3->setDisabled(true);
//    ui->spinBox_4->setDisabled(true);
//    ui->spinBox_5->setDisabled(true);
//    ui->advancedButton->setDisabled(true);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_amp(int)));
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(set_gain(int)));
    connect(ui->dial_2, SIGNAL(valueChanged(int)), this, SLOT(set_volume(int)));
    connect(ui->dial_3, SIGNAL(valueChanged(int)), this, SLOT(set_treble(int)));
    connect(ui->dial_4, SIGNAL(valueChanged(int)), this, SLOT(set_middle(int)));
    connect(ui->dial_5, SIGNAL(valueChanged(int)), this, SLOT(set_bass(int)));
    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(send_amp()));
}

Amplifier::~Amplifier()
{
    delete ui;
}

void Amplifier::set_gain(int value)
{
    gain = value;
}

void Amplifier::set_volume(int value)
{
    volume = value;
}

void Amplifier::set_treble(int value)
{
    treble = value;
}

void Amplifier::set_middle(int value)
{
    middle = value;
}

void Amplifier::set_bass(int value)
{
    bass = value;
}

void Amplifier::set_cabinet(int value)
{
    cabinet = value;
}

void Amplifier::set_noise_gate(int value)
{
    noise_gate = value;
}

void Amplifier::set_presence(int value)
{
    presence = value;
}

void Amplifier::set_gain2(int value)
{
    gain2 = value;
}

void Amplifier::set_master_vol(int value)
{
    master_vol = value;
}

void Amplifier::set_threshold(int value)
{
    threshold = value;
}

void Amplifier::set_depth(int value)
{
    depth = value;
}

void Amplifier::set_bias(int value)
{
    bias = value;
}

void Amplifier::set_sag(int value)
{
    sag = value;
}

void Amplifier::choose_amp(int value)
{
    amp_num = value;

    // set properties
    switch (value)
    {
    case FENDER_57_DELUXE:
        advanced->change_cabinet(cab57DLX);
        advanced->change_noise_gate(0);
        break;

    case FENDER_59_BASSMAN:
        advanced->change_cabinet(cabBSSMN);
        advanced->change_noise_gate(0);
        break;

    case FENDER_57_CHAMP:
        advanced->change_cabinet(cabCHAMP);
        advanced->change_noise_gate(0);
        break;

    case FENDER_65_DELUXE_REVERB:
        advanced->change_cabinet(cab65DLX);
        advanced->change_noise_gate(0);
        break;

    case FENDER_65_PRINCETON:
        advanced->change_cabinet(cab65PRN);
        advanced->change_noise_gate(0);
        break;

    case FENDER_65_TWIN_REVERB:
        advanced->change_cabinet(cab65TWN);
        advanced->change_noise_gate(0);
        break;

    case FENDER_SUPER_SONIC:
        advanced->change_cabinet(cabSS112);
        advanced->change_noise_gate(2);
        break;

    case BRITISH_60S:
        advanced->change_cabinet(cab2x12C);
        advanced->change_noise_gate(0);
        break;

    case BRITISH_70S:
        advanced->change_cabinet(cab4x12G);
        advanced->change_noise_gate(1);
        break;

    case BRITISH_80S:
        advanced->change_cabinet(cab4x12M);
        advanced->change_noise_gate(1);
        break;

    case AMERICAN_90S:
        advanced->change_cabinet(cab4x12V);
        advanced->change_noise_gate(3);
        break;

    case METAL_2000:
        advanced->change_cabinet(cab4x12G);
        advanced->change_noise_gate(2);
        break;

    default:
        break;
    }
}

void Amplifier::send_amp()
{
    ((MainWindow*)parent())->set_amplifier(amp_num, gain, volume, treble, middle, bass,
                                           cabinet, noise_gate, master_vol, gain2, presence, threshold, depth, bias, sag);
}
