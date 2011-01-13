#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <QShortcut>
#include <QMainWindow>
#include "effect.h"
#include "amplifier.h"
#include "mustang.h"
#include "data_structs.h"
#include "about.h"
#include "saveonamp.h"
#include "loadfromamp.h"

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
    void start_amp(void);    // initialize the communication
    void stop_amp(void);    // terminate communication
    int set_effect(struct fx_pedal_settings);
    int set_amplifier(struct amp_settings);
    int save_on_amp(char *, int);
    int load_from_amp(int);
    void enable_buttons(void);

private:
    Ui::MainWindow *ui;

    // pointers to objects
    Mustang *amp_ops;
    Amplifier *amp;
    Effect *effect1;
    Effect *effect2;
    Effect *effect3;
    Effect *effect4;
    About *about_window;
    SaveOnAmp *save;
    LoadFromAmp *load;
};

#endif // MAINWINDOW_H
