
#include <gmock/gmock.h>


class TypeTest : public testing::Test
{
};

TEST_F(TypeTest, buildAndTestWorks)
{
    EXPECT_EQ(true, true);
}

