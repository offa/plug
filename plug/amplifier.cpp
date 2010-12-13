#include "amplifier.h"
#include "ui_amplifier.h"

Amplifier::Amplifier(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Amplifier)
{
    ui->setupUi(this);
}

Amplifier::~Amplifier()
{
    delete ui;
}
