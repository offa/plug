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

#include "com/IdLookup.h"
#include <gmock/gmock.h>

using plug::amps;
using plug::cabinets;
using plug::effects;
using plug::lookupAmpById;
using plug::lookupCabinetById;
using plug::lookupEffectById;

class IdLookupTest : public testing::Test
{
protected:
};


TEST_F(IdLookupTest, lookupAmpById)
{
    EXPECT_EQ(lookupAmpById(0x67), amps::FENDER_57_DELUXE);
    EXPECT_EQ(lookupAmpById(0x64), amps::FENDER_59_BASSMAN);
    EXPECT_EQ(lookupAmpById(0x7c), amps::FENDER_57_CHAMP);
    EXPECT_EQ(lookupAmpById(0x53), amps::FENDER_65_DELUXE_REVERB);
    EXPECT_EQ(lookupAmpById(0x6a), amps::FENDER_65_PRINCETON);
    EXPECT_EQ(lookupAmpById(0x75), amps::FENDER_65_TWIN_REVERB);
    EXPECT_EQ(lookupAmpById(0x72), amps::FENDER_SUPER_SONIC);
    EXPECT_EQ(lookupAmpById(0x61), amps::BRITISH_60S);
    EXPECT_EQ(lookupAmpById(0x79), amps::BRITISH_70S);
    EXPECT_EQ(lookupAmpById(0x5e), amps::BRITISH_80S);
    EXPECT_EQ(lookupAmpById(0x5d), amps::AMERICAN_90S);
    EXPECT_EQ(lookupAmpById(0x6d), amps::METAL_2000);
}

TEST_F(IdLookupTest, lookupAmpByIdThrowsOnInvalidId)
{
    EXPECT_THROW(lookupAmpById(0x00), std::invalid_argument);
}

TEST_F(IdLookupTest, lookupEffectById)
{
    EXPECT_EQ(lookupEffectById(0x00), effects::EMPTY);
    EXPECT_EQ(lookupEffectById(0x3c), effects::OVERDRIVE);
    EXPECT_EQ(lookupEffectById(0x49), effects::WAH);
    EXPECT_EQ(lookupEffectById(0x4a), effects::TOUCH_WAH);
    EXPECT_EQ(lookupEffectById(0x1a), effects::FUZZ);
    EXPECT_EQ(lookupEffectById(0x1c), effects::FUZZ_TOUCH_WAH);
    EXPECT_EQ(lookupEffectById(0x88), effects::SIMPLE_COMP);
    EXPECT_EQ(lookupEffectById(0x07), effects::COMPRESSOR);
    EXPECT_EQ(lookupEffectById(0x12), effects::SINE_CHORUS);
    EXPECT_EQ(lookupEffectById(0x13), effects::TRIANGLE_CHORUS);
    EXPECT_EQ(lookupEffectById(0x18), effects::SINE_FLANGER);
    EXPECT_EQ(lookupEffectById(0x19), effects::TRIANGLE_FLANGER);
    EXPECT_EQ(lookupEffectById(0x2d), effects::VIBRATONE);
    EXPECT_EQ(lookupEffectById(0x40), effects::VINTAGE_TREMOLO);
    EXPECT_EQ(lookupEffectById(0x41), effects::SINE_TREMOLO);
    EXPECT_EQ(lookupEffectById(0x22), effects::RING_MODULATOR);
    EXPECT_EQ(lookupEffectById(0x29), effects::STEP_FILTER);
    EXPECT_EQ(lookupEffectById(0x4f), effects::PHASER);
    EXPECT_EQ(lookupEffectById(0x1f), effects::PITCH_SHIFTER);
    EXPECT_EQ(lookupEffectById(0x16), effects::MONO_DELAY);
    EXPECT_EQ(lookupEffectById(0x43), effects::MONO_ECHO_FILTER);
    EXPECT_EQ(lookupEffectById(0x48), effects::STEREO_ECHO_FILTER);
    EXPECT_EQ(lookupEffectById(0x44), effects::MULTITAP_DELAY);
    EXPECT_EQ(lookupEffectById(0x45), effects::PING_PONG_DELAY);
    EXPECT_EQ(lookupEffectById(0x15), effects::DUCKING_DELAY);
    EXPECT_EQ(lookupEffectById(0x46), effects::REVERSE_DELAY);
    EXPECT_EQ(lookupEffectById(0x2b), effects::TAPE_DELAY);
    EXPECT_EQ(lookupEffectById(0x2a), effects::STEREO_TAPE_DELAY);
    EXPECT_EQ(lookupEffectById(0x24), effects::SMALL_HALL_REVERB);
    EXPECT_EQ(lookupEffectById(0x3a), effects::LARGE_HALL_REVERB);
    EXPECT_EQ(lookupEffectById(0x26), effects::SMALL_ROOM_REVERB);
    EXPECT_EQ(lookupEffectById(0x3b), effects::LARGE_ROOM_REVERB);
    EXPECT_EQ(lookupEffectById(0x4e), effects::SMALL_PLATE_REVERB);
    EXPECT_EQ(lookupEffectById(0x4b), effects::LARGE_PLATE_REVERB);
    EXPECT_EQ(lookupEffectById(0x4c), effects::AMBIENT_REVERB);
    EXPECT_EQ(lookupEffectById(0x4d), effects::ARENA_REVERB);
    EXPECT_EQ(lookupEffectById(0x21), effects::FENDER_63_SPRING_REVERB);
    EXPECT_EQ(lookupEffectById(0x0b), effects::FENDER_65_SPRING_REVERB);
}

TEST_F(IdLookupTest, lookupEffectByIdThrowsOnInvalidId)
{
    EXPECT_THROW(lookupEffectById(0xff), std::invalid_argument);
}

TEST_F(IdLookupTest, lookupCabinetById)
{
    EXPECT_EQ(lookupCabinetById(0x00), cabinets::OFF);
    EXPECT_EQ(lookupCabinetById(0x01), cabinets::cab57DLX);
    EXPECT_EQ(lookupCabinetById(0x02), cabinets::cabBSSMN);
    EXPECT_EQ(lookupCabinetById(0x03), cabinets::cab65DLX);
    EXPECT_EQ(lookupCabinetById(0x04), cabinets::cab65PRN);
    EXPECT_EQ(lookupCabinetById(0x05), cabinets::cabCHAMP);
    EXPECT_EQ(lookupCabinetById(0x06), cabinets::cab4x12M);
    EXPECT_EQ(lookupCabinetById(0x07), cabinets::cab2x12C);
    EXPECT_EQ(lookupCabinetById(0x08), cabinets::cab4x12G);
    EXPECT_EQ(lookupCabinetById(0x09), cabinets::cab65TWN);
    EXPECT_EQ(lookupCabinetById(0x0a), cabinets::cab4x12V);
    EXPECT_EQ(lookupCabinetById(0x0b), cabinets::cabSS212);
    EXPECT_EQ(lookupCabinetById(0x0c), cabinets::cabSS112);
}

TEST_F(IdLookupTest, lookupCabinetByIdThrowsOnInvalidId)
{
    EXPECT_THROW(lookupCabinetById(0xff), std::invalid_argument);
}
