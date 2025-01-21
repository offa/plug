/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2025  offa
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

#include <libusb-1.0/libusb.h>

namespace plug::com::usb::libusb
{
    class ErrorCodeAdapter
    {
    public:
        constexpr explicit ErrorCodeAdapter(int errorCode)
            : errorCode_(errorCode)
        {
        }

        constexpr operator int() const noexcept
        {
            return errorCode_;
        }

        constexpr operator libusb_error() const noexcept
        {
            return static_cast<libusb_error>(errorCode_);
        }

    private:
        int errorCode_;
    };


    const char* strerror(ErrorCodeAdapter errorCode);
}
