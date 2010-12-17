#ifndef MUSTANG_H
#define MUSTANG_H

#include <libusb-1.0/libusb.h>

#define USB_VID 0x1ed8
#define USB_PID 0x0004
#define TMOUT 500
#define LENGTH 64

class Mustang
{
public:
    Mustang();
    ~Mustang();
    int start_amp();
    int stop_amp();

    void set_overdrive();
    void set_reverb();

private:
    libusb_device_handle *amp_hand;
    unsigned char FXEXEC[LENGTH];
};

#endif // MUSTANG_H
