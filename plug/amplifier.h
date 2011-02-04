#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include <QMainWindow>
#include <QSettings>
#include "amp_advanced.h"
#include "effects_enum.h"
#include "data_structs.h"

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
    Amp_Advanced *advanced;
    unsigned char amp_num, gain, volume, treble, middle, bass;
    unsigned char cabinet, noise_gate, presence, gain2, master_vol, threshold, depth, bias, sag;

public slots:
    // set basic variables
    void set_gain(int);
    void set_volume(int);
    void set_treble(int);
    void set_middle(int);
    void set_bass(int);
    void choose_amp(int);

    // set advanced variables
    void set_cabinet(int);
    void set_noise_gate(int);
    void set_presence(int);
    void set_gain2(int);
    void set_master_vol(int);
    void set_threshold(int);
    void set_depth(int);
    void set_bias(int);
    void set_sag(int);

    // send settings to the amplifier
    void send_amp(void);

    void load(struct amp_settings);
};

#endif // AMPLIFIER_H
