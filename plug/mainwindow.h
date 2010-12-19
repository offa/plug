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
    int set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp,
                   unsigned char knob1, unsigned char knob2, unsigned char knob3,
                   unsigned char knob4, unsigned char knob5, unsigned char knob6);

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
