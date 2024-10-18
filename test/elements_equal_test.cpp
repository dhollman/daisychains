#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "daisychains/elements_equal.h"
#include "daisychains/from_range.h"
#include "daisychains/filter.h"

TEST(ElementsEqual, Works) {
  auto expected = std::vector{1, 2, 3, 4, 5};
  auto result =  //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::filter([](auto) { return true; }) |  //
    dc::elements_equal(expected);
  EXPECT_TRUE(result);
}
  
static_assert([] {
  auto expected = std::vector{1, 2, 3, 4, 5};
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::filter([](auto) { return true; }) |  //
    dc::elements_equal(expected);
}());

static_assert([] {
  auto expected = std::vector<int>();
  return //
    dc::from_range(std::vector{1, 2, 3, 4, 5}) |  //
    dc::filter([](auto) { return false; }) |  //
    dc::elements_equal(expected);
}());