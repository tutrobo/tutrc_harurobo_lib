#pragma once

#include "main.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace tutrc_harurobo_lib {

class Timer {
public:
  Timer(TIM_HandleTypeDef *htim) : htim_(htim) { HAL_TIM_Base_Start_IT(htim_); }

  uint32_t get_counter() { return __HAL_TIM_GET_COUNTER(htim_); }

  void set_counter(uint32_t counter) { __HAL_TIM_SET_COUNTER(htim_, counter); }

  void set_timer_callback(std::function<void()> &&callback) {
    timer_callbacks_[htim_] = std::move(callback);
  }

private:
  TIM_HandleTypeDef *htim_;

  static std::unordered_map<TIM_HandleTypeDef *, std::function<void()>>
      timer_callbacks_;
  friend void ::HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
};

} // namespace tutrc_harurobo_lib

#endif
