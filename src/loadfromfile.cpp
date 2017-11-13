#include "loadfromfile.h"

LoadFromFile::LoadFromFile(QFile* file, QString* name, amp_settings* amp_settings, fx_pedal_settings fx_settings[4])
{
    // TODO: Use init list
    this->m_name = name;
    this->m_amp_settings = amp_settings;
    this->m_fx_settings = fx_settings;

    m_xml = std::make_unique<QXmlStreamReader>(file);
}

void LoadFromFile::loadfile()
{
    while(!m_xml->atEnd())
    {
        if(m_xml->isStartElement())
        {
            if(m_xml->name().toString() == "Amplifier")
                parseAmp();
            else if(m_xml->name().toString() == "FX")
                parseFX();
            else if(m_xml->name().toString() == "FUSE")
                parseFUSE();
            else if(m_xml->name().toString() == "UsbGain")
            {
                m_amp_settings->usb_gain = m_xml->readElementText().toInt();
            }
        }
        m_xml->readNextStartElement();
    }
}

void LoadFromFile::parseAmp()
{
    m_xml->readNextStartElement();
    while(m_xml->name() != "Amplifier")
    {
        if(m_xml->isStartElement())
        {
            if(m_xml->name() == "Module")
            {
                switch(m_xml->attributes().value("ID").toString().toInt())
                {
                case 0x67:
                    m_amp_settings->amp_num = FENDER_57_DELUXE;
                    break;

                case 0x64:
                    m_amp_settings->amp_num = FENDER_59_BASSMAN;
                    break;

                case 0x7c:
                    m_amp_settings->amp_num = FENDER_57_CHAMP;
                    break;

                case 0x53:
                    m_amp_settings->amp_num = FENDER_65_DELUXE_REVERB;
                    break;

                case 0x6a:
                    m_amp_settings->amp_num = FENDER_65_PRINCETON;
                    break;

                case 0x75:
                    m_amp_settings->amp_num = FENDER_65_TWIN_REVERB;
                    break;

                case 0x72:
                    m_amp_settings->amp_num = FENDER_SUPER_SONIC;
                    break;

                case 0x61:
                    m_amp_settings->amp_num = BRITISH_60S;
                    break;

                case 0x79:
                    m_amp_settings->amp_num = BRITISH_70S;
                    break;

                case 0x5e:
                    m_amp_settings->amp_num = BRITISH_80S;
                    break;

                case 0x5d:
                    m_amp_settings->amp_num = AMERICAN_90S;
                    break;

                case 0x6d:
                    m_amp_settings->amp_num = METAL_2000;
                    break;
                }
            }
            else if(m_xml->name() == "Param")
            {
                int i=0;
                switch(m_xml->attributes().value("ControlIndex").toString().toInt())
                {
                case 0:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->volume = i;
                    break;
                case 1:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->gain = i;
                    break;
                case 2:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->gain2 = i;
                    break;
                case 3:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->master_vol = i;
                    break;
                case 4:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->treble = i;
                    break;
                case 5:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->middle = i;
                    break;
                case 6:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->bass = i;
                    break;
                case 7:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->presence = i;
                    break;
                case 9:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->depth = i;
                    break;
                case 10:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_amp_settings->bias = i;
                    break;
                case 15:
                    i = m_xml->readElementText().toInt();
                    m_amp_settings->noise_gate = i;
                    break;
                case 16:
                    i = m_xml->readElementText().toInt();
                    m_amp_settings->threshold = i;
                    break;
                case 17:
                    i = m_xml->readElementText().toInt();
                    m_amp_settings->cabinet = i;
                    break;
                case 19:
                    i = m_xml->readElementText().toInt();
                    m_amp_settings->sag = i;
                    break;
                case 20:
                    m_amp_settings->brightness = (m_xml->readElementText().toInt()?true:false);
                    break;
                }
            }
        }
        m_xml->readNext();
    }
}

void LoadFromFile::parseFX()
{
    int x = 0;
    int fx_slots[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    m_xml->readNextStartElement();
    while(m_xml->name() != "FX")
    {
        if(m_xml->isStartElement())
        {
            if(m_xml->name() == "Stompbox")
                x = 0;
            else if(m_xml->name() == "Modulation")
                x = 1;
            else if(m_xml->name() == "Delay")
                x = 2;
            else if(m_xml->name() == "Reverb")
                x = 3;
            else if(m_xml->name() == "Module")
            {
                int position = m_xml->attributes().value("POS").toString().toInt();

                if(position > 3)
                    m_fx_settings[x].put_post_amp = true;
                else
                    m_fx_settings[x].put_post_amp = false;

                fx_slots[position] = x+1;

                switch(m_xml->attributes().value("ID").toString().toInt())
                {
                case 0x00:
                    m_fx_settings[x].effect_num = EMPTY;
                    break;

                case 0x3c:
                    m_fx_settings[x].effect_num =  OVERDRIVE;
                    break;

                case 0x49:
                    m_fx_settings[x].effect_num = WAH;
                    break;

                case 0x4a:
                    m_fx_settings[x].effect_num = TOUCH_WAH;
                    break;

                case 0x1a:
                    m_fx_settings[x].effect_num = FUZZ;
                    break;

                case 0x1c:
                    m_fx_settings[x].effect_num = FUZZ_TOUCH_WAH;
                    break;

                case 0x88:
                    m_fx_settings[x].effect_num = SIMPLE_COMP;
                    break;

                case 0x07:
                    m_fx_settings[x].effect_num = COMPRESSOR;
                    break;

                case 0x12:
                    m_fx_settings[x].effect_num = SINE_CHORUS;
                    break;

                case 0x13:
                    m_fx_settings[x].effect_num = TRIANGLE_CHORUS;
                    break;

                case 0x18:
                    m_fx_settings[x].effect_num = SINE_FLANGER;
                    break;

                case 0x19:
                    m_fx_settings[x].effect_num = TRIANGLE_FLANGER;
                    break;

                case 0x2d:
                    m_fx_settings[x].effect_num = VIBRATONE;
                    break;

                case 0x40:
                    m_fx_settings[x].effect_num = VINTAGE_TREMOLO;
                    break;

                case 0x41:
                    m_fx_settings[x].effect_num = SINE_TREMOLO;
                    break;

                case 0x22:
                    m_fx_settings[x].effect_num = RING_MODULATOR;
                    break;

                case 0x29:
                    m_fx_settings[x].effect_num = STEP_FILTER;
                    break;

                case 0x4f:
                    m_fx_settings[x].effect_num = PHASER;
                    break;

                case 0x1f:
                    m_fx_settings[x].effect_num = PITCH_SHIFTER;
                    break;

                case 0x16:
                    m_fx_settings[x].effect_num = MONO_DELAY;
                    break;

                case 0x43:
                    m_fx_settings[x].effect_num = MONO_ECHO_FILTER;
                    break;

                case 0x48:
                    m_fx_settings[x].effect_num = STEREO_ECHO_FILTER;
                    break;

                case 0x44:
                    m_fx_settings[x].effect_num = MULTITAP_DELAY;
                    break;

                case 0x45:
                    m_fx_settings[x].effect_num = PING_PONG_DELAY;
                    break;

                case 0x15:
                    m_fx_settings[x].effect_num = DUCKING_DELAY;
                    break;

                case 0x46:
                    m_fx_settings[x].effect_num = REVERSE_DELAY;
                    break;

                case 0x2b:
                    m_fx_settings[x].effect_num = TAPE_DELAY;
                    break;

                case 0x2a:
                    m_fx_settings[x].effect_num = STEREO_TAPE_DELAY;
                    break;

                case 0x24:
                    m_fx_settings[x].effect_num = SMALL_HALL_REVERB;
                    break;

                case 0x3a:
                    m_fx_settings[x].effect_num = LARGE_HALL_REVERB;
                    break;

                case 0x26:
                    m_fx_settings[x].effect_num = SMALL_ROOM_REVERB;
                    break;

                case 0x3b:
                    m_fx_settings[x].effect_num = LARGE_ROOM_REVERB;
                    break;

                case 0x4e:
                    m_fx_settings[x].effect_num = SMALL_PLATE_REVERB;
                    break;

                case 0x4b:
                    m_fx_settings[x].effect_num = LARGE_PLATE_REVERB;
                    break;

                case 0x4c:
                    m_fx_settings[x].effect_num = AMBIENT_REVERB;
                    break;

                case 0x4d:
                    m_fx_settings[x].effect_num = ARENA_REVERB;
                    break;

                case 0x21:
                    m_fx_settings[x].effect_num = FENDER_63_SPRING_REVERB;
                    break;

                case 0x0b:
                    m_fx_settings[x].effect_num = FENDER_65_SPRING_REVERB;
                    break;
                }
            }
            else if(m_xml->name() == "Param")
            {
                int i=0;
                switch(m_xml->attributes().value("ControlIndex").toString().toInt())
                {
                case 0:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_fx_settings[x].knob1 = i;
                    break;
                case 1:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_fx_settings[x].knob2 = i;
                    break;
                case 2:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_fx_settings[x].knob3 = i;
                    break;
                case 3:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_fx_settings[x].knob4 = i;
                    break;
                case 4:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_fx_settings[x].knob5 = i;
                    break;
                case 5:
                    i = m_xml->readElementText().toInt() >> 8;
                    m_fx_settings[x].knob6 = i;
                    break;
                }
            }
        }
        m_xml->readNext();
    }

    for(int i = 0, j = 0; i < 8; i++)
    {
        if(fx_slots[i])
        {
            m_fx_settings[fx_slots[i]-1].fx_slot = j;
            ++j;
        }
    }
}

void LoadFromFile::parseFUSE()
{
    m_xml->readNextStartElement();
    while(!m_xml->isEndElement())
    {
        if(m_xml->name() == "Info")
            m_name->operator =(m_xml->attributes().value("name").toString());
        m_xml->readNext();
    }
}
