#pragma once

#include "main.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include <cstdint>

namespace tutrc_harurobo_lib {

class PWM {
public:
  PWM(TIM_HandleTypeDef *htim, uint32_t channel)
      : htim_(htim), channel_(channel) {
    HAL_TIM_PWM_Start(htim_, channel_);
  }

  uint32_t get_compare() { return __HAL_TIM_GET_COMPARE(htim_, channel_); }

  void set_compare(uint32_t compare) {
    __HAL_TIM_SET_COMPARE(htim_, channel_, compare);
  }

private:
  TIM_HandleTypeDef *htim_;
  uint32_t channel_;
};

} // namespace tutrc_harurobo_lib

#endif
