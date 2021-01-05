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

#pragma once

#include "SignalChain.h"
#include "com/Connection.h"
#include <string_view>
#include <vector>
#include <memory>
#include <cstdint>

namespace plug::com
{
    using InitalData = std::tuple<SignalChain, std::vector<std::string>>;

    class Mustang
    {
    public:
        explicit Mustang(std::shared_ptr<Connection> connection);
        Mustang(const Mustang&) = delete;

        InitalData start_amp();
        void stop_amp();
        void set_effect(fx_pedal_settings value);
        void set_amplifier(amp_settings value);
        void save_on_amp(std::string_view name, std::uint8_t slot);
        SignalChain load_memory_bank(std::uint8_t slot);
        void save_effects(std::uint8_t slot, std::string_view name, const std::vector<fx_pedal_settings>& effects);


        Mustang& operator=(const Mustang&) = delete;


    private:
        InitalData loadData();
        void initializeAmp();

        const std::shared_ptr<Connection> conn;
    };
}
