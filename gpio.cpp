#include "gpio.hpp"

#ifdef HAL_GPIO_MODULE_ENABLED

std::unordered_map<uint16_t, tutrc_harurobo_lib::GPIO *>
    tutrc_harurobo_lib::GPIO::instances_;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  auto itr = tutrc_harurobo_lib::GPIO::instances_.find(GPIO_Pin);
  if (itr != tutrc_harurobo_lib::GPIO::instances_.end()) {
    tutrc_harurobo_lib::GPIO *gpio = itr->second;
    if (gpio->gpio_callback_) {
      gpio->gpio_callback_();
    }
  }
}

#endif
