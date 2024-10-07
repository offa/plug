/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2024  offa
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

#include "com/ConnectionFactory.h"
#include "com/CommunicationException.h"
#include "com/Mustang.h"
#include "com/UsbComm.h"
#include "com/UsbContext.h"
#include "DeviceModel.h"
#include <algorithm>

namespace plug::com
{
    namespace
    {
        inline constexpr std::uint16_t usbVID{0x1ed8};

        namespace usbPID
        {
            inline constexpr std::uint16_t mustangI_II{0x0004};
            inline constexpr std::uint16_t mustangIII_IV_V{0x0005};
            inline constexpr std::uint16_t mustangBronco{0x000a};
            inline constexpr std::uint16_t mustangMini{0x0010};
            inline constexpr std::uint16_t mustangFloor{0x0012};
            inline constexpr std::uint16_t mustangI_II_v2{0x0014};
            inline constexpr std::uint16_t mustangIII_IV_V_v2{0x0016};

            inline constexpr std::uint16_t mustangLT25{0x0037};
            inline constexpr std::uint16_t rumbleLT25{0x0038};
            inline constexpr std::uint16_t mustangLT40S{0x0046};
            inline constexpr std::uint16_t mustangMicro{0x0043};
        }

        inline constexpr std::initializer_list<std::uint16_t> pids{
            usbPID::mustangI_II,
            usbPID::mustangIII_IV_V,
            usbPID::mustangBronco,
            usbPID::mustangMini,
            usbPID::mustangFloor,
            usbPID::mustangI_II_v2,
            usbPID::mustangIII_IV_V_v2,
            
            usbPID::mustangLT25,
            usbPID::rumbleLT25,
            usbPID::mustangLT40S,
            usbPID::mustangMicro
        };

        DeviceModel getModel(std::uint16_t pid)
        {
            switch (pid)
            {
                case usbPID::mustangI_II:
                    return DeviceModel{"Mustang I/II", DeviceModel::Category::MustangV1, 24};
                case usbPID::mustangIII_IV_V:
                    return DeviceModel{"Mustang III/IV/V", DeviceModel::Category::MustangV1, 100};
                case usbPID::mustangBronco:
                    return DeviceModel{"Mustang Bronco", DeviceModel::Category::MustangV1, 0};
                case usbPID::mustangMini:
                    return DeviceModel{"Mustang Mini", DeviceModel::Category::MustangV1, 0};
                case usbPID::mustangFloor:
                    return DeviceModel{"Mustang Floor", DeviceModel::Category::MustangV1, 0};
                case usbPID::mustangI_II_v2:
                    return DeviceModel{"Mustang I/II", DeviceModel::Category::MustangV2, 24};
                case usbPID::mustangIII_IV_V_v2:
                    return DeviceModel{"Mustang III/IV/V", DeviceModel::Category::MustangV2, 100};


                case usbPID::mustangLT25:
                    return DeviceModel{"Mustang LT 25", DeviceModel::Category::MustangV3_USB, 0};
                case usbPID::mustangLT40S:
                    return DeviceModel{"Mustang LT 40S", DeviceModel::Category::MustangV3_USB, 0};
                // TODO: add mustangLT50 support when PID is known
                case usbPID::rumbleLT25:
                    return DeviceModel{"Rumble LT 25", DeviceModel::Category::MustangV3_USB, 0};

                default:
                    throw CommunicationException{"Unknown device pid: " + std::to_string(pid)};
            }
        }

    }

    std::unique_ptr<Mustang> connect(bool v3usb_devices_enabled)
    {
        auto devices = usb::listDevices();

        auto itr = std::find_if(devices.begin(), devices.end(), [](const auto& dev)
                                { return (dev.vendorId() == usbVID) && std::any_of(pids.begin(), pids.end(), [&dev](std::uint16_t pid)
                                                                                   { return dev.productId() == pid; }); });

        if (itr == devices.end())
        {
            throw CommunicationException{"No device found"};
        }
        std::unique_ptr<Mustang> retval = std::make_unique<Mustang>(getModel(itr->productId()), std::make_shared<UsbComm>(std::move(*itr)));
        if(v3usb_devices_enabled==false && retval->getDeviceModel().category()==DeviceModel::Category::MustangV3_USB)
        {
            // throw CommunicationException{"V3 USB device found but not enabled"};
        }
        return retval;
    }

}
