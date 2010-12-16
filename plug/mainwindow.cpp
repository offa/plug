#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mustang.h"

#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->Amplifier->setDisabled(true);

    amp_ops = new Mustang();
    amp = new Amplifier(this);
    effect1 = new Effect(this);
    effect2 = new Effect(this);
    effect3 = new Effect(this);
    effect4 = new Effect(this);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->Amplifier, SIGNAL(clicked()), amp, SLOT(show()));
    connect(ui->EffectButton1, SIGNAL(clicked()), effect1, SLOT(show()));
    connect(ui->EffectButton2, SIGNAL(clicked()), effect2, SLOT(show()));
    connect(ui->EffectButton3, SIGNAL(clicked()), effect3, SLOT(show()));
    connect(ui->EffectButton4, SIGNAL(clicked()), effect4, SLOT(show()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(start_amp()));
}

MainWindow::~MainWindow()
{
    delete amp_ops;
    delete ui;
}

void MainWindow::start_amp()
{
    int x;
    x = amp_ops->start_amp();
    if(x == 0)
        ui->statusBar->showMessage(tr("Connected"), 5000);
    else
    {
        char aaa[16];
        sprintf(aaa, "Error: %d", x);
        ui->statusBar->showMessage(aaa, 5000);
    }
}
