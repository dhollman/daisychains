#pragma once

#include "daisychains/fwd.h"
#include "daisychains/push_result.h"

namespace dc {

template <class Container>
class to_terminal : impl::terminal_base<to_terminal<Container>> {
 public:
  using terminal_t = to_terminal;
  template <class InputTypes>
  class imbued {
   public:
    using terminal_t = to_terminal;
    using input_types = InputTypes;

    explicit constexpr imbued(to_terminal) {}

    template <class ValueDeduced>
    constexpr auto push_value(ValueDeduced&& value) {
      container_.push_back(std::forward<ValueDeduced>(value));
      return push_result::keep_iterating();
    }

    template <class Self>
    constexpr auto get_output(this Self&& self) {
      return std::forward_like<Self>(self.container_);
    }

    constexpr auto push_stop(push_result result) { return result; }

   private:
    Container container_;
  };
};

namespace impl {

template <class Container>
struct to_fn {
  constexpr auto operator()() const { return to_terminal<Container>(); }
};

}  // namespace impl

template <class Container>
inline constexpr auto to = impl::to_fn<Container>{};

}  // namespace dc