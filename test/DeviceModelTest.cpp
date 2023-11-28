/*
 * PLUG - software to operate Fender Mustang amplifier
 *        Linux replacement for Fender FUSE software
 *
 * Copyright (C) 2017-2023  offa
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

#include "DeviceModel.h"
#include <gmock/gmock.h>

namespace plug::test
{
    class DeviceModelTest : public testing::Test
    {
    };


    TEST_F(DeviceModelTest, deviceInfos)
    {
        const DeviceModel model{"Mustang I", DeviceModel::Category::MustangV1, 100};
        EXPECT_EQ(model.name(), "Mustang I");
        EXPECT_EQ(model.category(), DeviceModel::Category::MustangV1);
        EXPECT_EQ(model.numberOfPresets(), 100);
    }
}
