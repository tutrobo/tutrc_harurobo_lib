#pragma once

#include <algorithm>
#include <type_traits>

namespace tutrc_harurobo_lib {

template <class T>
constexpr std::underlying_type_t<T> to_underlying(T value) noexcept {
  return static_cast<std::underlying_type_t<T>>(value);
}

template <class T>
constexpr const T &clamp(const T &v, const T &low, const T &high) {
  return std::min(std::max(v, low), high);
}

} // namespace tutrc_harurobo_lib
