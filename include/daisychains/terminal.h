#pragma once

#include "daisychains/fwd.h"
#include "daisychains/push_result.h"

namespace dc {

namespace impl {

template <class Derived>
class terminal_base {};

class imbued_terminal_base {
  public:
    constexpr auto push_stop(push_result result) { return result; }

};

}  // namespace impl

}  // namespace dc