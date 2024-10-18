#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "daisychains/concepts.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/terminal.h"

namespace dc {

////////////////////////////////////////////////////////////////////////////////
// incomplete_chain

template <class LinksTL, class Generator = impl::missing_generator,
          class Terminal = impl::missing_terminal,
          class = meta::indices_for<LinksTL>>
struct incomplete_chain;

namespace impl {

template <class T>
struct is_incomplete_chain_impl : std::false_type {};

template <class... Ts>
struct is_incomplete_chain_impl<incomplete_chain<Ts...>> : std::true_type {};

template <class Wrapped, size_t Idx>
struct link_storage {

  [[no_unique_address]] Wrapped link;

  template <class LinkDeduced>
  requires std::constructible_from<
      Wrapped, LinkDeduced&&> constexpr explicit link_storage(LinkDeduced&& in)
      : link(std::forward<LinkDeduced>(in)) {}
};

template <class TL, class Seq = meta::indices_for<TL>>
struct incomplete_chain_storage_impl;

template <class... Links, size_t... Idxs>
struct incomplete_chain_storage_impl<meta::type_list<Links...>,
                                     std::index_sequence<Idxs...>>
    : link_storage<Links, Idxs>... {
  template <class... LinksDeduced>
  requires(std::constructible_from<Links, LinksDeduced&&>&&...)
  constexpr explicit incomplete_chain_storage_impl(LinksDeduced&&... links)
      : link_storage<Links, Idxs>(std::forward<LinksDeduced>(links))... {}
};

}  // namespace impl

// TODO require either terminal missing or generator missing
template <class... Links, class Generator, class Terminal, size_t... Idxs>
struct incomplete_chain<meta::type_list<Links...>, Generator, Terminal,
                        std::index_sequence<Idxs...>>
    : private impl::link_storage<Links, Idxs>... {
 public:  // TODO make this private
  [[no_unique_address]] Generator generator_;
  [[no_unique_address]] Terminal terminal_;

  template <class GeneratorDeduced, class TerminalDeduced,
            class... LinksDeduced>
      requires std::constructible_from<Generator, GeneratorDeduced&&>&&
          std::constructible_from<Terminal, TerminalDeduced&&> &&
      (std::constructible_from<Links, LinksDeduced&&> && ...)
        constexpr incomplete_chain(GeneratorDeduced&& gen, TerminalDeduced&& term,
                           LinksDeduced&&... links)
      : impl::link_storage<Links, Idxs>(std::forward<LinksDeduced>(links))...,
  generator_(std::forward<GeneratorDeduced>(gen)),
  terminal_(std::forward<TerminalDeduced>(term)) {}

  friend struct incomplete_chain_access;

 public:
  static constexpr bool has_terminal =
      !std::is_same_v<Terminal, impl::missing_terminal>;
  static constexpr bool has_generator =
      !std::is_same_v<Terminal, impl::missing_generator>;

  template <class Self, dc::Generator GeneratorDeduced>
    requires(!has_terminal && !has_generator)
  constexpr auto with_generator(
      this Self&& self, GeneratorDeduced&& gen) {
    return incomplete_chain<meta::type_list<Links...>,
                            std::remove_cvref_t<GeneratorDeduced>,
                            impl::missing_terminal>{
        std::forward<GeneratorDeduced>(gen), impl::missing_terminal{},
        std::forward_like<Self>(
            self.template link_storage<Links, Idxs>::link)...};
  }

  // incomplete -> incomplete
  // (possibly with generator)
  template <class Self, Link NextLink>
    requires(!has_terminal)  //
  constexpr auto
      operator|(this Self&& self, NextLink&& next) {
    // TODO check that the next link has a compatible input category with
    // the current last link's output
    return incomplete_chain<
        meta::type_list<Links..., std::remove_cvref_t<NextLink>>, Generator,
        Terminal>{std::forward_like<Self>(self.generator_),
                  impl::missing_terminal{},
                  std::forward_like<Self>(
                      self.template link_storage<Links, Idxs>::link)...,
                  std::forward<NextLink>(next)};
  }

  // incomplete (no generator or terminal) -> incomplete (no generator)
  template <class Self, dc::Terminal TerminalDeduced>
  requires(!has_generator && !has_terminal)  //
  constexpr auto operator|(this Self&& self, TerminalDeduced&& next) {
    return incomplete_chain<meta::type_list<Links...>, Generator,
                            std::remove_cvref_t<TerminalDeduced>>{
        impl::missing_generator{}, std::forward<TerminalDeduced>(next),
        std::forward_like<Self>(
            self.template link_storage<Links, Idxs>::link)...};
  }

  template <class Self, dc::Terminal TerminalDeduced>
  requires(has_generator && !has_terminal)  //
      constexpr auto
      operator|(this Self&& self, TerminalDeduced&& next) {
    return incomplete_chain<meta::type_list<Links...>, Generator,
                            std::remove_cvref_t<TerminalDeduced>>{
        std::forward_like<Self>(self.generator_),
        std::forward<TerminalDeduced>(next),
        std::forward_like<Self>(
            self.template link_storage<Links, Idxs>::link)...}
        .complete_chain();
  }

  // TODO make this private?
  template <class Self>
    requires (has_generator && has_terminal)  //
  constexpr auto complete_chain(this Self&& self) {
    auto generated = Generator::get_output_types();
    auto imbued =
        [&]<class... Args>(Args&&... args) {
          return (std::forward_like<Self>(self.generator_) %= ... %=
                  std::forward<Args>(args));
        }(self.template link_storage<Links, Idxs>::link...,
          std::forward_like<Self>(self.terminal_));
    // We need the .prev() so that we don't try to construct an adaptor
    // for the terminal
    auto complete_chain = imbued.prev().construct_adaptor(
        std::forward<typename decltype(imbued)::link_t>(imbued.link));
    
    // This looks like we're doing a double move, but actually both the
    // generate() phase and the output phase need to know whether they're
    // expiring or not.
    // TODO(dhollman) write tests to make sure I'm not wrong about this
    complete_chain.generate();
    return std::forward_like<Self>(complete_chain).get_output();
  }
};

template <class T>
concept IncompleteChain =
    impl::is_incomplete_chain_impl<std::remove_cvref_t<T>>::value;

template <class T>
concept ChainWithGenerator =
    IncompleteChain<T> && std::remove_cvref_t<T>::has_generator;

template <class T>
concept ChainWithTerminal =
    IncompleteChain<T> && std::remove_cvref_t<T>::has_terminal;

// incomplete (no generator or terminal) -> incomplete (no terminal)
template <class GeneratorDeduced, class ChainDeduced>
    requires dc::Generator<GeneratorDeduced>  //
        && IncompleteChain<ChainDeduced>      //
    && (!ChainWithTerminal<ChainDeduced>)     //
constexpr auto operator|(GeneratorDeduced&& gen, ChainDeduced&& chain) {
  std::forward<ChainDeduced>(chain).with_generator(
      std::forward<GeneratorDeduced>(gen));
}

////////////////////////////////////////////////////////////////////////////////
// daisychain

}  // namespace dc