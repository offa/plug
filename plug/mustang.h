#ifndef MUSTANG_H
#define MUSTANG_H

#include "effects_enum.h"
#include <libusb-1.0/libusb.h>

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

class Mustang
{
public:
    Mustang();
    ~Mustang();
    int start_amp();    // initialize communication
    int stop_amp();    // terminate communication
    int set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp,    // send the effect to the amp
                   unsigned char knob1, unsigned char knob2, unsigned char knob3,
                   unsigned char knob4, unsigned char knob5, unsigned char knob6);
    int set_amplifier(unsigned char amplifier, unsigned char gain, unsigned char volume, unsigned char treble,
                      unsigned char middle, unsigned char bass, unsigned char cabinet, unsigned char noise_gate,
                      unsigned char master_vol, unsigned char gain2, unsigned char presence, unsigned char threshold,
                      unsigned char depth, unsigned char bias, unsigned char sag);

private:
    libusb_device_handle *amp_hand;    // handle for USB communication
    unsigned char FXEXEC[LENGTH];    // "apply effect" command for the amp
    unsigned char prev_array[4][LENGTH];    // array used to clear the effect

};

#endif // MUSTANG_H
