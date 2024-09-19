#pragma once

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

    explicit imbued(to_terminal) {}

    template <class ValueDeduced>
    constexpr auto push_value(ValueDeduced&& value) {
      container_.push_back(std::forward<ValueDeduced>(value));
    }

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