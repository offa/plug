#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
}

MainWindow::~MainWindow()
{
    delete ui;
}
