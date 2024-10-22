#include "uart.hpp"

#ifdef HAL_UART_MODULE_ENABLED

std::unordered_map<UART_HandleTypeDef *, tutrc_harurobo_lib::UART *>
    tutrc_harurobo_lib::UART::instances_;
tutrc_harurobo_lib::UART *tutrc_harurobo_lib::UART::uart_printf_ = nullptr;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  auto itr = tutrc_harurobo_lib::UART::instances_.find(huart);
  if (itr != tutrc_harurobo_lib::UART::instances_.end()) {
    tutrc_harurobo_lib::UART *uart = itr->second;
    BaseType_t yield = pdFALSE;
    xTaskNotifyFromISR(uart->task_handle_, 0, eNoAction, &yield);
    portYIELD_FROM_ISR(yield);
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  auto itr = tutrc_harurobo_lib::UART::instances_.find(huart);
  if (itr != tutrc_harurobo_lib::UART::instances_.end()) {
    tutrc_harurobo_lib::UART *uart = itr->second;
    BaseType_t yield = pdFALSE;
    xTaskNotifyFromISR(uart->task_handle_, Size, eSetValueWithOverwrite,
                       &yield);
    portYIELD_FROM_ISR(yield);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  auto itr = tutrc_harurobo_lib::UART::instances_.find(huart);
  if (itr != tutrc_harurobo_lib::UART::instances_.end()) {
    tutrc_harurobo_lib::UART *uart = itr->second;
    HAL_UART_Abort(huart);
    uart->rx_read_idx_ = 0;
    uart->rx_write_idx_ = 0;
    HAL_UARTEx_ReceiveToIdle_DMA(huart, uart->rx_buf_.data(),
                                 uart->rx_buf_.size());
  }
}

int _write(int, char *ptr, int len) {
  if (tutrc_harurobo_lib::UART::uart_printf_) {
    HAL_UART_Transmit_DMA(tutrc_harurobo_lib::UART::uart_printf_->huart_,
                          reinterpret_cast<uint8_t *>(ptr), len);
  }
  return len;
}

#endif
