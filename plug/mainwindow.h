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
    void start_amp();    // initialize the communication
    void stop_amp();    // terminate communication
    int set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp,    // send effect to the amp_ops object
                   unsigned char knob1, unsigned char knob2, unsigned char knob3,
                   unsigned char knob4, unsigned char knob5, unsigned char knob6);
    int set_amplifier(unsigned char amplifier, unsigned char gain, unsigned char volume, unsigned char treble,
                      unsigned char middle, unsigned char bass, unsigned char cabinet, unsigned char noise_gate,
                      unsigned char master_vol, unsigned char gain2, unsigned char presence, unsigned char threshold,
                      unsigned char depth, unsigned char bias, unsigned char sag);

private:
    Ui::MainWindow *ui;

    // pointers to objects
    Mustang *amp_ops;
    Amplifier *amp;
    Effect *effect1;
    Effect *effect2;
    Effect *effect3;
    Effect *effect4;
};

#endif // MAINWINDOW_H
