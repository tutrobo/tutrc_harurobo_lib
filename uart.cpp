#include "uart.hpp"

#ifdef HAL_UART_MODULE_ENABLED

std::unordered_map<UART_HandleTypeDef *, tutrc_harurobo_lib::UART *>
    tutrc_harurobo_lib::UART::instances_;
tutrc_harurobo_lib::UART *tutrc_harurobo_lib::UART::uart_printf_ = nullptr;

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
