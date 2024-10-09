#include "gpio.hpp"

#ifdef HAL_GPIO_MODULE_ENABLED

std::unordered_map<uint16_t, std::function<void()>>
    tutrc_harurobo_lib::GPIO::gpio_callbacks_;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  auto itr = tutrc_harurobo_lib::GPIO::gpio_callbacks_.find(GPIO_Pin);
  if (itr != tutrc_harurobo_lib::GPIO::gpio_callbacks_.end()) {
    itr->second();
  }
}

#endif
