#pragma once

#include <concepts>
#include "daisychains/adaptor.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"
#include "daisychains/push_result.h"

namespace dc {

class drop_link {
 private:
  size_t count_;

 public:
  using link_t = drop_link;

  template <std::integral T>
  explicit constexpr drop_link(T count) : count_(static_cast<size_t>(count)) {
    assert(count >= 0);
  }

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
class drop_link::adaptor<Wrapped, meta::type_list<InputTypes...>,
                         std::index_sequence<Idxs...>>
    : public adaptor_mixin<adaptor<Wrapped, meta::type_list<InputTypes...>,
                                   std::index_sequence<Idxs...>>,
                           drop_link>,
      public output_passthrough_mixin {
 private:
  drop_link link_;
  size_t count_;

  using adaptor_mixin_t = adaptor_mixin<adaptor, drop_link>;

 public:
  template <class LinkDeduced, class WrappedDeduced, class InputTs>
  constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs&&...)
    : adaptor_mixin_t(std::forward<WrappedDeduced>(wrapped)),
      link_(std::forward<LinkDeduced>(link)),
      count_(link_.count_) {}

  template <class Self, class... Args>
  constexpr auto push_value(this Self&& self, Args&&... args) {
    if (self.count_ > 0) {
      --self.count_;
      return push_result::keep_iterating();
    } else {
      return self.base().push_value(std::forward<Args>(args)...);
    }
  }

  constexpr void restart() { count_ = link_.count_; }

};

inline constexpr struct drop_fn {
  template <class T>
  constexpr auto operator()(T&& value) const {
    return drop_link{std::forward<T>(value)};
  }
} drop = {};

}  // namespace dc