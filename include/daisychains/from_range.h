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

    template <class Self>
    constexpr auto generate_value(this Self&& self) { 
      return self.base().push_value(*self.spot_++);
    }

    constexpr bool generator_is_done() const {
      return spot_ == generator_.rng_.end();
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