#pragma once

#include <ranges>
#include <type_traits>

#include "daisychains/adaptor.h"
#include "daisychains/concepts.h"
#include "daisychains/generator.h"
#include "daisychains/output_passthrough.h"

namespace dc {

template <std::ranges::input_range Range>
class from_range_generator
    : public impl::generator_base<from_range_generator<Range>> {
 private:
  Range rng_;

 public:
  using output_type = std::ranges::range_reference_t<Range>;
  using generator_t = from_range_generator;

  template <helpers::deduced_reference_for<Range> RangeDeduced>
  explicit constexpr from_range_generator(RangeDeduced&& rng)
      : rng_(std::forward<RangeDeduced>(rng)) {}

  template <class Wrapped>
  struct adaptor : public adaptor_mixin<adaptor<Wrapped>, from_range_generator>,
                   public output_passthrough_mixin {
   public:
    using generator_t = from_range_generator;

   private:
    using iterator_t = std::ranges::iterator_t<Range>;
    generator_t generator_;
    iterator_t spot_;

    using adaptor_mixin_t =
        adaptor_mixin<adaptor<Wrapped>, from_range_generator>;

   public:
    template <helpers::deduced_reference_for<Wrapped> LinkDeduced,
              helpers::deduced_reference_for<generator_t> GenDeduced>
    constexpr adaptor(LinkDeduced&& link, GenDeduced&& gen)
        : adaptor_mixin_t(std::forward<LinkDeduced>(link)),
          generator_(std::forward<GenDeduced>(gen)) {}

    // TODO(dhollman) move these musings to a markdown file somewhere
    // Note that the use of deducing this here is mostly for const
    // propagation.  In theory we could propagate reference-qulifiers
    // here (or maybe in a "generate_next", since the caller could know
    // something special about the last use), and have some special cases
    // where we know that we're the last use and we can move out of some
    // sort of temporary, let's not worry about that for now.  The other
    // thing we could think about here is if we want to abuse reference
    // qualifiers to pass on information about whether we're being used
    // in expression scope or not, which might allow us to do some sort
    // of optimizations that wouldn't be safe if we're not at expression
    // scope, but we don't have a use case for that yet.
    template <class Self>
    constexpr auto generate(this Self&& self) {
      self.restart();
      for (auto e = self.generator_.rng_.end(); self.spot_ != e;) {
        auto result = self.base().push_value(*self.spot_++);
        if (self.check_for_completion(result, /*i_am_done=*/self.spot_ == e)) {
          break;
        }
      }
    }

    constexpr void restart() { spot_ = generator_.rng_.begin(); }
  };
};

inline constexpr struct from_range_fn {
  template <std::ranges::input_range Range>
  constexpr auto operator()(Range&& rng) const {
    return from_range_generator<std::remove_cvref_t<Range>>(
        std::forward<Range>(rng));
  }
} from_range = {};

}  // namespace dc