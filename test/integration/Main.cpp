/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2018  offa
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

#include "mustang.h"
#include "version.h"
#include <iostream>

int main()
{
    std::cout << " === Plug v" << plug::version() << " - Integrationtest ===\n\n";

    plug::Mustang m;

    if( m.start_amp(nullptr, nullptr, nullptr, nullptr) == 0 )
    {
        std::cout << " * Connection OK\n";
    }
    else
    {
        std::cout << " * Connection FAILED\n";
        return 1;
    }

    return 0;
}
