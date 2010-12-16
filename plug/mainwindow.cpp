#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mustang.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    piec = new Mustang();
    wzmak = new Amplifier(this);
    efekt1 = new Effect(this);
    efekt2 = new Effect(this);
    efekt3 = new Effect(this);
    efekt4 = new Effect(this);

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->Amplifier, SIGNAL(clicked()), wzmak, SLOT(show()));
    connect(ui->EffectButton1, SIGNAL(clicked()), efekt1, SLOT(show()));
    connect(ui->EffectButton2, SIGNAL(clicked()), efekt2, SLOT(show()));
    connect(ui->EffectButton3, SIGNAL(clicked()), efekt3, SLOT(show()));
    connect(ui->EffectButton4, SIGNAL(clicked()), efekt4, SLOT(show()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(start_amp()));
}

MainWindow::~MainWindow()
{
    delete piec;
    delete ui;
}

void MainWindow::start_amp()
{
    int x;
    x = piec->start_amp();
    if(x == 0)
        ui->statusBar->showMessage(tr("Connected"), 5000);
    else
        ui->statusBar->showMessage(tr("Failed to connect"), 5000);
}
