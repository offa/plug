#include "loadfromfile.h"

LoadFromFile::LoadFromFile()
{
}

LoadFromFile::~LoadFromFile()
{
}

void LoadFromFile::parseFile(QFile *file, QString *name, struct amp_settings *ampset, struct fx_pedal_settings fxset[4])
{
    QXmlStreamReader *xml = new QXmlStreamReader(file);

    while(!xml->atEnd())
    {
        if(xml->isStartElement())
        {
            if(xml->name().toString() == "Amplifier")
                parseAmp(xml, ampset);
            else if(xml->name().toString() == "FX")
                parseFX(xml, fxset);
            else if(xml->name().toString() == "FUSE")
                parseFUSE(xml, name);
        }
        xml->readNextStartElement();
    }
}

void LoadFromFile::parseAmp(QXmlStreamReader *xml, struct amp_settings *settings)
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
                    settings->amp_num = FENDER_57_DELUXE;
                    break;

                case 0x64:
                    settings->amp_num = FENDER_59_BASSMAN;
                    break;

                case 0x7c:
                    settings->amp_num = FENDER_57_CHAMP;
                    break;

                case 0x53:
                    settings->amp_num = FENDER_65_DELUXE_REVERB;
                    break;

                case 0x6a:
                    settings->amp_num = FENDER_65_PRINCETON;
                    break;

                case 0x75:
                    settings->amp_num = FENDER_65_TWIN_REVERB;
                    break;

                case 0x72:
                    settings->amp_num = FENDER_SUPER_SONIC;
                    break;

                case 0x61:
                    settings->amp_num = BRITISH_60S;
                    break;

                case 0x79:
                    settings->amp_num = BRITISH_70S;
                    break;

                case 0x5e:
                    settings->amp_num = BRITISH_80S;
                    break;

                case 0x5d:
                    settings->amp_num = AMERICAN_90S;
                    break;

                case 0x6d:
                    settings->amp_num = METAL_2000;
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
                    settings->volume = i;
                    break;
                case 1:
                    i = xml->readElementText().toInt() >> 8;
                    settings->gain = i;
                    break;
                case 2:
                    i = xml->readElementText().toInt() >> 8;
                    settings->gain2 = i;
                    break;
                case 3:
                    i = xml->readElementText().toInt() >> 8;
                    settings->master_vol = i;
                    break;
                case 4:
                    i = xml->readElementText().toInt() >> 8;
                    settings->treble = i;
                    break;
                case 5:
                    i = xml->readElementText().toInt() >> 8;
                    settings->middle = i;
                    break;
                case 6:
                    i = xml->readElementText().toInt() >> 8;
                    settings->bass = i;
                    break;
                case 7:
                    i = xml->readElementText().toInt() >> 8;
                    settings->presence = i;
                    break;
                case 9:
                    i = xml->readElementText().toInt() >> 8;
                    settings->depth = i;
                    break;
                case 10:
                    i = xml->readElementText().toInt() >> 8;
                    settings->bias = i;
                    break;
                case 15:
                    i = xml->readElementText().toInt();
                    settings->noise_gate = i;
                    break;
                case 16:
                    i = xml->readElementText().toInt();
                    settings->threshold = i;
                    break;
                case 17:
                    i = xml->readElementText().toInt();
                    settings->cabinet = i;
                    break;
                case 19:
                    i = xml->readElementText().toInt();
                    settings->sag = i;
                    break;
                }
            }
        }
        xml->readNext();
    }
}

void LoadFromFile::parseFX(QXmlStreamReader *xml, struct fx_pedal_settings settings[4])
{
    int x = 0;
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
                settings[x].fx_slot = x; //(xml->attributes().value("POS").toString().toInt() % 4);
                if(xml->attributes().value("POS").toString().toInt() > 3)
                    settings[x].put_post_amp = true;

                switch(xml->attributes().value("ID").toString().toInt())
                {
                case 0x00:
                    settings[x].effect_num = EMPTY;
                    break;

                case 0x3c:
                    settings[x].effect_num =  OVERDRIVE;
                    break;

                case 0x49:
                    settings[x].effect_num = FIXED_WAH;
                    break;

                case 0x4a:
                    settings[x].effect_num = TOUCH_WAH;
                    break;

                case 0x1a:
                    settings[x].effect_num = FUZZ;
                    break;

                case 0x1c:
                    settings[x].effect_num = FUZZ_TOUCH_WAH;
                    break;

                case 0x88:
                    settings[x].effect_num = SIMPLE_COMP;
                    break;

                case 0x07:
                    settings[x].effect_num = COMPRESSOR;
                    break;

                case 0x12:
                    settings[x].effect_num = SINE_CHORUS;
                    break;

                case 0x13:
                    settings[x].effect_num = TRIANGLE_CHORUS;
                    break;

                case 0x18:
                    settings[x].effect_num = SINE_FLANGER;
                    break;

                case 0x19:
                    settings[x].effect_num = TRIANGLE_FLANGER;
                    break;

                case 0x2d:
                    settings[x].effect_num = VIBRATONE;
                    break;

                case 0x40:
                    settings[x].effect_num = VINTAGE_TREMOLO;
                    break;

                case 0x41:
                    settings[x].effect_num = SINE_TREMOLO;
                    break;

                case 0x22:
                    settings[x].effect_num = RING_MODULATOR;
                    break;

                case 0x29:
                    settings[x].effect_num = STEP_FILTER;
                    break;

                case 0x4f:
                    settings[x].effect_num = PHASER;
                    break;

                case 0x1f:
                    settings[x].effect_num = PITCH_SHIFTER;
                    break;

                case 0x16:
                    settings[x].effect_num = MONO_DELAY;
                    break;

                case 0x43:
                    settings[x].effect_num = MONO_ECHO_FILTER;
                    break;

                case 0x48:
                    settings[x].effect_num = STEREO_ECHO_FILTER;
                    break;

                case 0x44:
                    settings[x].effect_num = MULTITAP_DELAY;
                    break;

                case 0x45:
                    settings[x].effect_num = PING_PONG_DELAY;
                    break;

                case 0x15:
                    settings[x].effect_num = DUCKING_DELAY;
                    break;

                case 0x46:
                    settings[x].effect_num = REVERSE_DELAY;
                    break;

                case 0x2b:
                    settings[x].effect_num = TAPE_DELAY;
                    break;

                case 0x2a:
                    settings[x].effect_num = STEREO_TAPE_DELAY;
                    break;

                case 0x24:
                    settings[x].effect_num = SMALL_HALL_REVERB;
                    break;

                case 0x3a:
                    settings[x].effect_num = LARGE_HALL_REVERB;
                    break;

                case 0x26:
                    settings[x].effect_num = SMALL_ROOM_REVERB;
                    break;

                case 0x3b:
                    settings[x].effect_num = LARGE_ROOM_REVERB;
                    break;

                case 0x4e:
                    settings[x].effect_num = SMALL_PLATE_REVERB;
                    break;

                case 0x4b:
                    settings[x].effect_num = LARGE_PLATE_REVERB;
                    break;

                case 0x4c:
                    settings[x].effect_num = AMBIENT_REVERB;
                    break;

                case 0x4d:
                    settings[x].effect_num = ARENA_REVERB;
                    break;

                case 0x21:
                    settings[x].effect_num = FENDER_63_SPRING_REVERB;
                    break;

                case 0x0b:
                    settings[x].effect_num = FENDER_65_SPRING_REVERB;
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
                    settings[x].knob1 = i;
                    break;
                case 1:
                    i = xml->readElementText().toInt() >> 8;
                    settings[x].knob2 = i;
                    break;
                case 2:
                    i = xml->readElementText().toInt() >> 8;
                    settings[x].knob3 = i;
                    break;
                case 3:
                    i = xml->readElementText().toInt() >> 8;
                    settings[x].knob4 = i;
                    break;
                case 4:
                    i = xml->readElementText().toInt() >> 8;
                    settings[x].knob5 = i;
                    break;
                case 5:
                    i = xml->readElementText().toInt() >> 8;
                    settings[x].knob6 = i;
                    break;
                }
            }
        }
        xml->readNext();
    }
}

void LoadFromFile::parseFUSE(QXmlStreamReader *xml, QString *name)
{
    xml->readNextStartElement();
    while(!xml->isEndElement())
    {
        if(xml->name() == "Info")
            name->operator =(xml->attributes().value("name").toString());
        xml->readNext();
    }
}
