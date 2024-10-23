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

class ScopedLock {
public:
  ScopedLock(osMutexId_t mutex) : mutex_(mutex) {}

  ~ScopedLock() { osMutexRelease(mutex_); }

  bool lock(uint32_t timeout = osWaitForever) {
    return osMutexAcquire(mutex_, timeout) == osOK;
  }

private:
  osMutexId_t mutex_;
};

} // namespace tutrc_harurobo_lib
