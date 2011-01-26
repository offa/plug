#include "saveonamp.h"
#include "ui_saveonamp.h"

#include "mainwindow.h"

SaveOnAmp::SaveOnAmp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveOnAmp)
{
    ui->setupUi(this);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
}

SaveOnAmp::~SaveOnAmp()
{
    delete ui;
}

void SaveOnAmp::save()
{
    ((MainWindow*)parent())->save_on_amp(ui->lineEdit->text().toAscii().data(), ui->comboBox->currentIndex());
    this->close();
}
