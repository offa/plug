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

#include "IdLookup.h"
#include <gmock/gmock.h>

using namespace plug;

class IdLookupTest : public testing::Test
{
protected:
};


TEST_F(IdLookupTest, lookupAmpById)
{
    EXPECT_THAT(lookupAmpById(0x67), amps::FENDER_57_DELUXE);
    EXPECT_THAT(lookupAmpById(0x64), amps::FENDER_59_BASSMAN);
    EXPECT_THAT(lookupAmpById(0x7c), amps::FENDER_57_CHAMP);
    EXPECT_THAT(lookupAmpById(0x53), amps::FENDER_65_DELUXE_REVERB);
    EXPECT_THAT(lookupAmpById(0x6a), amps::FENDER_65_PRINCETON);
    EXPECT_THAT(lookupAmpById(0x75), amps::FENDER_65_TWIN_REVERB);
    EXPECT_THAT(lookupAmpById(0x72), amps::FENDER_SUPER_SONIC);
    EXPECT_THAT(lookupAmpById(0x61), amps::BRITISH_60S);
    EXPECT_THAT(lookupAmpById(0x79), amps::BRITISH_70S);
    EXPECT_THAT(lookupAmpById(0x5e), amps::BRITISH_80S);
    EXPECT_THAT(lookupAmpById(0x5d), amps::AMERICAN_90S);
    EXPECT_THAT(lookupAmpById(0x6d), amps::METAL_2000);
}

TEST_F(IdLookupTest, lookupAmpByIdThrowsOnInvalidId)
{
    EXPECT_THROW(lookupAmpById(0x00), std::invalid_argument);
}
