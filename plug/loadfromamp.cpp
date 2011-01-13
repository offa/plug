#include "loadfromamp.h"
#include "ui_loadfromamp.h"

#include "mainwindow.h"

LoadFromAmp::LoadFromAmp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadFromAmp)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(load()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
}

LoadFromAmp::~LoadFromAmp()
{
    delete ui;
}

void LoadFromAmp::load()
{
    ((MainWindow*)parent())->load_from_amp(ui->comboBox->currentIndex());
    this->close();
}
