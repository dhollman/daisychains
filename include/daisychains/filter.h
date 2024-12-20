#pragma once

#include "daisychains/adaptor.h"
#include "daisychains/concepts.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/output_passthrough.h"
#include "daisychains/push_result.h"

namespace dc {

template <class Fn>
class filter_link {
 private:
  Fn fn_;

 public:
  using link_t = filter_link;

  template <helpers::deduced_reference_for<Fn> FnDeduced>
  constexpr explicit filter_link(FnDeduced&& fn)
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
                             filter_link>,
        public output_passthrough_mixin {
   private:
    filter_link link_;

    using adaptor_mixin_t = adaptor_mixin<adaptor, filter_link>;

    // TODO complete link needs to include terminals potentially
    // static_assert(dc::CompleteLink<Wrapped>,
    //               "Wrapped link must be a complete link");

   public:
    template <class LinkDeduced, class WrappedDeduced, class InputTs>
    constexpr adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs)
        : adaptor_mixin_t(std::forward<WrappedDeduced>(wrapped)),
          link_(std::forward<LinkDeduced>(link)) {}

    template <class Self, class... Args>
    constexpr auto push_value(this Self&& self, Args&&... args) {
      if (self.link_.fn_(args...)) {
        return self.base().push_value(std::forward<Args>(args)...);
      }
      return push_result::keep_iterating();
    }
  };
};

inline constexpr struct filter_fn {
  template <class Fn>
  constexpr auto operator()(Fn&& fn) const {
    return filter_link<std::remove_cvref_t<Fn>>(std::forward<Fn>(fn));
  }
} filter = {};

}  // namespace dc