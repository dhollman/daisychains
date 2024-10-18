#pragma once

#include <type_traits>
#include <utility>
#include "daisychains/concepts.h"
#include "daisychains/fwd.h"

namespace dc {

namespace impl {

struct after_terminal {};

// TODO figure out how to do this without a move
template <class Wrapped, class InputTypes, class Prev>
class imbued_link : Prev {
 public:
  template <
    helpers::deduced_reference_for<Wrapped> WrappedDeduced,
    helpers::deduced_reference_for<Prev> PrevDeduced>
  constexpr imbued_link(WrappedDeduced&& wrapped, PrevDeduced&& prev)
      : Prev(std::forward<PrevDeduced>(prev)),
        link(std::forward<Wrapped>(wrapped)) {}

  // Link can be either an lvalue ref or not because of ref collapsing
  // imbued_link only ever exists on the stack
  Wrapped link;

  using this_t = imbued_link;
  using link_t = Wrapped;

  template <class Self>
  constexpr decltype(auto) prev(this Self&& self) {
    // TODO propagate const
    return std::forward_like<Self>(static_cast<Prev&>(self));
  }

  template <dc::Link LinkDeduced>
  constexpr auto imbue_next(LinkDeduced&& next) && {
    using next_inputs =
        decltype(Wrapped::get_output_types_from_input_types(InputTypes{}));
    return imbued_link<std::remove_cvref_t<LinkDeduced>, next_inputs, this_t>{
        std::forward<LinkDeduced>(next), std::move(*this)};
  }

  template <dc::Terminal TerminalDeduced>
  constexpr auto imbue_next(TerminalDeduced&& next) && {
    using next_inputs =
        decltype(Wrapped::get_output_types_from_input_types(InputTypes{}));
    auto imbued_terminal =
        typename TerminalDeduced::template imbued<next_inputs>(
            std::forward<TerminalDeduced>(next));
    return imbued_link<decltype(imbued_terminal), next_inputs, this_t>{
        std::move(imbued_terminal), std::move(*this)};
  }

  // Can be customized, but by default just construct the ::adaptor template
  // input_types, link, and the wrapped link
  template <class WrappedCompleteLinkDeduced>
  constexpr auto construct_adaptor(WrappedCompleteLinkDeduced&& wrapped) {
    // TODO double check perfect forwarding here
    return static_cast<Prev>(*this).construct_adaptor(
      typename Wrapped::template adaptor<
          std::remove_cvref_t<WrappedCompleteLinkDeduced>, InputTypes>(
          std::forward<Wrapped>(link),
          std::forward<WrappedCompleteLinkDeduced>(wrapped), InputTypes{})
    );
  }

  // TODO make this private?
  // This is just so that we can fold over something easily
  template <class Self, class LinkOrTerminalDeduced>
      requires dc::Link<LinkOrTerminalDeduced> || //
        dc::Terminal<LinkOrTerminalDeduced>       //
  constexpr auto operator%=(this Self&& self, LinkOrTerminalDeduced&& next) {
    return std::forward<Self>(self).imbue_next(
        std::forward<LinkOrTerminalDeduced>(next));
  }

  template <class CompleteLinkDeduced>
  constexpr auto construct_chain(CompleteLinkDeduced&& to_be_wrapped) {
    if constexpr (Generator<Prev>) {
      return prev().construct_adaptor(
          std::forward<Wrapped>(link).construct_adaptor(
              std::forward<CompleteLinkDeduced>(to_be_wrapped)));
    } else {
      return prev().construct_chain(
          std::forward<Wrapped>(link).construct_adaptor(
              std::forward<CompleteLinkDeduced>(to_be_wrapped)));
    }
  }
};

template <class Derived>
class link_base {
 public:
  template <class InputTypes>  // InputTypes is a type_list
  static constexpr auto get_output_types_from_input_types(InputTypes tl) {
    // TODO add this to the link concept
    return Derived::get_output_types_from_input_types(tl);
  }
};

}  // namespace impl

}  // namespace dc