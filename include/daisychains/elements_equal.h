#pragma once

#include <concepts>
#include <ranges>

#include "daisychains/concepts.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/push_result.h"

namespace dc {

template <std::ranges::input_range Range>
class elements_equal_terminal
    : impl::terminal_base<elements_equal_terminal<Range>> {
 private:
  Range expected_;

 public:
  using terminal_t = elements_equal_terminal;

  template <helpers::deduced_reference_for<Range> RangeDeduced>
  explicit constexpr elements_equal_terminal(RangeDeduced&& expected)
      : expected_(std::forward<RangeDeduced>(expected)) {}

  template <class InputTypes>
  class imbued {
   private:
    using iterator_t = std::ranges::iterator_t<Range>;
    using sentinel_t = std::ranges::sentinel_t<Range>;

    elements_equal_terminal terminal_;
    iterator_t expected_iter_;
    bool all_match_;
    // TODO figure out whether we want to store the sentinel here (probably not)

   public:
    using terminal_t = elements_equal_terminal;
    using input_types = InputTypes;

    template <
        helpers::deduced_reference_for<elements_equal_terminal> TerminalDeduced>
    explicit constexpr imbued(TerminalDeduced&& terminal)
        : terminal_(std::forward<TerminalDeduced>(terminal)),
          expected_iter_(std::ranges::begin(terminal_.expected_)),
          all_match_(std::ranges::empty(terminal_.expected_)) {}

    template <class ValueDeduced>
    constexpr auto push_value(ValueDeduced&& value) {
      if (expected_iter_ == std::ranges::end(terminal_.expected_)) {
        all_match_ = false;
        return push_result::stop_iterating();
      } else {
        if (value == *expected_iter_++) {
          if (expected_iter_ == std::ranges::end(terminal_.expected_)) {
            all_match_ = true;
            return push_result::keep_iterating();
          } else {
            return push_result::keep_iterating();
          }
        } else {
          all_match_ = false;
          return push_result::stop_iterating();
        }
      }
    }

    constexpr auto get_output() { return all_match_; }
    constexpr auto push_stop(push_result result) { return result; }
  };
};

inline constexpr struct elements_equal_fn {
  template <std::ranges::input_range Range>
  constexpr auto operator()(Range&& range) const {
    return elements_equal_terminal<std::remove_cvref_t<Range>>(
        std::forward<Range>(range));
  }
} elements_equal = {};

}  // namespace dc