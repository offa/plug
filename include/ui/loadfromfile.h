/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2022  offa
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

#pragma once

#include "data_structs.h"
#include <QFile>
#include <QXmlStreamReader>
#include <vector>
#include <memory>

class QXmlStreamReader;

namespace plug
{

    class LoadFromFile
    {
    public:
        LoadFromFile(QFile* file, QString* name, amp_settings* amp_settings, std::vector<fx_pedal_settings>& fx_settings);

        void loadfile();

    private:
        QString* m_name;
        amp_settings* m_amp_settings;
        std::vector<fx_pedal_settings>& m_fx_settings;
        const std::unique_ptr<QXmlStreamReader> m_xml;

        void parseAmp();
        void parseFX();
        void parseFUSE();
    };
}
