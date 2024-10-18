#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "daisychains/from_range.h"
#include "daisychains/filter.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

// Demonstrate some basic assertions.
TEST(Filter, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::filter([](int i) { return i % 2 == 0; }) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(2, 4));
}

static_assert([] {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::filter([](int i) { return i % 2 == 0; }) |  //
    dc::to<std::vector<int>>();
  return result == std::vector{2, 4};
}());