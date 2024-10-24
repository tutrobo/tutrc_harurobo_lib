#pragma once

#include <algorithm>
#include <type_traits>

#include "cmsis_os2.h"

namespace tutrc_harurobo_lib {

template <class T>
constexpr std::underlying_type_t<T> to_underlying(T value) noexcept {
  return static_cast<std::underlying_type_t<T>>(value);
}

template <class T>
constexpr const T &clamp(const T &v, const T &low, const T &high) {
  return std::min(std::max(v, low), high);
}

template <class T, class U = T>
osThreadId_t create_thread(void (*func)(T), U argument,
                           const osThreadAttr_t &attr) {
  return osThreadNew(reinterpret_cast<osThreadFunc_t>(func), argument, &attr);
}

} // namespace tutrc_harurobo_lib
