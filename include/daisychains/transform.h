#pragma once

#include "daisychains/adaptor.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"

namespace dc {

template <class Fn>
class tranform_link {
 private:
  Fn fn_;

 public:
  using link_t = tranform_link;

  template <helpers::deduced_reference_for<Fn> FnDeduced>
  constexpr explicit tranform_link(FnDeduced&& fn)
      : fn_(std::forward<FnDeduced>(fn)) {}

  template <class... Ts>
  static constexpr auto get_output_types_from_input_types(
      meta::type_list<Ts...>) {
    return meta::type_list<std::invoke_result_t<Fn, Ts>...>{};
  }

  template <class Wrapped, class InputTypes,
            class = meta::indices_for<InputTypes>>
  class adaptor;

  template <class Wrapped, class... InputTypes, size_t... Idxs>
  class adaptor<Wrapped, meta::type_list<InputTypes...>,
                std::index_sequence<Idxs...>>
      : public adaptor_mixin<adaptor<Wrapped, meta::type_list<InputTypes...>,
                                     std::index_sequence<Idxs...>>,
                             tranform_link>,
        public output_passthrough_mixin {
   private:
    tranform_link link_;

    using adaptor_mixin_t = adaptor_mixin<adaptor, tranform_link>;

   public:
    template <class LinkDeduced, class WrappedDeduced, class InputTs>
    constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs)
        : adaptor_mixin_t(std::forward<WrappedDeduced>(wrapped)),
          link_(std::forward<LinkDeduced>(link)) {}

    template <class Self, class... Args>
    constexpr auto push_value(this Self&& self, Args&&... args) {
      return self.base().push_value(self.link_.fn_(std::forward<Args>(args)...));
    }
  };

};

inline constexpr struct transform_fn {
  template <class Fn>
  constexpr auto operator()(Fn&& fn) const {
    return tranform_link<std::remove_cvref_t<Fn>>(std::forward<Fn>(fn));
  }
} transform = {};

}  // namespace dc