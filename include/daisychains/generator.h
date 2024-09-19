#pragma once

#include "daisychains/chain.h"
#include "daisychains/fwd.h"
#include "daisychains/link.h"

namespace dc {

namespace impl {

template <class Derived>
class generator_base {
 public:
  static constexpr auto get_output_types() requires requires {
    typename Derived::output_type;
  }
  { return meta::type_list<typename Derived::output_type>{}; }

  template <dc::Link LinkDeduced>
  constexpr auto imbue_next(LinkDeduced&& next) {
    using next_inputs = decltype(Derived::get_output_types());
    return imbued_link<LinkDeduced, next_inputs, Derived>{
        std::forward<LinkDeduced>(next),
        std::move(*static_cast<Derived*>(this))};
  }

  // TODO make this private?
  // This is just so that we can fold over something easily
  template <dc::Link LinkDeduced>
  constexpr auto operator%=(LinkDeduced&& next) {
    return imbue_next(std::forward<LinkDeduced>(next));
  }

  template <dc::Link LinkDeduced>
  constexpr auto operator|(LinkDeduced&& next) {
    return incomplete_chain<meta::type_list<std::remove_cvref_t<LinkDeduced>>,
                            Derived, impl::missing_terminal>{
        std::move(*static_cast<Derived*>(this)), impl::missing_terminal{},
        std::forward<LinkDeduced>(next)};
  }
};

}  // namespace impl

}  // namespace dc