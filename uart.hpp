#pragma once

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

extern "C" int _write(int file, char *ptr, int len);

namespace tutrc_harurobo_lib {

class UART {
public:
  UART(UART_HandleTypeDef *huart, size_t queue_size = 64)
      : huart_(huart), rx_buf_(queue_size) {
    instances_[huart_] = this;
    task_handle_ = xTaskGetCurrentTaskHandle();
    rx_notify_ = xQueueCreate(1, sizeof(uint16_t));
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_buf_.data(), rx_buf_.size());
  }

  bool transmit(uint8_t *data, size_t size) {
    if (HAL_UART_Transmit_DMA(huart_, data, size) != HAL_OK) {
      return false;
    }
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    return true;
  }

  bool receive(uint8_t *data, size_t size, uint32_t timeout) {
    TimeOut_t timeout_state;
    vTaskSetTimeOutState(&timeout_state);
    while (rx_buf_count() < size) {
      uint16_t rx_write_idx;
      if (xQueueReceive(rx_notify_, &rx_write_idx, timeout) == pdTRUE) {
        rx_write_idx_ = rx_write_idx;
      }
      if (xTaskCheckForTimeOut(&timeout_state, &timeout) != pdFALSE) {
        break;
      }
    }
    if (rx_buf_count() < size) {
      return false;
    }
    for (size_t i = 0; i < size; ++i) {
      if (rx_read_idx_ == rx_buf_.size()) {
        rx_read_idx_ = 0;
      }
      data[i] = rx_buf_[rx_read_idx_++];
    }
    return true;
  }

  void flush() { rx_read_idx_ = rx_write_idx_; }

  void enable_printf() { uart_printf_ = this; }

private:
  UART_HandleTypeDef *huart_;
  TaskHandle_t task_handle_;
  QueueHandle_t rx_notify_;
  std::vector<uint8_t> rx_buf_;
  uint16_t rx_read_idx_ = 0;
  uint16_t rx_write_idx_ = 0;

  size_t rx_buf_count() {
    return (rx_buf_.size() + rx_write_idx_ - rx_read_idx_) % rx_buf_.size();
  }

  static std::unordered_map<UART_HandleTypeDef *, UART *> instances_;
  static UART *uart_printf_;
  friend void ::HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
  friend void ::HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,
                                           uint16_t Size);
  friend void ::HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
  friend int ::_write(int file, char *ptr, int len);
};

} // namespace tutrc_harurobo_lib

#endif
