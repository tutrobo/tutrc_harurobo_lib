#include "../uart.hpp"

#ifdef HAL_UART_MODULE_ENABLED

namespace tutrc_harurobo_lib {

std::unordered_map<UART_HandleTypeDef *, UART *> UART::instances_;
UART *UART::uart_printf_ = nullptr;

UART::UART(UART_HandleTypeDef *huart, size_t rx_buf_size)
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

bool UART::transmit(uint8_t *data, size_t size) {
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

bool UART::receive(uint8_t *data, size_t size, uint32_t timeout) {
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

void UART::flush() { rx_buf_head_ = rx_buf_tail_; }

void UART::enable_printf() { uart_printf_ = this; }

size_t UART::rx_buf_count() {
  return (rx_buf_.size() + rx_buf_tail_ - rx_buf_head_) % rx_buf_.size();
}

} // namespace tutrc_harurobo_lib

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  auto itr = tutrc_harurobo_lib::UART::instances_.find(huart);
  if (itr != tutrc_harurobo_lib::UART::instances_.end()) {
    tutrc_harurobo_lib::UART *uart = itr->second;
    osSemaphoreRelease(uart->tx_sem_);
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  auto itr = tutrc_harurobo_lib::UART::instances_.find(huart);
  if (itr != tutrc_harurobo_lib::UART::instances_.end()) {
    tutrc_harurobo_lib::UART *uart = itr->second;
    BaseType_t yield = pdFALSE;
    xQueueOverwriteFromISR(uart->rx_queue_, &Size, &yield);
    portYIELD_FROM_ISR(yield);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  auto itr = tutrc_harurobo_lib::UART::instances_.find(huart);
  if (itr != tutrc_harurobo_lib::UART::instances_.end()) {
    tutrc_harurobo_lib::UART *uart = itr->second;
    HAL_UART_Abort(huart);
    uart->rx_buf_head_ = 0;
    uart->rx_buf_tail_ = 0;
    HAL_UARTEx_ReceiveToIdle_DMA(huart, uart->rx_buf_.data(),
                                 uart->rx_buf_.size());
  }
}

int _write(int, char *ptr, int len) {
  if (tutrc_harurobo_lib::UART::uart_printf_) {
    tutrc_harurobo_lib::UART::uart_printf_->transmit(
        reinterpret_cast<uint8_t *>(ptr), len);
  }
  return len;
}

#endif
