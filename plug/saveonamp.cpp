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
    char slot_names[24][5] = {
        "[O1]", "[O2]", "[O3]", "[O4]", "[O5]", "[O6]", "[O7]", "[O8]",
        "[G1]", "[G2]", "[G3]", "[G4]", "[G5]", "[G6]", "[G7]", "[G8]",
        "[R1]", "[R2]", "[R3]", "[R4]", "[R5]", "[R6]", "[R7]", "[R8]"
    };
    QString name(QString("%1 %2").arg(slot_names[ui->comboBox->currentIndex()]).arg(ui->lineEdit->text()));

    ui->comboBox->setItemText(ui->comboBox->currentIndex(), name);
    ((MainWindow*)parent())->change_name(ui->comboBox->currentIndex(), &name);
    ((MainWindow*)parent())->save_on_amp(ui->lineEdit->text().toAscii().data(), ui->comboBox->currentIndex());
    this->close();
}

void SaveOnAmp::load_names(char names[24][32])
{
    char slot_names[24][5] = {
        "[O1]", "[O2]", "[O3]", "[O4]", "[O5]", "[O6]", "[O7]", "[O8]",
        "[G1]", "[G2]", "[G3]", "[G4]", "[G5]", "[G6]", "[G7]", "[G8]",
        "[R1]", "[R2]", "[R3]", "[R4]", "[R5]", "[R6]", "[R7]", "[R8]"
    };

    for(int i = 0; i < 24; i++)
        ui->comboBox->setItemText(i, QString("%1 %2").arg(slot_names[i]).arg(names[i]));
}
