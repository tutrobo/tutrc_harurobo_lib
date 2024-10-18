#pragma once

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "utility.hpp"

namespace tutrc_harurobo_lib {

class UART {
public:
  UART(UART_HandleTypeDef *huart, size_t rx_buf_size = 64)
      : huart_(huart), buf_(rx_buf_size) {
    HAL_UART_Receive_DMA(huart_, buf_.data(), buf_.size());
  }

  size_t available() {
    size_t write_idx = buf_.size() - __HAL_DMA_GET_COUNTER(huart_->hdmarx);
    return (buf_.size() + write_idx - read_idx_) % buf_.size();
  }

  void flush() { read_idx_ = (read_idx_ + available()) % buf_.size(); }

  HAL_StatusTypeDef transmit(uint8_t *data, size_t size) {
    return HAL_UART_Transmit(huart_, data, size, HAL_MAX_DELAY);
  }

  void receive(uint8_t *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
      data[i] = buf_[read_idx_];
      read_idx_ = (read_idx_ + 1) % buf_.size();
    }
  }

private:
  UART_HandleTypeDef *huart_;
  std::vector<uint8_t> buf_;
  size_t read_idx_;
};

} // namespace tutrc_harurobo_lib

#endif
