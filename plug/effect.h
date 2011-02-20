#ifndef EFFECT_H
#define EFFECT_H

#include <QMainWindow>
#include <QSettings>
#include "effects_enum.h"
#include "data_structs.h"

namespace Ui {
    class Effect;
}

class Effect : public QMainWindow
{
    Q_OBJECT

public:
    explicit Effect(QWidget *parent = 0, int number = 0);
    ~Effect();

private:
    Ui::Effect *ui;
    char title[10];
    unsigned char fx_slot, effect_num, knob1, knob2, knob3, knob4, knob5, knob6;
    bool put_post_amp;

public slots:
    // functions to set variables
    void set_post_amp(bool);
    void set_knob1(int);
    void set_knob2(int);
    void set_knob3(int);
    void set_knob4(int);
    void set_knob5(int);
    void set_knob6(int);
    void choose_fx(int);
    void off_switch(bool);

    // send settings to the amplifier
    void send_fx(void);

    void load(struct fx_pedal_settings);
    void get_settings(struct fx_pedal_settings &);
};

#endif // EFFECT_H
