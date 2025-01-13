#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

#include "daisychains/cycle.h"
#include "daisychains/drop.h"
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

TEST(Cycle, TakeTakeCycleWorks) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3}) |  //
    dc::take(7) |  //
    dc::take(2) |  //
    dc::cycle() |  //
    dc::take(9) |

    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(1, 2, 1, 2, 1, 2, 1, 2, 1));
}

TEST(Cycle, TakeDropCycleWorks) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3}) |  //
    dc::take(7) |  //
    dc::drop(2) |  //
    dc::cycle() |  //
    dc::take(9) |
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(3, 3, 3, 3, 3, 3, 3, 3, 3));
}

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3}) |  //
    dc::cycle() |  //
    dc::take(7) |  //
    dc::elements_equal(std::vector{1, 2, 3, 1, 2, 3, 1});
}());

// TODO this should be a runtime error in addition to a compile-time error, I think?
// (or possibly emptiness should be a special case that's handled by cycle and take?)
// Basically we need to decide what to do with this case because it's always nonsensical 
// from at least some perspectives, and from others there's sometimes reasonable behavior
// static_assert([] {
//   return //
//     dc::from_range(std::vector<int>{}) |  //
//     dc::cycle() |  //
//     dc::take(5) |  //
//     dc::elements_equal(std::vector<int>{});
// }());