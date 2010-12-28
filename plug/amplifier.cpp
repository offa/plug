#include "amplifier.h"
#include "ui_amplifier.h"

#include "mainwindow.h"

Amplifier::Amplifier(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Amplifier)
{
    ui->setupUi(this);

    // initialize variables
    amp_num = ui->comboBox->currentIndex();
    knob1 = 0;
    knob2 = 0;
    knob3 = 0;
    knob4 = 0;
    knob5 = 0;
    knob6 = 0;

    // disabled until I discover how it works
    ui->dial->setDisabled(true);
    ui->dial_2->setDisabled(true);
    ui->dial_3->setDisabled(true);
    ui->dial_4->setDisabled(true);
    ui->dial_5->setDisabled(true);
    ui->dial_6->setDisabled(true);
    ui->spinBox->setDisabled(true);
    ui->spinBox_2->setDisabled(true);
    ui->spinBox_3->setDisabled(true);
    ui->spinBox_4->setDisabled(true);
    ui->spinBox_5->setDisabled(true);
    ui->spinBox_6->setDisabled(true);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(choose_amp(int)));
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(set_knob1(int)));
    connect(ui->dial_2, SIGNAL(valueChanged(int)), this, SLOT(set_knob2(int)));
    connect(ui->dial_3, SIGNAL(valueChanged(int)), this, SLOT(set_knob3(int)));
    connect(ui->dial_4, SIGNAL(valueChanged(int)), this, SLOT(set_knob4(int)));
    connect(ui->dial_5, SIGNAL(valueChanged(int)), this, SLOT(set_knob5(int)));
    connect(ui->dial_6, SIGNAL(valueChanged(int)), this, SLOT(set_knob6(int)));
    connect(ui->setButton, SIGNAL(clicked()), this, SLOT(send_amp()));
}

Amplifier::~Amplifier()
{
    delete ui;
}

void Amplifier::set_knob1(int value)
{
    knob1 = value;
}

void Amplifier::set_knob2(int value)
{
    knob2 = value;
}

void Amplifier::set_knob3(int value)
{
    knob3 = value;
}

void Amplifier::set_knob4(int value)
{
    knob4 = value;
}

void Amplifier::set_knob5(int value)
{
    knob5 = value;
}

void Amplifier::set_knob6(int value)
{
    knob6 = value;
}

void Amplifier::choose_amp(int value)
{
    amp_num = value;

    // activate knobs
    switch (value)
    {
    case FENDER_57_DELUXE:
        break;
    case FENDER_59_BASSMAN:
        break;
    case FENDER_57_CHAMP:
        break;
    case FENDER_65_DELUXE_REVERB:
        break;
    case FENDER_65_PRINCETON:
        break;
    case FENDER_65_TWIN_REVERB:
        break;
    case FENDER_SUPER_SONIC:
        break;
    case BRITISH_60S:
        break;
    case BRITISH_70S:
        break;
    case BRITISH_80S:
        break;
    case AMERICAN_90S:
        break;
    case METAL_2000:
        break;
    default:
        break;
    }

    // set labels
    switch (value)
    {
    case FENDER_57_DELUXE:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case FENDER_59_BASSMAN:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case FENDER_57_CHAMP:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));

        break;

    case FENDER_65_DELUXE_REVERB:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case FENDER_65_PRINCETON:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case FENDER_65_TWIN_REVERB:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case FENDER_SUPER_SONIC:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case BRITISH_60S:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case BRITISH_70S:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case BRITISH_80S:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case AMERICAN_90S:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;

    case METAL_2000:
        ui->label->setText(tr(""));
        ui->label_2->setText(tr(""));
        ui->label_3->setText(tr(""));
        ui->label_4->setText(tr(""));
        ui->label_5->setText(tr(""));
        ui->label_6->setText(tr(""));
        break;
    }
}

void Amplifier::send_amp()
{
    ((MainWindow*)parent())->set_amplifier(amp_num, knob1, knob2, knob3, knob4, knob5, knob6);
}
