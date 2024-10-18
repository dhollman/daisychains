#pragma once

#include <utility>

#include "daisychains/concepts.h"
#include "daisychains/fwd.h"
#include "daisychains/meta.h"
#include "daisychains/push_result.h"

namespace dc {

namespace impl {

template <class Wrapped>
struct wrapped_link_storage_mixin {
 private:
  Wrapped wrapped_;

 protected:
  template <helpers::deduced_reference_for<Wrapped> LinkDeduced>
  explicit constexpr wrapped_link_storage_mixin(LinkDeduced&& link)
      : wrapped_(std::forward<LinkDeduced>(link)) {}

 public:
  // TODO(dhollman) decide if I like this name enough from ranges
  //                to keep it
  template <class Self>
  constexpr decltype(auto) base(this Self&& self) {
    return std::forward_like<Self>(self.wrapped_);
  }
};

}  // namespace impl

template <class Derived, class LinkOrGenerator>
class adaptor_mixin;

// Generator version
template <template <class...> class DerivedTemplate, class Wrapped,
          class... Ignored, dc::Generator Gen>
class adaptor_mixin<DerivedTemplate<Wrapped, Ignored...>, Gen>
    : public impl::wrapped_link_storage_mixin<Wrapped> {
 private:
  using link_storage_mixin_t = impl::wrapped_link_storage_mixin<Wrapped>;

 public:
  using generator_t = Gen;

  using link_storage_mixin_t::link_storage_mixin_t;
};

// Link version
template <template <class...> class DerivedTemplate, class Wrapped,
          class... InputTypes, dc::Link Lnk, class... Ignored>
class adaptor_mixin<
    DerivedTemplate<Wrapped, meta::type_list<InputTypes...>, Ignored...>, Lnk>
    : public impl::wrapped_link_storage_mixin<Wrapped> {
 private:
  using link_storage_mixin_t = impl::wrapped_link_storage_mixin<Wrapped>;

 public:
  using link_t = Lnk;
  using input_types = meta::type_list<InputTypes...>;

  using link_storage_mixin_t::link_storage_mixin_t;

  template <class Self>
  constexpr auto push_stop(this Self&& self, push_result result) {
    return self.base().push_stop(result);
  }
};

}  // namespace dc