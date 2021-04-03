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

#include "com/UsbException.h"
#include <libusb-1.0/libusb.h>

namespace plug::com::usb
{
    namespace
    {

        const char* libusb_strerror_wrapper(int errcode)
        {
            if constexpr (std::is_invocable_v<decltype(libusb_strerror), int>)
            {
                return libusb_strerror(errcode);
            }
            return libusb_strerror(static_cast<libusb_error>(errcode));
        }
    }

    UsbException::UsbException(int errorCode)
        : error_(errorCode),
          name_(libusb_error_name(errorCode)),
          message_(libusb_strerror_wrapper(errorCode))
    {
    }

    int UsbException::code() const noexcept
    {
        return error_;
    }

    std::string UsbException::name() const
    {
        return name_;
    }

    std::string UsbException::message() const
    {
        return message_;
    }

    const char* UsbException::what() const noexcept
    {
        return message_.c_str();
    }
}
