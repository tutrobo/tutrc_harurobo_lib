#include "stdio.hpp"

#ifdef HAL_UART_MODULE_ENABLED

static UART_HandleTypeDef *huart_printf = nullptr;

void tutrc_harurobo_lib::enable_printf(UART_HandleTypeDef *huart) {
  huart_printf = huart;
}

extern "C" int _write(int, char *ptr, int len) {
  if (huart_printf) {
    HAL_UART_Transmit(huart_printf, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  }
  return len;
}

#endif
