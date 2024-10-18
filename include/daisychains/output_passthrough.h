#pragma once

#include "daisychains/fwd.h"

namespace dc {

struct output_passthrough_mixin {
  // TODO(dhollman) move these musings to a markdown file somewhere
  // TODO(dhollman) figure out if we want to perfect forward to the
  //                return value here (probably not, but I guess it
  //                could be important for non-movable output types
  //                somehow?).  Probably the right thing to do is to
  //                perfect forward if the return of base().get_output()
  //                is a reference, and not otherwise.  I don't really
  //                care about optimizing for expensive-to-move types
  //                throughout this library, and especially not here 
  //                where we have mandatory RVO to handle that anyway.
  template <class Self>
  constexpr auto get_output(this Self&& self) {
    return std::forward<Self>(self).base().get_output();
  }
};

}  // namespace dc