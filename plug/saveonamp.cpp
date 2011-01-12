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
    QByteArray bytename(ui->lineEdit->text().toAscii());
    ((MainWindow*)parent())->save_on_amp((char*)bytename.constData(), ui->comboBox->currentIndex());
    this->close();
}
