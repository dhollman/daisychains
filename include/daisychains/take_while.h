#pragma once

#include "daisychains/adaptor.h"
#include "daisychains/concepts.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"
#include "daisychains/push_result.h"

namespace dc {

template <class Fn>
class take_while_link {
 private:
  Fn fn_;

 public:
  using link_t = take_while_link;

  template <helpers::deduced_reference_for<Fn> FnDeduced>
  constexpr explicit take_while_link(FnDeduced&& fn)
      : fn_(std::forward<FnDeduced>(fn)) {}

  template <class... Ts>
  static constexpr auto get_output_types_from_input_types(
      meta::type_list<Ts...>) {
    return meta::type_list<Ts...>{};
  }

  template <class Wrapped, class InputTypes,
            class = meta::indices_for<InputTypes>>
  class adaptor;

  template <class Wrapped, class... InputTypes, size_t... Idxs>
  class adaptor<Wrapped, meta::type_list<InputTypes...>,
                std::index_sequence<Idxs...>>
      : public adaptor_mixin<adaptor<Wrapped, meta::type_list<InputTypes...>,
                                     std::index_sequence<Idxs...>>,
                             take_while_link>,
        public output_passthrough_mixin {
   private:
    take_while_link link_;
    bool done_ = false;

    using adaptor_mixin_t = adaptor_mixin<adaptor, take_while_link>;

   public:
    template <class LinkDeduced, class WrappedDeduced, class InputTs>
    constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs)
        : adaptor_mixin_t(std::forward<WrappedDeduced>(wrapped)),
          link_(std::forward<LinkDeduced>(link)) {}

    template <class Self, class... Args>
    constexpr auto push_value(this Self&& self, Args&&... args) {
      if (!self.done_ && self.link_.fn_(args...)) {
        return self.base().push_value(std::forward<Args>(args)...);
      } else {
        self.done_ = true;
        return push_result::stop_iterating();
      }
    }
  };
};

inline constexpr struct take_while_fn {
  template <class Fn>
  constexpr auto operator()(Fn&& fn) const {
    return take_while_link<std::remove_cvref_t<Fn>>{std::forward<Fn>(fn)};
  }
} take_while = {};

}  // namespace dc