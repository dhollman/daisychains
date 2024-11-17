#pragma once

#include "daisychains/adaptor.h"
#include "daisychains/concepts.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"
#include "daisychains/push_result.h"

namespace dc {

// TODO decide if we want to allow Value to be a reference type
template <class Value>
class repeat_generator
   : public impl::generator_base<repeat_generator<Value>> {
 private:
  Value value_;

 public:
  using output_type = Value;
  using generator_t = repeat_generator;

  template <helpers::deduced_reference_for<Value> ValueDeduced>
  explicit constexpr repeat_generator(ValueDeduced&& value)
      : value_(std::forward<ValueDeduced>(value)) {}

  template <class Wrapped>
  struct adaptor : public adaptor_mixin<adaptor<Wrapped>, repeat_generator>,
                   public output_passthrough_mixin {
   private:
    repeat_generator generator_;
    using generator_t = repeat_generator;

    using adaptor_mixin_t = adaptor_mixin<adaptor, repeat_generator>;

   public:
    template <helpers::deduced_reference_for<Wrapped> LinkDeduced,
              helpers::deduced_reference_for<repeat_generator> GenDeduced>
    constexpr adaptor(LinkDeduced&& link, GenDeduced&& gen)
        : adaptor_mixin_t(std::forward<LinkDeduced>(link)),
          generator_(std::forward<GenDeduced>(gen)) {}

    template <class Self>
    constexpr auto generate(this Self&& self) {
      while (true) {
        auto result = self.base().push_value(self.generator_.value_);
        if (self.check_for_completion(result, /*i_am_done=*/false)) {
          break;
        }
      }
    }
  };
};

inline constexpr struct repeat_fn {
  template <class Value>
  constexpr auto operator()(Value&& value) const {
    return repeat_generator<Value>(std::forward<Value>(value));
  }
} repeat = {};

}  // namespace dc