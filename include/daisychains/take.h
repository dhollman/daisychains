#pragma once

#include "daisychains/adaptor.h"
#include "daisychains/fwd.h"
#include "daisychains/link.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"
#include "daisychains/push_result.h"

#include <cassert>
#include <cstddef>
#include <utility>

namespace dc {

class take_link : impl::link_base<take_link> {
 private:
  size_t count_;

 public:
  using link_t = take_link;

  explicit constexpr take_link(size_t count) : count_(count) {}

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
class take_link::adaptor<Wrapped, meta::type_list<InputTypes...>,
                         std::index_sequence<Idxs...>>
    : public adaptor_mixin<adaptor<Wrapped, meta::type_list<InputTypes...>,
                                   std::index_sequence<Idxs...>>,
                           take_link>,
      public output_passthrough_mixin {
 private:
  take_link link_;
  size_t count_;

  using adaptor_mixin_t = adaptor_mixin<adaptor, take_link>;

 public:
  template <class LinkDeduced, class WrappedDeduced, class InputTs>
  constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs)
      : adaptor_mixin_t(std::forward<WrappedDeduced>(wrapped)),
        link_(std::forward<LinkDeduced>(link)),
        count_(link_.count_) {}

  template <class Self, class... Args>
  constexpr auto push_value(this Self&& self, Args&&... args) {
    if (self.count_ > 0) {
      --self.count_;
      return self.base().push_value(std::forward<Args>(args)...);
    } else {
      return push_result::stop_iterating();
    }
  }

  constexpr auto push_stop(push_result result) {
    auto downstream_result = this->base().push_stop(result);
    if (downstream_result.should_restart()) {
      assert(count_ == 0);
      count_ = link_.count_;
    } else if (downstream_result.should_stop_iterating() || count_ == 0) {
      return push_result::stop_iterating();
    } else {
      return downstream_result;
    }
  }
};

inline constexpr struct take_fn {
  template <class T>
  constexpr auto operator()(T&& t) const {
    return take_link(std::forward<T>(t));
  }
} take = {};

}  // namespace dc