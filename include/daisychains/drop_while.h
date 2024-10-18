#pragma once

#include "daisychains/adaptor.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"

namespace dc {

template <class Fn>
class drop_while_link {
 private:
  Fn fn_;

 public:
  using link_t = drop_while_link;

  template <helpers::deduced_reference_for<Fn> FnDeduced>
  constexpr explicit drop_while_link(FnDeduced&& fn)
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
                             drop_while_link>,
        public output_passthrough_mixin {
   private:
    drop_while_link link_;
    bool started_ = false;

    using adaptor_mixin_t = adaptor_mixin<adaptor, drop_while_link>;

   public:
    template <class LinkDeduced, class WrappedDeduced, class InputTs>
    constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs)
        : adaptor_mixin_t(std::forward<WrappedDeduced>(wrapped)),
          link_(std::forward<LinkDeduced>(link)) {}

    template <class Self, class... Args>
    constexpr auto push_value(this Self&& self, Args&&... args) {
      if (!self.started_ && self.link_.fn_(args...)) {
        return push_result::keep_iterating();
      } else {
        self.started_ = true;
        return self.base().push_value(std::forward<Args>(args)...);
      }
    }
  };

};

inline constexpr struct drop_while_fn {
  template <class Fn>
  constexpr auto operator()(Fn&& fn) const {
    return drop_while_link<Fn>{std::forward<Fn>(fn)};
  }
} drop_while = {};

}  // namespace dc