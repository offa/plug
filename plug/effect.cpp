#include "effect.h"
#include "ui_effect.h"

Effect::Effect(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Effect)
{
    ui->setupUi(this);
}

Effect::~Effect()
{
    delete ui;
}
