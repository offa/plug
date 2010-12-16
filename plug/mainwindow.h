#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "effect.h"
#include "amplifier.h"
#include "mustang.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void start_amp();

private:
    Ui::MainWindow *ui;

    Mustang *piec;
    Amplifier *wzmak;
    Effect *efekt1;
    Effect *efekt2;
    Effect *efekt3;
    Effect *efekt4;
};

#endif // MAINWINDOW_H
