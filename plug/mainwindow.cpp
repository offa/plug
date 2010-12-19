#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->Amplifier->setDisabled(true);
    ui->EffectButton1->setDisabled(true);
    ui->EffectButton2->setDisabled(true);
    ui->EffectButton3->setDisabled(true);
    ui->EffectButton4->setDisabled(true);

    amp_ops = new Mustang();
    amp = new Amplifier(this);
    effect1 = new Effect(this, 0);
    effect2 = new Effect(this, 1);
    effect3 = new Effect(this, 2);
    effect4 = new Effect(this, 3);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->Amplifier, SIGNAL(clicked()), amp, SLOT(show()));
    connect(ui->EffectButton1, SIGNAL(clicked()), effect1, SLOT(show()));
    connect(ui->EffectButton2, SIGNAL(clicked()), effect2, SLOT(show()));
    connect(ui->EffectButton3, SIGNAL(clicked()), effect3, SLOT(show()));
    connect(ui->EffectButton4, SIGNAL(clicked()), effect4, SLOT(show()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(start_amp()));
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
    {
        ui->EffectButton1->setDisabled(false);
        ui->EffectButton2->setDisabled(false);
        ui->EffectButton3->setDisabled(false);
        ui->EffectButton4->setDisabled(false);
        ui->statusBar->showMessage(tr("Connected"), 5000);
    }
    else
    {
        char aaa[16];
        sprintf(aaa, "Error: %d", x);
        ui->statusBar->showMessage(aaa, 5000);
    }
}

int MainWindow::set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp,
                           unsigned char knob1, unsigned char knob2, unsigned char knob3,
                           unsigned char knob4, unsigned char knob5, unsigned char knob6)
{
    int ret;
    ret = amp_ops->set_effect(effect, fx_slot, put_post_amp, knob1, knob2, knob3, knob4, knob5, knob6);
    return ret;
}
