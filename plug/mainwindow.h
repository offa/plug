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
    void set_reverb();
    void set_overdrive();

private:
    Ui::MainWindow *ui;

    Mustang *amp_ops;
    Amplifier *amp;
    Effect *effect1;
    Effect *effect2;
    Effect *effect3;
    Effect *effect4;
};

#endif // MAINWINDOW_H
