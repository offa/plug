#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "effect.h"
#include "amplifier.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Amplifier *wzmak;
    Effect *efekt1;
    Effect *efekt2;
    Effect *efekt3;
    Effect *efekt4;
};

#endif // MAINWINDOW_H
