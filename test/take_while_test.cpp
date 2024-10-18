#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "daisychains/take_while.h"
#include "daisychains/from_range.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

TEST(TakeWhile, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::take_while([](int i) { return i < 4; }) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(1, 2, 3));
}
