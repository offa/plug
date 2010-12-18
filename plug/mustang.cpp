#include "mustang.h"

#include <stdio.h>

Mustang::Mustang()
{
    FXEXEC[0] = 0x1c;
    FXEXEC[1] = 0x03;
    for(int i = 2; i < LENGTH; i++)
    {
        FXEXEC[i] = 0x00;
    }
}

Mustang::~Mustang()
{
    this->stop_amp();
}

int Mustang::start_amp()
{
    int ret;

    // initialize libusb
    ret = libusb_init(NULL);
    if (ret)
    {
        return ret;
    }

    // get handle for the device
    amp_hand = libusb_open_device_with_vid_pid(NULL, USB_VID, USB_PID);
    if(amp_hand == NULL)
    {
        stop_amp();
        return -100;
    }

    // detach kernel driver
    ret = libusb_kernel_driver_active(amp_hand, 0);
    if(ret)
    {
        ret = libusb_detach_kernel_driver(amp_hand, 0);
        if(ret)
        {
            return ret;
        }
    }

    // claim the device
    ret = libusb_claim_interface(amp_hand, 0);
    if(ret)
    {
        stop_amp();
        return ret;
    }

    return 0;
}

int Mustang::stop_amp()
{
    int ret;

    if(amp_hand != NULL)
    {
        // release claimed interface
        ret = libusb_release_interface(amp_hand, 0);
        if(ret)
        {
            return ret;
        }

        // re-attach kernel driver
        ret = libusb_attach_kernel_driver(amp_hand, 0);

        // close opened interface
        libusb_close(amp_hand);
        amp_hand = NULL;
        printf("amp stopped\n");
    }

    // stop using libusb
    libusb_exit(NULL);

    return 0;
}

int Mustang::set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp, unsigned char knob1,
                        unsigned char knob2, unsigned char knob3, unsigned char knob4, unsigned char knob5)
{
    // TODO: add rest of the effects

    int ret, recieved;
    unsigned char array[64] = {
      0x1c, 0x03, 0x09, 0x00, 0x00, 0x00, 0x01, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x24, 0x00, 0x01, 0x00, 0x08, 0x01, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x6e, 0x5d, 0x6e, 0x80, 0x91, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    if(put_post_amp)
    {
        fx_slot += 4;
    }
    array[FXSLOT] = fx_slot;
    array[KNOB1] = knob1;
    array[KNOB2] = knob2;
    array[KNOB3] = knob3;
    array[KNOB4] = knob4;
    array[KNOB5] = knob5;

    switch (effect) {
    case OVERDRIVE:
        array[DSP] = 0x06;
        array[EFFECT] = 0x3c;
        break;

    case REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x24;
        break;
    }

//    ret = libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);
//    ret = libusb_interrupt_transfer(amp_hand, 0x01, FXEXEC, LENGTH, &recieved, TMOUT);

    FILE *f;
    static char trynum=0;
    char mes[16];
    sprintf(mes, "test%d.bin",trynum);
    f=fopen(mes,"w");
    fwrite(array, sizeof(array), 1, f);
    fclose(f);
    trynum++;

    return 0;
}
