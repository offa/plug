#include "saveonamp.h"
#include "ui_saveonamp.h"

#include "mainwindow.h"

SaveOnAmp::SaveOnAmp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SaveOnAmp)
{
    ui->setupUi(this);

    QSettings settings;
    restoreGeometry(settings.value("Windows/saveAmpPresetWindowGeometry").toByteArray());

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
}

SaveOnAmp::~SaveOnAmp()
{
    QSettings settings;
    settings.setValue("Windows/saveAmpPresetWindowGeometry", saveGeometry());
    delete ui;
}

void SaveOnAmp::save()
{
    QSettings settings;
    QString name(QString("[%1] %2").arg(ui->comboBox->currentIndex()).arg(ui->lineEdit->text()));

    ui->comboBox->setItemText(ui->comboBox->currentIndex(), name);
    ((MainWindow*)parent())->change_name(ui->comboBox->currentIndex(), &name);
    ((MainWindow*)parent())->save_on_amp(ui->lineEdit->text().toAscii().data(), ui->comboBox->currentIndex());
    if(!settings.value("Settings/keepWindowsOpen").toBool())
        this->close();
}

void SaveOnAmp::load_names(char names[][32])
{
    for(int i = 0; i < 100; i++)
    {
        if(names[i][0] == 0x00)
            break;
        ui->comboBox->addItem(QString("[%1] %2").arg(i+1).arg(names[i]));
    }
}

void SaveOnAmp::delete_items()
{
    int j = ui->comboBox->count();
    for(int i = 0; i < j; i++)
        ui->comboBox->removeItem(0);
}

void SaveOnAmp::change_index(int value, QString name)
{
    if(value > 0)
        ui->comboBox->setCurrentIndex(value);
    ui->lineEdit->setText(name);
}
