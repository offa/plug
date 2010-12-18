#ifndef MUSTANG_H
#define MUSTANG_H

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

class Mustang
{
public:
    Mustang();
    ~Mustang();
    int start_amp();
    int stop_amp();
    int set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp, unsigned char knob1,
                   unsigned char knob2, unsigned char knob3, unsigned char knob4, unsigned char knob5);

private:
    libusb_device_handle *amp_hand;
    unsigned char FXEXEC[LENGTH];

    enum effects {
        NONE,
        OVERDRIVE,
        REVERB
    };
};

#endif // MUSTANG_H
