#include <gtest/gtest.h>

#include <vector>

#include "daisychains/chain.h"
#include "daisychains/filter.h"
#include "daisychains/from_range.h"
#include "daisychains/to.h"

// Demonstrate some basic assertions.
TEST(Filter, Works) {
  auto chain = dc::from_range(std::vector{1, 2, 3, 4, 5}) |
               dc::filter([](int i) { return i % 2 == 0; }) |
               dc::to<std::vector<int>>();
}