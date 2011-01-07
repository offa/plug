#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShortcut>

#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // create child objects
    amp_ops = new Mustang();
    amp = new Amplifier(this);
    effect1 = new Effect(this, 0);
    effect2 = new Effect(this, 1);
    effect3 = new Effect(this, 2);
    effect4 = new Effect(this, 3);

    // connect buttons to slots
    connect(ui->Amplifier, SIGNAL(clicked()), amp, SLOT(show()));
    connect(ui->EffectButton1, SIGNAL(clicked()), effect1, SLOT(show()));
    connect(ui->EffectButton2, SIGNAL(clicked()), effect2, SLOT(show()));
    connect(ui->EffectButton3, SIGNAL(clicked()), effect3, SLOT(show()));
    connect(ui->EffectButton4, SIGNAL(clicked()), effect4, SLOT(show()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(start_amp()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(stop_amp()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    // shortcut to activate buttons while debuging
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(enable_buttons()));
}

MainWindow::~MainWindow()
{
    delete amp_ops;    // stop the communication before exiting
    delete ui;
}

void MainWindow::start_amp()
{
    int x;

    x = amp_ops->start_amp();    // request initialization of communication
    if(x == 0)    // if request succeded
    {
        // activate buttons
        ui->Amplifier->setDisabled(false);
        ui->EffectButton1->setDisabled(false);
        ui->EffectButton2->setDisabled(false);
        ui->EffectButton3->setDisabled(false);
        ui->EffectButton4->setDisabled(false);
        ui->statusBar->showMessage(tr("Connected"), 5000);    // show message on the status bar
    }
    else    // if request failed
    {
        // print error code
        char err[16];
        sprintf(err, "Error: %d", x);
        ui->statusBar->showMessage(err, 5000);
    }
}

void MainWindow::stop_amp()
{
    int x;

    x = amp_ops->stop_amp();
    if(x == 0)    // if request succeded
    {
        // deactivate buttons
        ui->Amplifier->setDisabled(true);
        ui->EffectButton1->setDisabled(true);
        ui->EffectButton2->setDisabled(true);
        ui->EffectButton3->setDisabled(true);
        ui->EffectButton4->setDisabled(true);
        ui->statusBar->showMessage(tr("Disconnected"), 5000);    // show message on the status bar
    }
    else    // if request failed
    {
        // print error code
        char err[16];
        sprintf(err, "Error: %d", x);
        ui->statusBar->showMessage(err, 5000);
    }

}

// pass the message to the amp
int MainWindow::set_effect(struct fx_pedal_settings pedal)
{
    int ret;
    ret = amp_ops->set_effect(pedal);
    return ret;
}

int MainWindow::set_amplifier(struct amp_settings settings)
{
    int ret;
    ret = amp_ops->set_amplifier(settings);
    return ret;
}

// activate buttons
void MainWindow::enable_buttons(void)
{
    ui->Amplifier->setDisabled(false);
    ui->EffectButton1->setDisabled(false);
    ui->EffectButton2->setDisabled(false);
    ui->EffectButton3->setDisabled(false);
    ui->EffectButton4->setDisabled(false);
}
