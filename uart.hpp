#pragma once

#include "main.h"

#ifndef HAL_UART_MODULE_ENABLED

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
  UART(UART_HandleTypeDef *huart, size_t rx_buf_size = 64)
      : huart_(huart), rx_buf_(rx_buf_size) {
    instances_[huart_] = this;
    tx_mutex_ = osMutexNew(nullptr);
    rx_mutex_ = osMutexNew(nullptr);
    tx_sem_ = osSemaphoreNew(1, 0, nullptr);
    rx_queue_ = xQueueCreate(1, sizeof(uint16_t));
    HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_buf_.data(), rx_buf_.size());
  }

  bool transmit(uint8_t *data, size_t size) {
    ScopedLock lock(tx_mutex_);
    if (!lock.lock(osWaitForever)) {
      return false;
    }
    if (HAL_UART_Transmit_DMA(huart_, data, size) != HAL_OK) {
      return false;
    }
    return osSemaphoreAcquire(tx_sem_, osWaitForever) == osOK;
  }

  bool receive(uint8_t *data, size_t size, uint32_t timeout) {
    ScopedLock lock(rx_mutex_);
    if (timeout == 0) {
      if (!lock.lock(timeout)) {
        return false;
      }
      uint16_t rx_write_idx;
      if (xQueueReceive(rx_queue_, &rx_write_idx, timeout) == pdTRUE) {
        rx_write_idx_ = rx_write_idx;
      }
    } else {
      TimeOut_t timeout_state;
      vTaskSetTimeOutState(&timeout_state);
      if (!lock.lock(timeout)) {
        return false;
      }
      while (rx_buf_count() < size) {
        if (xTaskCheckForTimeOut(&timeout_state, &timeout) != pdFALSE) {
          break;
        }
        uint16_t rx_write_idx;
        if (xQueueReceive(rx_queue_, &rx_write_idx, timeout) == pdTRUE) {
          rx_write_idx_ = rx_write_idx;
        }
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
  osMutexId_t tx_mutex_;
  osMutexId_t rx_mutex_;
  osSemaphoreId_t tx_sem_;
  QueueHandle_t rx_queue_;
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
