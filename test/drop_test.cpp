#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "daisychains/drop.h"
#include "daisychains/elements_equal.h"
#include "daisychains/from_range.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

TEST(Drop, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::drop(2) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(3, 4, 5));
}

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::drop(2) |  //
    dc::elements_equal(std::vector{3, 4, 5});
}());

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::drop(5) |  //
    dc::elements_equal(std::vector<int>{});
}());