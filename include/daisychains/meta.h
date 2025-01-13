#pragma once

#include <type_traits>
#include <utility>

#include "daisychains/fwd.h"

namespace dc {

namespace impl {

template <class TL>
struct indices_for_impl;

template <class... Links>
struct indices_for_impl<meta::type_list<Links...>>
  : std::type_identity<std::index_sequence_for<Links...>> {};

}  // namespace impl

namespace meta {

// Basically std::type_identity, but specific to this library to 
// avoid any potential conflicts where we actually want a std::type_identity
// (in theory at least; also because I'm lazy)
// TODO use this with static member functions and deducing this to avoid CRTP?
template <class T>
struct ty {
  using type = T;
};

template <class... Links>
struct type_list {};

template <class...>
struct optional_type;

template <class T>
struct optional_type<T> {
  using type = T;
};

template <>
struct optional_type<> {};

template <class TL>
using indices_for = typename dc::impl::indices_for_impl<TL>::type;

}  // namespace meta

}  // namespace dc