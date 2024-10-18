#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "daisychains/take.h"
#include "daisychains/elements_equal.h"
#include "daisychains/from_range.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

TEST(Take, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::take(3) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(1, 2, 3));
}

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::take(3) |  //
    dc::elements_equal(std::vector{1, 2, 3});
}());

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::take(0) |  //
    dc::elements_equal(std::vector<int>{});
}());