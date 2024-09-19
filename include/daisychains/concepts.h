#pragma once

#include "daisychains/fwd.h"

namespace dc {

template <class T>
concept Link = std::same_as<typename std::remove_cvref_t<T>::link_t,
                            std::remove_cvref_t<T>>;

template <class T>
concept PushResult = true;  // TODO

template <class T>
concept CompleteLink =
    Link<typename std::remove_cvref_t<T>::link_t> &&
    requires(T && t, typename std::remove_cvref_t<T>::input_types types) {
  []<typename... Ts>(meta::type_list<Ts...>) requires requires(Ts & ... ts) {
    { t.push_value(ts...) } -> PushResult;
  } {}(types);
};

template <class T>
concept Generator = std::same_as<typename std::remove_cvref_t<T>::generator_t,
                                 std::remove_cvref_t<T>>;

template <class T>
concept Terminal = std::same_as<typename std::remove_cvref_t<T>::terminal_t,
                                std::remove_cvref_t<T>>;

namespace helpers {

template <class T, class Normal>
concept deduced_reference_for = std::same_as<std::remove_cvref_t<T>, Normal>;

}  // namespace helpers

}  // namespace dc