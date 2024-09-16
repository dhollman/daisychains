#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

#include "daisychains/concepts.h"
#include "daisychains/generator.h"
#include "daisychains/meta.h"
#include "daisychains/terminal.h"

namespace dc {

namespace impl {

template <class Link, size_t Idx>
struct link_storage {

  [[no_unique_address]] Link link;

  template <class LinkDeduced>
    requires constructible_from<Link, LinkDeduced&&>
  constexpr explicit link_storage(LinkDeduced&& in)
    : link(std::forward<LinkDeduced>(in)) {}
  
  constexpr link_storage(link_storage const&) = default;
  constexpr link_storage(link_storage&&) = default;
  constexpr link_storage& operator=(link_storage const&) = default;
  constexpr link_storage& operator=(link_storage&&) = default;
};

template <class TL, class Seq = meta::indices_for<TL>>
struct incomplete_chain_storage_impl;

template <class... Links, size_t... Idxs>
struct incomplete_chain_storage_impl<
  meta::type_list<Links...>, std::index_sequence<Idxs...>>
  : link_storage<Links, Idxs>...
{
  template <class... LinksDeduced>
    requires (constructible_from<Links, LinksDeduced&&> && ...)
  incomplete_chain_storage_impl(LinksDeduced&&... links) 
    : link_storage<Links, Idxs>(std::forward<LinksDeduced>(links))... 
  {}
};

}  // namespace impl

template <class LinksTL,
  class Generator = impl::missing_generator,
  class Terminal = impl::missing_terminal,
  class = meta::indices_for<LinksTL>>
struct incomplete_chain; 

// TODO require either terminal missing or generator missing
template <class... Links, class Generator, class Terminal, size_t... Idxs>
struct incomplete_chain<
  meta::type_list<Links...>, 
  Generator, 
  Terminal,
  std::index_sequence<Idxs...>> 
  : private impl::link_storage<Links, Idxs>...
{
 private:

  [[no_unique_address]] Generator generator_;
  [[no_unique_address]] Terminal terminal_;

  template <class GeneratorDeduced, class... LinksDeduced>
    requires std::constructible_from<Generator, GeneratorDeduced&&> &&
      std::constructible_from<Terminal, TerminalDeduced&&> &&
      (std::constructible_from<Links, LinksDeduced&&> && ...)
  incomplete_chain_storage_impl(
      GeneratorDeduced&& gen,
      TerminalDeduced&& term,
      LinksDeduced&&... links
  ) : link_storage<Links, Idxs>(std::forward<LinksDeduced>(links))...,
      generator_(std::forward<GeneratorDeduced>(generator)),
      terminal_(std::forward<TerminalDeduced>(terminal))
  {}

 public:
  // incomplete -> incomplete
  // (possibly with generator)
  template <class Self, Link NextLink> 
    requires std::same_as<Terminal, impl::missing_terminal>
  auto operator|(this Self&& self, NextLink&& next) {
    // TODO check that the next link has a compatible input category with 
    // the current last link's output
    return incomplete_chain<
        meta::type_list<Links..., std::remove_cvref_t<NextLink>>,
        Generator, Terminal>{
      std::forward_like<Self>(generator_),
      std::forward_like<Self>(
        self.template impl::link_storage<Links, Idxs>::link
      )..., std::forward<NextLink>(next)
    };
  }

  // incomplete (no generator or terminal) -> incomplete (no generator)
  template <class Self, Terminal TerminalDeduced> 
    requires std::same_as<Terminal, impl::missing_terminal>
      && std::same_as<Generator, impl::missing_generator>
  auto operator|(this Self&& self, TerminalDeduced&& next) {
    return incomplete_chain<
      type_list<Links...>, Generator, std::remove_cvref_t<TerminalDeduced>
    >{
      impl::missing_generator{},
      std::forward<TerminalDeduced>(next),
      std::forward_like<Self>(
        self.template impl::link_storage<Links, Idxs>::link
      )...
    };
  }
};

}  // namespace dc