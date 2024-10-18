#include "stdio.hpp"

#ifdef HAL_UART_MODULE_ENABLED

static inline UART_HandleTypeDef *huart_stdin = nullptr;

void enable_stdin(UART_HandleTypeDef *huart) { huart_stdin = huart; }

extern "C" int _write(int, char *ptr, int len) {
  if (huart_stdin) {
    HAL_UART_Transmit(huart_stdin, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  }
  return len;
}

#endif
