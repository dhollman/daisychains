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
    return imbued_link<std::remove_cvref_t<LinkDeduced>, next_inputs, Derived>{
        std::forward<LinkDeduced>(next),
        std::move(*static_cast<Derived*>(this))};
  }

  template <dc::Terminal TerminalDeduced>
  constexpr auto imbue_next(TerminalDeduced&& next) {
    using next_inputs = decltype(Derived::get_output_types());
    return imbued_link<std::remove_cvref_t<TerminalDeduced>, next_inputs, Derived>{
        std::forward<TerminalDeduced>(next),
        std::move(*static_cast<Derived*>(this))};
  }

  // TODO make this private?
  // This is just so that we can fold over something easily
  template <dc::Link LinkDeduced>
  constexpr auto operator%=(LinkDeduced&& next) {
    return imbue_next(std::forward<LinkDeduced>(next));
  }

  // TODO this can be combined with the above, but I'm not sure I won't want it separate later
  template <dc::Terminal TerminalDeduced>
  constexpr auto operator%=(TerminalDeduced&& next) {
    return imbue_next(std::forward<TerminalDeduced>(next));
  }

  template <dc::Link LinkDeduced>
  constexpr auto operator|(LinkDeduced&& next) {
    return incomplete_chain<meta::type_list<std::remove_cvref_t<LinkDeduced>>,
                            Derived, impl::missing_terminal>{
        std::move(*static_cast<Derived*>(this)), impl::missing_terminal{},
        std::forward<LinkDeduced>(next)};
  }

  template <dc::Terminal TerminalDeduced>
  constexpr auto operator|(TerminalDeduced&& next) {
    return incomplete_chain<meta::type_list<>, Derived,
                            std::remove_cvref_t<TerminalDeduced>>{
        std::move(*static_cast<Derived*>(this)),
        std::forward<TerminalDeduced>(next)}
        .complete_chain();
  }

  template <class Self, class CompleteLinkDeduced>
  constexpr auto construct_adaptor(this Self&& self,
                                   CompleteLinkDeduced&& wrapped) {
    return typename Derived::template adaptor<
        std::remove_cvref_t<CompleteLinkDeduced>>(
        std::forward<CompleteLinkDeduced>(wrapped), std::forward<Self>(self));
  }
};

}  // namespace impl

}  // namespace dc