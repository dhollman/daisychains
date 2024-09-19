#pragma once

#include <ranges>

#include "daisychains/concepts.h"
#include "daisychains/generator.h"

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

  template <dc::CompleteLink Wrapped>
  struct adaptor {
   public:
    using generator_t = from_range_generator;

   private:
    Wrapped wrapped_;
    generator_t generator_;

   public:
    template <helpers::deduced_reference_for<Wrapped> LinkDeduced,
              helpers::deduced_reference_for<generator_t> GenDeduced>
    constexpr adaptor(LinkDeduced&& link, GenDeduced&& gen)
        : wrapped_(std::forward<LinkDeduced>(link)),
          generator_(std::forward<GenDeduced>(gen)) {}

    template <class Self>
    constexpr auto generate(this Self&& self) {
      for (auto&& val : self.generator_.rng_) {
        self.wrapped_.push_value(val);
      }
    }
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