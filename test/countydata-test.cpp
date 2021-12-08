#include <gtest/gtest.h>

#include "county.h"
#include "countydata.h"

// TODO: Define this path in the project because relative paths to test
// resources is always a bad idea.
static constexpr auto svgPath = "../resources/Usa_counties_large.svg";

struct CountyDataTest : public ::testing::Test {
  CountyData data;
};

// This is an assert in the constructor, but let's test it explicitly
TEST_F(CountyDataTest, SanityCheck)
{
  constexpr size_t nCountiesInUSA = 3143U;
  ASSERT_EQ(data.size(), nCountiesInUSA);
}


// TODO: Tests of readFromFile() for various files and countyVisited()

