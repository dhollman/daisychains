#pragma once

#include <utility>

#include "daisychains/adaptor.h"
#include "daisychains/fwd.h"
#include "daisychains/link.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"
#include "daisychains/push_result.h"

namespace dc {

class cycle_link : impl::link_base<cycle_link> {
 public:
  using link_t = cycle_link;

  template <class... Ts>
  static constexpr auto get_output_types_from_input_types(
      meta::type_list<Ts...>) {
    return meta::type_list<Ts...>{};
  }

  template <class Wrapped, class InputTypes,
            class = meta::indices_for<InputTypes>>
  class adaptor;
};

template <class Wrapped, class... InputTypes, size_t... Idxs>
class cycle_link::adaptor<Wrapped, meta::type_list<InputTypes...>,
                          std::index_sequence<Idxs...>>
    : public adaptor_mixin<adaptor<Wrapped, meta::type_list<InputTypes...>,
                                   std::index_sequence<Idxs...>>,
                           cycle_link>,
      public output_passthrough_mixin {
 private:
  cycle_link link_;

  using adaptor_mixin_t = adaptor_mixin<adaptor, cycle_link>;

 public:
  template <class LinkDeduced, class WrappedDeduced, class InputTs>
  constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped,
                    InputTs)
      : adaptor_mixin_t(std::forward<LinkDeduced>(link),
                        std::forward<WrappedDeduced>(wrapped)),
        link_(std::forward<LinkDeduced>(link)) {}

  template <class... Args>
  constexpr auto push_value(Args&&... args) {
    return this->base().push_value(std::forward<Args>(args)...);
  }

  constexpr auto push_stop(push_result result) {
    auto downstream_result = this->base().push_stop(result);
    if (downstream_result.should_stop_iterating()) {
      return push_result::stop_iterating();
    } else if (result.should_stop_iterating()) {
      return push_result::should_restart();
    } else {
      return downstream_result;
    }
  }
};

inline constexpr struct cycle_fn {
  constexpr auto operator()() const { return cycle_link{}; }
} cycle = {};
}  // namespace dc