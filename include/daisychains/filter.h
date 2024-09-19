#pragma once

namespace dc {

template <class Fn>
class filter_link {
 private:
  Fn fn_;

 public:
  using link_t = filter_link;

  template <helpers::deduced_reference_for<Fn> FnDeduced>
  explicit filter_link(FnDeduced&& fn) : fn_(std::forward<FnDeduced>(fn)) {}

  template <class... Ts>
  static constexpr auto get_output_types_from_input_types(
      meta::type_list<Ts...>) {
    return meta::type_list<std::invoke_result_t<Fn, Ts...>>{};
  }

  template <class Wrapped, class InputTypes,
            class = meta::indices_for<InputTypes>>
  class adaptor;

  template <class Wrapped, class... InputTypes, size_t... Idxs>
  class adaptor<Wrapped, meta::type_list<InputTypes...>,
                std::index_sequence<Idxs...>> {
   private:
    filter_link link_;
    Wrapped wrapped_;

   public:
    using link_t = filter_link;

    template <class LinkDeduced, class WrappedDeduced, class InputTs>
    adaptor(LinkDeduced&& link, WrappedDeduced&& wrapped, InputTs)
        : link_(std::forward<LinkDeduced>(link)),
          wrapped_(std::forward<WrappedDeduced>(wrapped)) {}

    template <class... Args>
    constexpr auto push_value(Args&&... args) {
      return wrapped_.push_value(link_.fn_(std::forward<Args>(args)...));
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