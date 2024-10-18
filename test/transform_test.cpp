#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>

#include "daisychains/transform.h"
#include "daisychains/from_range.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

TEST(Transform, Works) {
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::transform([](int i) { return i * 2; }) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(2, 4, 6, 8, 10));
}