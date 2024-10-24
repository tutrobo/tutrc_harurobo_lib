#pragma once

#include <type_traits>

namespace tutrc_harurobo_lib {

template <class T>
constexpr std::underlying_type_t<T> to_underlying(T value) noexcept {
  return static_cast<std::underlying_type_t<T>>(value);
}

} // namespace tutrc_harurobo_lib
