#pragma once

#include "daisychains/fwd.h"

namespace dc {

class push_result {
 private:
  // TODO make this state information an enum?
  // TODO this should be an enum probably because the states are mutually exclusive
  bool stop_iterating_ : 1 = false;
  bool restart_ : 1 = false;

  constexpr push_result() = default;
 public:

  static constexpr auto stop_iterating() {
    auto rv = push_result{};
    rv.stop_iterating_ = true;
    return rv;
  }

  static constexpr auto restart() {
    auto rv = push_result{};
    rv.restart_ = true;
    return rv;
  }

  static constexpr auto keep_iterating() {
    auto rv = push_result{};
    rv.stop_iterating_ = false;
    return rv;
  }

  [[nodiscard]] constexpr bool should_stop_iterating() const {
    return stop_iterating_;
  }

  [[nodiscard]] constexpr bool should_restart() const {
    return restart_;
  }

};

}  // namespace dc