#include "daisychains/elements_equal.h"
#include "daisychains/drop_while.h"
#include "daisychains/elements_equal.h"
#include "daisychains/from_range.h"
#include "daisychains/to.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

using ::testing::ElementsAre;

TEST(DropWhile, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::drop_while([](int i) { return i < 3; }) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(3, 4, 5));
}

static_assert([] {
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::drop_while([](int i) { return i < 3; }) |  //
    dc::elements_equal(std::vector{3, 4, 5});
});