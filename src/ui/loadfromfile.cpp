/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2023  offa
 * Copyright (C) 2010-2016  piorekf <piorek@piorekf.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ui/loadfromfile.h"
#include "effects_enum.h"
#include <QXmlStreamReader>

namespace plug
{

    LoadFromFile::LoadFromFile(QFile* file, QString* name, amp_settings* amp_settings, std::vector<fx_pedal_settings>& fx_settings)
        : m_name(name),
          m_amp_settings(amp_settings),
          m_fx_settings(fx_settings),
          m_xml(std::make_unique<QXmlStreamReader>(file))
    {
    }

    void LoadFromFile::loadfile()
    {
        while (!m_xml->atEnd())
        {
            if (m_xml->isStartElement())
            {
                if (m_xml->name().toString() == "Amplifier")
                {
                    parseAmp();
                }
                else if (m_xml->name().toString() == "FX")
                {
                    parseFX();
                }
                else if (m_xml->name().toString() == "FUSE")
                {
                    parseFUSE();
                }
                else if (m_xml->name().toString() == "UsbGain")
                {
                    m_amp_settings->usb_gain = static_cast<std::uint8_t>(m_xml->readElementText().toUInt());
                }
            }
            m_xml->readNextStartElement();
        }
    }

    void LoadFromFile::parseAmp()
    {
        m_xml->readNextStartElement();
        while (m_xml->name() != "Amplifier")
        {
            if (m_xml->isStartElement())
            {
                if (m_xml->name() == "Module")
                {
                    switch (m_xml->attributes().value("ID").toString().toInt())
                    {
                        case 0x67:
                            m_amp_settings->amp_num = amps::FENDER_57_DELUXE;
                            break;

                        case 0x64:
                            m_amp_settings->amp_num = amps::FENDER_59_BASSMAN;
                            break;

                        case 0x7c:
                            m_amp_settings->amp_num = amps::FENDER_57_CHAMP;
                            break;

                        case 0x53:
                            m_amp_settings->amp_num = amps::FENDER_65_DELUXE_REVERB;
                            break;

                        case 0x6a:
                            m_amp_settings->amp_num = amps::FENDER_65_PRINCETON;
                            break;

                        case 0x75:
                            m_amp_settings->amp_num = amps::FENDER_65_TWIN_REVERB;
                            break;

                        case 0x72:
                            m_amp_settings->amp_num = amps::FENDER_SUPER_SONIC;
                            break;

                        case 0x61:
                            m_amp_settings->amp_num = amps::BRITISH_60S;
                            break;

                        case 0x79:
                            m_amp_settings->amp_num = amps::BRITISH_70S;
                            break;

                        case 0x5e:
                            m_amp_settings->amp_num = amps::BRITISH_80S;
                            break;

                        case 0x5d:
                            m_amp_settings->amp_num = amps::AMERICAN_90S;
                            break;

                        case 0x6d:
                            m_amp_settings->amp_num = amps::METAL_2000;
                            break;
                    }
                }
                else if (m_xml->name() == "Param")
                {
                    int i = 0;
                    switch (m_xml->attributes().value("ControlIndex").toString().toInt())
                    {
                        case 0:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->volume = static_cast<std::uint8_t>(i);
                            break;
                        case 1:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->gain = static_cast<std::uint8_t>(i);
                            break;
                        case 2:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->gain2 = static_cast<std::uint8_t>(i);
                            break;
                        case 3:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->master_vol = static_cast<std::uint8_t>(i);
                            break;
                        case 4:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->treble = static_cast<std::uint8_t>(i);
                            break;
                        case 5:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->middle = static_cast<std::uint8_t>(i);
                            break;
                        case 6:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->bass = static_cast<std::uint8_t>(i);
                            break;
                        case 7:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->presence = static_cast<std::uint8_t>(i);
                            break;
                        case 9:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->depth = static_cast<std::uint8_t>(i);
                            break;
                        case 10:
                            i = m_xml->readElementText().toInt() >> 8;
                            m_amp_settings->bias = static_cast<std::uint8_t>(i);
                            break;
                        case 15:
                            i = m_xml->readElementText().toInt();
                            m_amp_settings->noise_gate = static_cast<std::uint8_t>(i);
                            break;
                        case 16:
                            i = m_xml->readElementText().toInt();
                            m_amp_settings->threshold = static_cast<std::uint8_t>(i);
                            break;
                        case 17:
                            i = m_xml->readElementText().toInt();
                            m_amp_settings->cabinet = static_cast<cabinets>(i);
                            break;
                        case 19:
                            i = m_xml->readElementText().toInt();
                            m_amp_settings->sag = static_cast<std::uint8_t>(i);
                            break;
                        case 20:
                            m_amp_settings->brightness = (m_xml->readElementText().toInt() != 0);
                            break;
                    }
                }
            }
            m_xml->readNext();
        }
    }

    void LoadFromFile::parseFX()
    {
        std::vector<fx_pedal_settings> settings;

        m_xml->readNextStartElement();
        while (m_xml->name() != "FX")
        {
            fx_pedal_settings effect{FxSlot{0}, effects::EMPTY, 0, 0, 0, 0, 0, 0, false};

            if (m_xml->isStartElement())
            {
                if (m_xml->name() == "Module")
                {
                    const int position = m_xml->attributes().value("POS").toString().toInt();
                    effect.slot = FxSlot{static_cast<std::uint8_t>(position)};

                    switch (m_xml->attributes().value("ID").toString().toInt())
                    {
                        case 0x00:
                            effect.effect_num = effects::EMPTY;
                            break;

                        case 0x3c:
                            effect.effect_num = effects::OVERDRIVE;
                            break;

                        case 0x49:
                            effect.effect_num = effects::WAH;
                            break;

                        case 0x4a:
                            effect.effect_num = effects::TOUCH_WAH;
                            break;

                        case 0x1a:
                            effect.effect_num = effects::FUZZ;
                            break;

                        case 0x1c:
                            effect.effect_num = effects::FUZZ_TOUCH_WAH;
                            break;

                        case 0x88:
                            effect.effect_num = effects::SIMPLE_COMP;
                            break;

                        case 0x07:
                            effect.effect_num = effects::COMPRESSOR;
                            break;

                        case 0x12:
                            effect.effect_num = effects::SINE_CHORUS;
                            break;

                        case 0x13:
                            effect.effect_num = effects::TRIANGLE_CHORUS;
                            break;

                        case 0x18:
                            effect.effect_num = effects::SINE_FLANGER;
                            break;

                        case 0x19:
                            effect.effect_num = effects::TRIANGLE_FLANGER;
                            break;

                        case 0x2d:
                            effect.effect_num = effects::VIBRATONE;
                            break;

                        case 0x40:
                            effect.effect_num = effects::VINTAGE_TREMOLO;
                            break;

                        case 0x41:
                            effect.effect_num = effects::SINE_TREMOLO;
                            break;

                        case 0x22:
                            effect.effect_num = effects::RING_MODULATOR;
                            break;

                        case 0x29:
                            effect.effect_num = effects::STEP_FILTER;
                            break;

                        case 0x4f:
                            effect.effect_num = effects::PHASER;
                            break;

                        case 0x1f:
                            effect.effect_num = effects::PITCH_SHIFTER;
                            break;

                        case 0x16:
                            effect.effect_num = effects::MONO_DELAY;
                            break;

                        case 0x43:
                            effect.effect_num = effects::MONO_ECHO_FILTER;
                            break;

                        case 0x48:
                            effect.effect_num = effects::STEREO_ECHO_FILTER;
                            break;

                        case 0x44:
                            effect.effect_num = effects::MULTITAP_DELAY;
                            break;

                        case 0x45:
                            effect.effect_num = effects::PING_PONG_DELAY;
                            break;

                        case 0x15:
                            effect.effect_num = effects::DUCKING_DELAY;
                            break;

                        case 0x46:
                            effect.effect_num = effects::REVERSE_DELAY;
                            break;

                        case 0x2b:
                            effect.effect_num = effects::TAPE_DELAY;
                            break;

                        case 0x2a:
                            effect.effect_num = effects::STEREO_TAPE_DELAY;
                            break;

                        case 0x24:
                            effect.effect_num = effects::SMALL_HALL_REVERB;
                            break;

                        case 0x3a:
                            effect.effect_num = effects::LARGE_HALL_REVERB;
                            break;

                        case 0x26:
                            effect.effect_num = effects::SMALL_ROOM_REVERB;
                            break;

                        case 0x3b:
                            effect.effect_num = effects::LARGE_ROOM_REVERB;
                            break;

                        case 0x4e:
                            effect.effect_num = effects::SMALL_PLATE_REVERB;
                            break;

                        case 0x4b:
                            effect.effect_num = effects::LARGE_PLATE_REVERB;
                            break;

                        case 0x4c:
                            effect.effect_num = effects::AMBIENT_REVERB;
                            break;

                        case 0x4d:
                            effect.effect_num = effects::ARENA_REVERB;
                            break;

                        case 0x21:
                            effect.effect_num = effects::FENDER_63_SPRING_REVERB;
                            break;

                        case 0x0b:
                            effect.effect_num = effects::FENDER_65_SPRING_REVERB;
                            break;
                    }
                }
                else if (m_xml->name() == "Param")
                {
                    int i = 0;
                    switch (m_xml->attributes().value("ControlIndex").toString().toInt())
                    {
                        case 0:
                            i = m_xml->readElementText().toInt() >> 8;
                            effect.knob1 = static_cast<std::uint8_t>(i);
                            break;
                        case 1:
                            i = m_xml->readElementText().toInt() >> 8;
                            effect.knob2 = static_cast<std::uint8_t>(i);
                            break;
                        case 2:
                            i = m_xml->readElementText().toInt() >> 8;
                            effect.knob3 = static_cast<std::uint8_t>(i);
                            break;
                        case 3:
                            i = m_xml->readElementText().toInt() >> 8;
                            effect.knob4 = static_cast<std::uint8_t>(i);
                            break;
                        case 4:
                            i = m_xml->readElementText().toInt() >> 8;
                            effect.knob5 = static_cast<std::uint8_t>(i);
                            break;
                        case 5:
                            i = m_xml->readElementText().toInt() >> 8;
                            effect.knob6 = static_cast<std::uint8_t>(i);
                            break;
                    }
                }
            }

            if (effect.effect_num != effects::EMPTY)
            {
                settings.push_back(effect);
            }
            m_xml->readNext();
        }

        m_fx_settings = settings;
    }

    void LoadFromFile::parseFUSE()
    {
        m_xml->readNextStartElement();
        while (!m_xml->isEndElement())
        {
            if (m_xml->name() == "Info")
            {
                m_name->operator=(m_xml->attributes().value("name").toString());
            }
            m_xml->readNext();
        }
    }
}
