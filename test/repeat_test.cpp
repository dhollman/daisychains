#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "daisychains/repeat.h"
#include "daisychains/elements_equal.h"
#include "daisychains/from_range.h"
#include "daisychains/take.h"
#include "daisychains/to.h"

using ::testing::ElementsAre;

TEST(Repeat, Works) {
  auto result =  //
    dc::repeat(42) |  //
    dc::take(5) |  //
    dc::to<std::vector<int>>();
  EXPECT_THAT(result, ElementsAre(42, 42, 42, 42, 42));
}

static_assert([] {
  return //
    dc::repeat(42) |  //
    dc::take(5) |  //
    dc::elements_equal(std::vector{42, 42, 42, 42, 42});
}());