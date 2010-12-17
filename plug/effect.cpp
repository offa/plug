#include "effect.h"
#include "ui_effect.h"

Effect::Effect(QWidget *parent, int number) :
    QMainWindow(parent),
    ui(new Ui::Effect)
{
    slot = number;
    ui->setupUi(this);

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(set_fx(int)));
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(post_amp(bool)));
    connect(ui->checkBox_2, SIGNAL(toggled(bool)), this, SLOT(turn_on(bool)));
    connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(set_knob1(int)));
    connect(ui->dial_2, SIGNAL(valueChanged(int)), this, SLOT(set_knob2(int)));
    connect(ui->dial_3, SIGNAL(valueChanged(int)), this, SLOT(set_knob3(int)));
    connect(ui->dial_4, SIGNAL(valueChanged(int)), this, SLOT(set_knob4(int)));
    connect(ui->dial_5, SIGNAL(valueChanged(int)), this, SLOT(set_knob5(int)));
}

Effect::~Effect()
{
    delete ui;
}

void Effect::turn_on(bool value)
{
    turned_on = value;
}

void Effect::post_amp(bool value)
{
    if(value)
    {
        slot += 4;
    }
    else
    {
        slot -= 4;
    }
}

void Effect::set_knob1(int value)
{
    knob1 = value;
}

void Effect::set_knob2(int value)
{
    knob2 = value;
}

void Effect::set_knob3(int value)
{
    knob3 = value;
}

void Effect::set_knob4(int value)
{
    knob4 = value;
}

void Effect::set_knob5(int value)
{
    knob5 = value;
}

void Effect::set_fx(int value)
{
    switch(value)
    {
    case 0:
        break;
    case 1:
        break;
    }
}
