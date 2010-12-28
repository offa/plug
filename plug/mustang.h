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

// array fields
#define DSP 2
#define EFFECT 16
#define FXSLOT 18
#define KNOB1 32
#define KNOB2 33
#define KNOB3 34
#define KNOB4 35
#define KNOB5 36
#define KNOB6 37

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
    int set_amplifier(unsigned char amplifier, unsigned char knob1, unsigned char knob2, unsigned char knob3,
                      unsigned char knob4, unsigned char knob5, unsigned char knob6);

private:
    libusb_device_handle *amp_hand;    // handle for USB communication
    unsigned char FXEXEC[LENGTH];    // "apply effect" command for the amp
    unsigned char prev_array[4][LENGTH];    // array used to clear the effect

};

#endif // MUSTANG_H
