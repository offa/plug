#include "loadfromfile.h"

LoadFromFile::LoadFromFile(QFile *file, QString *name, struct amp_settings *amplifier_settings, struct fx_pedal_settings fx_settings[4])
{
    this->name = name;
    this->amplifier_settings = amplifier_settings;
    this->fx_settings = fx_settings;

    xml = new QXmlStreamReader(file);
}

LoadFromFile::~LoadFromFile()
{
    delete xml;
}

void LoadFromFile::loadfile()
{
    while(!xml->atEnd())
    {
        if(xml->isStartElement())
        {
            if(xml->name().toString() == "Amplifier")
                parseAmp();
            else if(xml->name().toString() == "FX")
                parseFX();
            else if(xml->name().toString() == "FUSE")
                parseFUSE();
        }
        xml->readNextStartElement();
    }
}

void LoadFromFile::parseAmp()
{
    xml->readNextStartElement();
    while(xml->name() != "Amplifier")
    {
        if(xml->isStartElement())
        {
            if(xml->name() == "Module")
            {
                switch(xml->attributes().value("ID").toString().toInt())
                {
                case 0x67:
                    amplifier_settings->amp_num = FENDER_57_DELUXE;
                    break;

                case 0x64:
                    amplifier_settings->amp_num = FENDER_59_BASSMAN;
                    break;

                case 0x7c:
                    amplifier_settings->amp_num = FENDER_57_CHAMP;
                    break;

                case 0x53:
                    amplifier_settings->amp_num = FENDER_65_DELUXE_REVERB;
                    break;

                case 0x6a:
                    amplifier_settings->amp_num = FENDER_65_PRINCETON;
                    break;

                case 0x75:
                    amplifier_settings->amp_num = FENDER_65_TWIN_REVERB;
                    break;

                case 0x72:
                    amplifier_settings->amp_num = FENDER_SUPER_SONIC;
                    break;

                case 0x61:
                    amplifier_settings->amp_num = BRITISH_60S;
                    break;

                case 0x79:
                    amplifier_settings->amp_num = BRITISH_70S;
                    break;

                case 0x5e:
                    amplifier_settings->amp_num = BRITISH_80S;
                    break;

                case 0x5d:
                    amplifier_settings->amp_num = AMERICAN_90S;
                    break;

                case 0x6d:
                    amplifier_settings->amp_num = METAL_2000;
                    break;
                }
            }
            else if(xml->name() == "Param")
            {
                int i=0;
                switch(xml->attributes().value("ControlIndex").toString().toInt())
                {
                case 0:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->volume = i;
                    break;
                case 1:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->gain = i;
                    break;
                case 2:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->gain2 = i;
                    break;
                case 3:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->master_vol = i;
                    break;
                case 4:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->treble = i;
                    break;
                case 5:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->middle = i;
                    break;
                case 6:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->bass = i;
                    break;
                case 7:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->presence = i;
                    break;
                case 9:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->depth = i;
                    break;
                case 10:
                    i = xml->readElementText().toInt() >> 8;
                    amplifier_settings->bias = i;
                    break;
                case 15:
                    i = xml->readElementText().toInt();
                    amplifier_settings->noise_gate = i;
                    break;
                case 16:
                    i = xml->readElementText().toInt();
                    amplifier_settings->threshold = i;
                    break;
                case 17:
                    i = xml->readElementText().toInt();
                    amplifier_settings->cabinet = i;
                    break;
                case 19:
                    i = xml->readElementText().toInt();
                    amplifier_settings->sag = i;
                    break;
                case 20:
                    amplifier_settings->brightness = (xml->readElementText().toInt()?true:false);
                    break;
                }
            }
        }
        xml->readNext();
    }
}

void LoadFromFile::parseFX()
{
    int x = 0;
    int fx_slots[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    xml->readNextStartElement();
    while(xml->name() != "FX")
    {
        if(xml->isStartElement())
        {
            if(xml->name() == "Stompbox")
                x = 0;
            else if(xml->name() == "Modulation")
                x = 1;
            else if(xml->name() == "Delay")
                x = 2;
            else if(xml->name() == "Reverb")
                x = 3;
            else if(xml->name() == "Module")
            {
                int position = xml->attributes().value("POS").toString().toInt();

                if(position > 3)
                    fx_settings[x].put_post_amp = true;
                else
                    fx_settings[x].put_post_amp = false;

                fx_slots[position] = x+1;

                switch(xml->attributes().value("ID").toString().toInt())
                {
                case 0x00:
                    fx_settings[x].effect_num = EMPTY;
                    break;

                case 0x3c:
                    fx_settings[x].effect_num =  OVERDRIVE;
                    break;

                case 0x49:
                    fx_settings[x].effect_num = FIXED_WAH;
                    break;

                case 0x4a:
                    fx_settings[x].effect_num = TOUCH_WAH;
                    break;

                case 0x1a:
                    fx_settings[x].effect_num = FUZZ;
                    break;

                case 0x1c:
                    fx_settings[x].effect_num = FUZZ_TOUCH_WAH;
                    break;

                case 0x88:
                    fx_settings[x].effect_num = SIMPLE_COMP;
                    break;

                case 0x07:
                    fx_settings[x].effect_num = COMPRESSOR;
                    break;

                case 0x12:
                    fx_settings[x].effect_num = SINE_CHORUS;
                    break;

                case 0x13:
                    fx_settings[x].effect_num = TRIANGLE_CHORUS;
                    break;

                case 0x18:
                    fx_settings[x].effect_num = SINE_FLANGER;
                    break;

                case 0x19:
                    fx_settings[x].effect_num = TRIANGLE_FLANGER;
                    break;

                case 0x2d:
                    fx_settings[x].effect_num = VIBRATONE;
                    break;

                case 0x40:
                    fx_settings[x].effect_num = VINTAGE_TREMOLO;
                    break;

                case 0x41:
                    fx_settings[x].effect_num = SINE_TREMOLO;
                    break;

                case 0x22:
                    fx_settings[x].effect_num = RING_MODULATOR;
                    break;

                case 0x29:
                    fx_settings[x].effect_num = STEP_FILTER;
                    break;

                case 0x4f:
                    fx_settings[x].effect_num = PHASER;
                    break;

                case 0x1f:
                    fx_settings[x].effect_num = PITCH_SHIFTER;
                    break;

                case 0x16:
                    fx_settings[x].effect_num = MONO_DELAY;
                    break;

                case 0x43:
                    fx_settings[x].effect_num = MONO_ECHO_FILTER;
                    break;

                case 0x48:
                    fx_settings[x].effect_num = STEREO_ECHO_FILTER;
                    break;

                case 0x44:
                    fx_settings[x].effect_num = MULTITAP_DELAY;
                    break;

                case 0x45:
                    fx_settings[x].effect_num = PING_PONG_DELAY;
                    break;

                case 0x15:
                    fx_settings[x].effect_num = DUCKING_DELAY;
                    break;

                case 0x46:
                    fx_settings[x].effect_num = REVERSE_DELAY;
                    break;

                case 0x2b:
                    fx_settings[x].effect_num = TAPE_DELAY;
                    break;

                case 0x2a:
                    fx_settings[x].effect_num = STEREO_TAPE_DELAY;
                    break;

                case 0x24:
                    fx_settings[x].effect_num = SMALL_HALL_REVERB;
                    break;

                case 0x3a:
                    fx_settings[x].effect_num = LARGE_HALL_REVERB;
                    break;

                case 0x26:
                    fx_settings[x].effect_num = SMALL_ROOM_REVERB;
                    break;

                case 0x3b:
                    fx_settings[x].effect_num = LARGE_ROOM_REVERB;
                    break;

                case 0x4e:
                    fx_settings[x].effect_num = SMALL_PLATE_REVERB;
                    break;

                case 0x4b:
                    fx_settings[x].effect_num = LARGE_PLATE_REVERB;
                    break;

                case 0x4c:
                    fx_settings[x].effect_num = AMBIENT_REVERB;
                    break;

                case 0x4d:
                    fx_settings[x].effect_num = ARENA_REVERB;
                    break;

                case 0x21:
                    fx_settings[x].effect_num = FENDER_63_SPRING_REVERB;
                    break;

                case 0x0b:
                    fx_settings[x].effect_num = FENDER_65_SPRING_REVERB;
                    break;
                }
            }
            else if(xml->name() == "Param")
            {
                int i=0;
                switch(xml->attributes().value("ControlIndex").toString().toInt())
                {
                case 0:
                    i = xml->readElementText().toInt() >> 8;
                    fx_settings[x].knob1 = i;
                    break;
                case 1:
                    i = xml->readElementText().toInt() >> 8;
                    fx_settings[x].knob2 = i;
                    break;
                case 2:
                    i = xml->readElementText().toInt() >> 8;
                    fx_settings[x].knob3 = i;
                    break;
                case 3:
                    i = xml->readElementText().toInt() >> 8;
                    fx_settings[x].knob4 = i;
                    break;
                case 4:
                    i = xml->readElementText().toInt() >> 8;
                    fx_settings[x].knob5 = i;
                    break;
                case 5:
                    i = xml->readElementText().toInt() >> 8;
                    fx_settings[x].knob6 = i;
                    break;
                }
            }
        }
        xml->readNext();
    }

    for(int i = 0, j = 0; i < 8; i++)
    {
        if(fx_slots[i])
        {
            fx_settings[fx_slots[i]-1].fx_slot = j;
            j++;
        }
    }
}

void LoadFromFile::parseFUSE()
{
    xml->readNextStartElement();
    while(!xml->isEndElement())
    {
        if(xml->name() == "Info")
            name->operator =(xml->attributes().value("name").toString());
        xml->readNext();
    }
}
