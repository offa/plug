/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2021  offa
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

#include "com/MustangUpdater.h"
#include "com/Mustang.h"
#include "com/Packet.h"
#include <chrono>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

namespace plug::com
{
    namespace
    {
        // amp's VID and PID while in update mode
        inline constexpr std::uint16_t USB_UPDATE_VID{0x1ed8};
        inline constexpr std::uint16_t SMALL_AMPS_USB_UPDATE_PID{0x0006};    //Mustang I and II
        inline constexpr std::uint16_t BIG_AMPS_USB_UPDATE_PID{0x0007};      //Mustang III, IV, V
        inline constexpr std::uint16_t MINI_USB_UPDATE_PID{0x0011};          //Mustang Mini
        inline constexpr std::uint16_t FLOOR_USB_UPDATE_PID{0x0013};         //Mustang Floor
        inline constexpr std::uint16_t SMALL_AMPS_V2_USB_UPDATE_PID{0x0015}; //Mustang I & II V2
        inline constexpr std::uint16_t BIG_AMPS_V2_USB_UPDATE_PID{0x0017};   //Mustang III+ V2
    }

    namespace
    {
        void closeUsb(libusb_device_handle* handle)
        {
            if (handle != nullptr)
            {
                const int ret = libusb_release_interface(handle, 0);

                if (ret != LIBUSB_ERROR_NO_DEVICE)
                {
                    libusb_attach_kernel_driver(handle, 0);
                }

                libusb_close(handle);
                libusb_exit(nullptr);
            }
        }


        inline constexpr std::chrono::milliseconds timeout{500};
        inline constexpr std::size_t sizeOfPacket = packetRawTypeSize;
    }


    int updateFirmware(const char* filename)
    {
        // initialize libusb
        int ret = libusb_init(nullptr);
        if (ret != 0)
        {
            return ret;
        }

        // get handle for the device
        libusb_device_handle* amp_hand = libusb_open_device_with_vid_pid(nullptr, USB_UPDATE_VID, SMALL_AMPS_USB_UPDATE_PID);
        if (amp_hand == nullptr)
        {
            amp_hand = libusb_open_device_with_vid_pid(nullptr, USB_UPDATE_VID, BIG_AMPS_USB_UPDATE_PID);
            if (amp_hand == nullptr)
            {
                amp_hand = libusb_open_device_with_vid_pid(nullptr, USB_UPDATE_VID, SMALL_AMPS_V2_USB_UPDATE_PID);
                if (amp_hand == nullptr)
                {
                    amp_hand = libusb_open_device_with_vid_pid(nullptr, USB_UPDATE_VID, BIG_AMPS_V2_USB_UPDATE_PID);
                    if (amp_hand == nullptr)
                    {
                        amp_hand = libusb_open_device_with_vid_pid(nullptr, USB_UPDATE_VID, MINI_USB_UPDATE_PID);
                        if (amp_hand == nullptr)
                        {
                            amp_hand = libusb_open_device_with_vid_pid(nullptr, USB_UPDATE_VID, FLOOR_USB_UPDATE_PID);
                            if (amp_hand == nullptr)
                            {
                                libusb_exit(nullptr);
                                return -100;
                            }
                        }
                    }
                }
            }
        }

        // detach kernel driver
        ret = libusb_kernel_driver_active(amp_hand, 0);
        if (ret != 0)
        {
            ret = libusb_detach_kernel_driver(amp_hand, 0);
            if (ret != 0)
            {
                closeUsb(amp_hand);
                return ret;
            }
        }

        // claim the device
        ret = libusb_claim_interface(amp_hand, 0);
        if (ret != 0)
        {
            closeUsb(amp_hand);
            return ret;
        }

        FILE* file = fopen(filename, "rb");
        // send date when firmware was created
        fseek(file, 0x1a, SEEK_SET);
        unsigned char array[sizeOfPacket];
        memset(array, 0x00, sizeOfPacket);
        array[0] = 0x02;
        array[1] = 0x03;
        array[2] = 0x01;
        array[3] = 0x06;
        [[maybe_unused]] const auto n = fread(array + 4, 1, 11, file);
        int recieved{0};
        ret = libusb_interrupt_transfer(amp_hand, 0x01, array, sizeOfPacket, &recieved, timeout.count());
        libusb_interrupt_transfer(amp_hand, 0x81, array, sizeOfPacket, &recieved, timeout.count());
        usleep(10000);

        // send firmware
        fseek(file, 0x110, SEEK_SET);
        for (;;)
        {
            memset(array, 0x00, sizeOfPacket);
            unsigned char number{0};
            array[0] = array[1] = 0x03;
            array[2] = number;
            ++number;
            array[3] = static_cast<std::uint8_t>(fread(array + 4, 1, sizeOfPacket - 8, file));
            ret = libusb_interrupt_transfer(amp_hand, 0x01, array, sizeOfPacket, &recieved, timeout.count());
            libusb_interrupt_transfer(amp_hand, 0x81, array, sizeOfPacket, &recieved, timeout.count());
            usleep(10000);

            if (feof(file) != 0) // if reached end of the file
            {
                break; // exit loop
            }
        }
        fclose(file);

        // send "finished" packet
        memset(array, 0x00, sizeOfPacket);
        array[0] = 0x04;
        array[1] = 0x03;
        libusb_interrupt_transfer(amp_hand, 0x01, array, sizeOfPacket, &recieved, timeout.count());
        libusb_interrupt_transfer(amp_hand, 0x81, array, sizeOfPacket, &recieved, timeout.count());

        closeUsb(amp_hand);

        return 0;
    }
}
