#include "mustang.h"

#include <stdio.h>
#include <QtDebug>

Mustang::Mustang()
{
    // "apply efect" command
    FXEXEC[0] = 0x1c;
    FXEXEC[1] = 0x03;
    for(int i = 2; i < LENGTH; i++)
    {
        FXEXEC[i] = 0x00;
    }

    prev_array[0][0] = 0x00;
    prev_array[1][0] = 0x00;
    prev_array[2][0] = 0x00;
    prev_array[3][0] = 0x00;
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
        //printf("amp stopped\n");
    }

    // stop using libusb
    libusb_exit(NULL);

    return 0;
}

int Mustang::set_effect(unsigned char effect, unsigned char fx_slot, bool put_post_amp,
                        unsigned char knob1, unsigned char knob2, unsigned char knob3,
                        unsigned char knob4, unsigned char knob5, unsigned char knob6)
{
    int ret, recieved;    // variables used when sending
    unsigned char set_slot=fx_slot;    // where to put the effect
    unsigned char array[64] = {    // empty data form
      0x1c, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    if(put_post_amp)    // put effect in a slot after amplifier
    {
        set_slot = fx_slot + 4;
    }

    // fill the form with data
    array[FXSLOT] = set_slot;
    array[KNOB1] = knob1;
    array[KNOB2] = knob2;
    array[KNOB3] = knob3;
    array[KNOB4] = knob4;
    array[KNOB5] = knob5;
    // some effects have more knobs
    if (effect == MONO_ECHO_FILTER || effect == STEREO_ECHO_FILTER || effect == TAPE_DELAY || effect == STEREO_TAPE_DELAY)
    {
        array[KNOB6] = knob6;
    }

    // fill the form with missing data
    int k=0;
    switch (effect) {
    case EMPTY:
        for (int i = 0; i < 4; i++)
        {
            if (prev_array[i][FXSLOT] == set_slot)
            {
                for (int j = 0; j < LENGTH; j++)
                {
                    array[j] = prev_array[i][j];
                }
                k++;
                break;
            }
        }
        if (k == 0)
        {
            return 0;
        }
        array[EFFECT] = 0x00;
        array[KNOB1] = 0x00;
        array[KNOB2] = 0x00;
        array[KNOB3] = 0x00;
        array[KNOB4] = 0x00;
        array[KNOB5] = 0x00;
        array[KNOB6] = 0x00;
        break;

    case OVERDRIVE:
        array[DSP] = 0x06;
        array[EFFECT] = 0x3c;
        break;

    case FIXED_WAH:
        array[DSP] = 0x06;
        array[EFFECT] = 0x49;
        array[19] = 0x01;
        break;

    case TOUCH_WAH:
        array[DSP] = 0x06;
        array[EFFECT] = 0x4a;
        array[19] = 0x01;
        break;

    case FUZZ:
        array[DSP] = 0x06;
        array[EFFECT] = 0x1a;
        break;

    case FUZZ_TOUCH_WAH:
        array[DSP] = 0x06;
        array[EFFECT] = 0x1c;
        break;

    case SIMPLE_COMP:
        array[DSP] = 0x06;
        array[EFFECT] = 0x88;
        array[19] = 0x08;
        if(array[KNOB1] > 0x03)
        {
            array[KNOB1] = 0x03;
        }
        array[KNOB2] = 0x00;
        array[KNOB3] = 0x00;
        array[KNOB4] = 0x00;
        array[KNOB5] = 0x00;
        break;

    case COMPRESSOR:
        array[DSP] = 0x06;
        array[EFFECT] = 0x07;
        break;

    case SINE_CHORUS:
        array[DSP] = 0x07;
        array[EFFECT] = 0x12;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case TRIANGLE_CHORUS:
        array[DSP] = 0x07;
        array[EFFECT] = 0x13;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case SINE_FLANGER:
        array[DSP] = 0x07;
        array[EFFECT] = 0x18;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case TRIANGLE_FLANGER:
        array[DSP] = 0x07;
        array[EFFECT] = 0x19;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case VIBRATONE:
        array[DSP] = 0x07;
        array[EFFECT] = 0x2d;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case VINTAGE_TREMOLO:
        array[DSP] = 0x07;
        array[EFFECT] = 0x40;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case SINE_TREMOLO:
        array[DSP] = 0x07;
        array[EFFECT] = 0x41;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case RING_MODULATOR:
        array[DSP] = 0x07;
        array[EFFECT] = 0x22;
        array[19] = 0x01;
        if(array[KNOB4] > 0x01)
        {
            array[KNOB4] = 0x01;
        }
        break;

    case STEP_FILTER:
        array[DSP] = 0x07;
        array[EFFECT] = 0x29;
        array[19] = 0x01;
        array[20] = 0x01;
        break;

    case PHASER:
        array[DSP] = 0x07;
        array[EFFECT] = 0x4f;
        array[19] = 0x01;
        array[20] = 0x01;
        if(array[KNOB5] > 0x01)
        {
            array[KNOB5] = 0x01;
        }
        break;

    case PITCH_SHIFTER:
        array[DSP] = 0x07;
        array[EFFECT] = 0x1f;
        array[19] = 0x01;
        break;

    case MONO_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x16;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case MONO_ECHO_FILTER:
        array[DSP] = 0x08;
        array[EFFECT] = 0x43;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case STEREO_ECHO_FILTER:
        array[DSP] = 0x08;
        array[EFFECT] = 0x48;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case MULTITAP_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x44;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case PING_PONG_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x45;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case DUCKING_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x15;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case REVERSE_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x46;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case TAPE_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x2b;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case STEREO_TAPE_DELAY:
        array[DSP] = 0x08;
        array[EFFECT] = 0x2a;
        array[19] = 0x02;
        array[20] = 0x01;
        break;

    case SMALL_HALL_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x24;
        break;

    case LARGE_HALL_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x3a;
        break;

    case SMALL_ROOM_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x26;
        break;

    case LARGE_ROOM_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x3b;
        break;

    case SMALL_PLATE_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x4e;
        break;

    case LARGE_PLATE_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x4b;
        break;

    case AMBIENT_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x4c;
        break;

    case ARENA_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x4d;
        break;

    case FENDER_63_SPRING_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x21;
        break;

    case FENDER_65_SPRING_REVERB:
        array[DSP] = 0x09;
        array[EFFECT] = 0x0b;
        break;

    }

    // clear DSP if something was there
//    if(prev_array[array[DSP]-6][0]!=0x00)
//    {
//        unsigned char clear_array[LENGTH];
//        for (int i=0; i<LENGTH;i++)
//        {
//            clear_array[i]=prev_array[array[DSP]-6][i];
//        }
//        clear_array[EFFECT] = 0x00;
//        clear_array[KNOB1] = 0x00;
//        clear_array[KNOB2] = 0x00;
//        clear_array[KNOB3] = 0x00;
//        clear_array[KNOB4] = 0x00;
//        clear_array[KNOB5] = 0x00;
//        clear_array[KNOB6] = 0x00;
//        ret = libusb_interrupt_transfer(amp_hand, 0x01, clear_array, LENGTH, &recieved, TMOUT);
//        ret = libusb_interrupt_transfer(amp_hand, 0x01, FXEXEC, LENGTH, &recieved, TMOUT);
//        //DEBUG
//        qDebug()<<"clear: DSP: "<<clear_array[DSP]<<", slot: "<<clear_array[FXSLOT]<<", effect: "<<clear_array[EFFECT];
//    }

    // send packet to the amp
    ret = libusb_interrupt_transfer(amp_hand, 0x01, array, LENGTH, &recieved, TMOUT);
    ret = libusb_interrupt_transfer(amp_hand, 0x01, FXEXEC, LENGTH, &recieved, TMOUT);
    //DEBUG
//    qDebug()<<"set: DSP: "<<array[DSP]<<", slot: "<<array[FXSLOT]<<", effect: "<<array[EFFECT];

    // save current settings
    for (int i = 0; i < LENGTH; i++)
    {
        prev_array[array[DSP]-6][i] = array[i];
    }


    //DEBUG
//    FILE *f;
//    static char trynum=0;
//    char mes[16];
//    sprintf(mes, "test%d.bin",trynum);
//    f=fopen(mes,"w");
//    fwrite(array, sizeof(array), 1, f);
//    //fwrite(FXEXEC, sizeof(FXEXEC), 1, f);
//    fclose(f);
//    trynum++;


    return ret;
}
