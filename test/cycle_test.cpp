#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

#include "daisychains/cycle.h"
#include "daisychains/elements_equal.h"
#include "daisychains/from_range.h"
#include "daisychains/take.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

TEST(Cycle, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3}) |  //
    dc::cycle() |  //
    dc::take(7) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(1, 2, 3, 1, 2, 3, 1));
}

TEST(Cycle, TakeCycleWorks) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3}) |  //
    dc::take(2) |  //
    dc::cycle() |  //
    dc::take(7) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(1, 2, 1, 2, 1, 2, 1));
}

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3}) |  //
    dc::cycle() |  //
    dc::take(7) |  //
    dc::elements_equal(std::vector{1, 2, 3, 1, 2, 3, 1});
}());

static_assert([] {
  return //
    dc::from_range(std::vector<int>{}) |  //
    dc::cycle() |  //
    dc::take(5) |  //
    dc::elements_equal(std::vector<int>{});
}());