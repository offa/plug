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

#include "FxSlot.h"
#include <gmock/gmock.h>


namespace plug::test
{
    class FxSlotTest : public testing::Test
    {
    };

    TEST_F(FxSlotTest, validSlotId)
    {
        constexpr FxSlot slotPre{0};
        EXPECT_EQ(slotPre.id(), 0);
        EXPECT_FALSE(slotPre.isFxLoop());

        constexpr FxSlot slotPost{4};
        EXPECT_EQ(slotPost.id(), 4);
        EXPECT_TRUE(slotPost.isFxLoop());
    }

    TEST_F(FxSlotTest, invalidSlotIdThrows)
    {
        EXPECT_THROW(FxSlot{9}, std::invalid_argument);
    }

}
