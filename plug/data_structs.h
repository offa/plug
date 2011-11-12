#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

struct amp_settings
{
    unsigned char amp_num;
    unsigned char gain;
    unsigned char volume;
    unsigned char treble;
    unsigned char middle;
    unsigned char bass;
    unsigned char cabinet;
    unsigned char noise_gate;
    unsigned char master_vol;
    unsigned char gain2;
    unsigned char presence;
    unsigned char threshold;
    unsigned char depth;
    unsigned char bias;
    unsigned char sag;
    bool brightness;
};

struct fx_pedal_settings
{
    unsigned char fx_slot;
    unsigned char effect_num;
    unsigned char knob1;
    unsigned char knob2;
    unsigned char knob3;
    unsigned char knob4;
    unsigned char knob5;
    unsigned char knob6;
    bool put_post_amp;
};

#endif // DATA_STRUCTS_H
