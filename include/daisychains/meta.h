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
  : std::type_identity<std::index_sequence_for<Ts...>> {};

}  // namespace impl

namespace meta {

template <class... Links>
struct type_list {};

template <class...>
struct optional_type;

template <class T>
struct optional_type<T> {};

template <>
struct optional_type<> {};

template <class TL>
using indices_for = typename dc::impl::indices_for_impl<TL>::type;

}  // namespace meta

}  // namespace dc