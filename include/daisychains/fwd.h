#pragma once

namespace dc {

namespace meta {

template <class... Links>
struct type_list;

}  // namespace meta

namespace impl {

struct missing_generator {};
struct missing_terminal {};

template <class Derived>
class link_base;

template <class Derived>
class generator_base;

template <class Derived>
class terminal_base;

}  // namespace impl

}  // namespace dc