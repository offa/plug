#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include <QMainWindow>
#include "effects_enum.h"

namespace Ui {
    class Amplifier;
}

class Amplifier : public QMainWindow
{
    Q_OBJECT

public:
    explicit Amplifier(QWidget *parent = 0);
    ~Amplifier();

private:
    Ui::Amplifier *ui;
    unsigned char amp_num, knob1, knob2, knob3, knob4, knob5, knob6;

private slots:
    void set_knob1(int);
    void set_knob2(int);
    void set_knob3(int);
    void set_knob4(int);
    void set_knob5(int);
    void set_knob6(int);
    void choose_amp(int);
    void send_amp(void);
};

#endif // AMPLIFIER_H
