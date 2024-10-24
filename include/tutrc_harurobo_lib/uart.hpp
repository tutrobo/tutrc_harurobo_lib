#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "queue.h"
#include "task.h"

#include "utility.hpp"

extern "C" int _write(int file, char *ptr, int len);

namespace tutrc_harurobo_lib {

class UART {
public:
  UART(UART_HandleTypeDef *huart, size_t rx_buf_size = 64);
  bool transmit(uint8_t *data, size_t size);
  bool receive(uint8_t *data, size_t size, uint32_t timeout);
  void flush();
  void enable_printf();

private:
  UART_HandleTypeDef *huart_;
  osMutexId_t tx_mutex_;
  osMutexId_t rx_mutex_;
  osSemaphoreId_t tx_sem_;
  QueueHandle_t rx_queue_;
  std::vector<uint8_t> rx_buf_;
  uint16_t rx_buf_head_ = 0;
  uint16_t rx_buf_tail_ = 0;

  size_t rx_buf_count();

  static std::unordered_map<UART_HandleTypeDef *, UART *> instances_;
  static UART *uart_printf_;
  friend void ::HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
  friend void ::HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,
                                           uint16_t Size);
  friend void ::HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
  friend int ::_write(int file, char *ptr, int len);
};

} // namespace tutrc_harurobo_lib
