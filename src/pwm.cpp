#include "tutrc_harurobo_lib/pwm.hpp"

#ifdef HAL_TIM_MODULE_ENABLED

namespace tutrc_harurobo_lib {

PWM::PWM(TIM_HandleTypeDef *htim, uint32_t channel)
    : htim_(htim), channel_(channel) {
  if (HAL_TIM_PWM_Start(htim_, channel_) != HAL_OK) {
    Error_Handler();
  }
}

uint32_t PWM::get_compare() { return __HAL_TIM_GET_COMPARE(htim_, channel_); }

void PWM::set_compare(uint32_t compare) {
  __HAL_TIM_SET_COMPARE(htim_, channel_, compare);
}

} // namespace tutrc_harurobo_lib

#endif
