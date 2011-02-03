#ifndef MUSTANG_H
#define MUSTANG_H

#include <stdio.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <QtDebug>
#include "effects_enum.h"
#include "data_structs.h"

// amp's VID and PID
#define USB_VID 0x1ed8
#define USB_PID 0x0004

// for USB communication
#define TMOUT 500
#define LENGTH 64

// effect array fields
#define DSP 2
#define EFFECT 16
#define FXSLOT 18
#define KNOB1 32
#define KNOB2 33
#define KNOB3 34
#define KNOB4 35
#define KNOB5 36
#define KNOB6 37

// amp array fields
#define AMPLIFIER 16
#define VOLUME 32
#define GAIN 33
#define TREBLE 36
#define MIDDLE 37
#define BASS 38
#define CABINET 49
#define NOISE_GATE 47
#define THRESHOLD 48
#define MASTER_VOL 35
#define GAIN2 34
#define PRESENCE 39
#define DEPTH 41
#define BIAS 42
#define SAG 51

// save fields
#define SAVE_SLOT 4

class Mustang
{
public:
    Mustang();
    ~Mustang();
    int start_amp(char [][32], char *, struct amp_settings *, struct fx_pedal_settings *);    // initialize communication
    int stop_amp(void);    // terminate communication
    int set_effect(struct fx_pedal_settings);
    int set_amplifier(struct amp_settings);
    int save_on_amp(char *, int);
    int load_memory_bank(int, char *, struct amp_settings *, struct fx_pedal_settings *);

private:
    libusb_device_handle *amp_hand;    // handle for USB communication
    unsigned char execute[LENGTH];    // "apply" command sent after each instruction
    unsigned char prev_array[4][LENGTH];    // array used to clear the effect

    int decode_data(unsigned char [6][LENGTH], char *, struct amp_settings *, struct fx_pedal_settings *t);
};

#endif // MUSTANG_H
