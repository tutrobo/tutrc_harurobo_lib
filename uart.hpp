#pragma once

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

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

    if (HAL_UARTEx_ReceiveToIdle_DMA(huart_, rx_buf_.data(), rx_buf_.size()) !=
        HAL_OK) {
      Error_Handler();
    }
  }

  bool transmit(uint8_t *data, size_t size) {
    if (osMutexAcquire(tx_mutex_, osWaitForever) != osOK) {
      return false;
    }
    if (HAL_UART_Transmit_DMA(huart_, data, size) != HAL_OK) {
      osMutexRelease(tx_mutex_);
      return false;
    }
    osSemaphoreAcquire(tx_sem_, osWaitForever);
    osMutexRelease(tx_mutex_);
    return true;
  }

  bool receive(uint8_t *data, size_t size, uint32_t timeout) {
    if (timeout == 0) {
      if (osMutexAcquire(rx_mutex_, 0) != osOK) {
        return false;
      }
      uint16_t rx_write_idx;
      if (xQueueReceive(rx_queue_, &rx_write_idx, 0) == pdTRUE) {
        rx_buf_tail_ = rx_write_idx;
      }
    } else {
      TimeOut_t timeout_state;
      vTaskSetTimeOutState(&timeout_state);
      if (osMutexAcquire(rx_mutex_, timeout) != osOK) {
        osMutexRelease(rx_mutex_);
        return false;
      }
      while (rx_buf_count() < size) {
        if (xTaskCheckForTimeOut(&timeout_state, &timeout) != pdFALSE) {
          break;
        }
        uint16_t rx_write_idx;
        if (xQueueReceive(rx_queue_, &rx_write_idx, timeout) == pdTRUE) {
          rx_buf_tail_ = rx_write_idx;
        }
      }
    }

    if (rx_buf_count() < size) {
      osMutexRelease(rx_mutex_);
      return false;
    }
    for (size_t i = 0; i < size; ++i) {
      if (rx_buf_head_ == rx_buf_.size()) {
        rx_buf_head_ = 0;
      }
      data[i] = rx_buf_[rx_buf_head_++];
    }
    osMutexRelease(rx_mutex_);
    return true;
  }

  void flush() { rx_buf_head_ = rx_buf_tail_; }

  void enable_printf() { uart_printf_ = this; }

private:
  UART_HandleTypeDef *huart_;
  osMutexId_t tx_mutex_;
  osMutexId_t rx_mutex_;
  osSemaphoreId_t tx_sem_;
  QueueHandle_t rx_queue_;
  std::vector<uint8_t> rx_buf_;
  uint16_t rx_buf_head_ = 0;
  uint16_t rx_buf_tail_ = 0;

  size_t rx_buf_count() {
    return (rx_buf_.size() + rx_buf_tail_ - rx_buf_head_) % rx_buf_.size();
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
