/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2021  offa
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

#include "com/Mustang.h"
#include "com/ConnectionFactory.h"
#include "com/UsbContext.h"
#include "Version.h"
#include <iostream>

int main()
{
    using namespace plug::com::usb;

    std::cout << " === Plug v" << plug::version() << " - Integrationtest ===\n\n";

    Context ctx;
    auto devs = listDevices();

    for (auto& d : devs)
    {
        try
        {
            std::cout << " - " << d.vendorId() << ":" << d.productId() << "\n";
            d.open();
        }
        catch (const std::exception& ex)
        {
            std::cout << "ERROR: " << ex.what() << "\n";
        }
    }

    return 0;
}
