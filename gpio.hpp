#pragma once

#include "main.h"

#include <cstdint>
#include <functional>
#include <unordered_map>

#ifdef HAL_GPIO_MODULE_ENABLED

namespace tutrc_harurobo_lib {

class GPIO {
public:
  GPIO(GPIO_TypeDef *port, uint16_t pin) : port_(port), pin_(pin) {}

  void write(GPIO_PinState state) { HAL_GPIO_WritePin(port_, pin_, state); }

  void toggle() { HAL_GPIO_TogglePin(port_, pin_); }

  GPIO_PinState read() { return HAL_GPIO_ReadPin(port_, pin_); }

  void set_gpio_callback(std::function<void()> &&callback) {
    gpio_callbacks_[pin_] = std::move(callback);
  }

private:
  GPIO_TypeDef *port_;
  uint16_t pin_;

  static std::unordered_map<uint16_t, std::function<void()>> gpio_callbacks_;
  friend void ::HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
};

} // namespace tutrc_harurobo_lib

#endif
