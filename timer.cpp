#include "timer.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

std::unordered_map<TIM_HandleTypeDef *, std::function<void()>>
    tutrc_harurobo_lib::Timer::timer_callbacks_;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  auto itr = tutrc_harurobo_lib::Timer::timer_callbacks_.find(htim);
  if (itr != tutrc_harurobo_lib::Timer::timer_callbacks_.end()) {
    itr->second();
  }
}

#endif
