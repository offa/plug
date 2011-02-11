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

void LoadFromAmp::load_names(char names[24][32])
{
    char slot_names[24][5] = {
        "[O1]", "[O2]", "[O3]", "[O4]", "[O5]", "[O6]", "[O7]", "[O8]",
        "[G1]", "[G2]", "[G3]", "[G4]", "[G5]", "[G6]", "[G7]", "[G8]",
        "[R1]", "[R2]", "[R3]", "[R4]", "[R5]", "[R6]", "[R7]", "[R8]"
    };

    for(int i = 0; i < 24; i++)
        ui->comboBox->setItemText(i, QString("%1 %2").arg(slot_names[i]).arg(names[i]));
}

void LoadFromAmp::change_name(int slot, QString *name)
{
    ui->comboBox->setItemText(slot, *name);
}
