#pragma once

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include <array>
#include <cstddef>
#include <cstdint>

#include "utility.hpp"

namespace tutrc_harurobo_lib {

class PS3 {
public:
  enum class Axis {
    LEFT_X,
    LEFT_Y,
    RIGHT_X,
    RIGHT_Y,
  };

  enum class Key {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    TRIANGLE,
    CROSS,
    CIRCLE,
    SQUARE = 8,
    L1,
    L2,
    R1,
    R2,
    START,
    SELECT,
  };

  PS3(UART_HandleTypeDef *huart) : huart_(huart) {
    HAL_UART_Receive_DMA(huart_, buf_.data(), buf_.size());
  }

  void update() {
    keys_prev_ = keys_;

    uint8_t checksum = 0;

    for (size_t i = 0; i < 8; ++i) {
      if (buf_[i] == 0x80) {
        for (size_t j = 1; j < 7; ++j) {
          checksum += buf_[(i + j) % 8];
        }
        if ((checksum & 0x7F) == buf_[(i + 7) % 8]) {
          keys_ = (buf_[(i + 1) % 8] << 8) | buf_[(i + 2) % 8];
          if ((keys_ & 0x03) == 0x03) {
            keys_ &= ~0x03;
            keys_ |= 1 << 13;
          }
          if ((keys_ & 0x0C) == 0x0C) {
            keys_ &= ~0x0C;
            keys_ |= 1 << 14;
          }

          for (size_t j = 0; j < 4; ++j) {
            axes_[j] = ((float)buf_[(i + j + 3) % 8] - 64) / 64;
          }
        }
        return;
      }
    }
  }

  float get_axis(Axis axis) { return axes_[to_underlying(axis)]; }

  bool get_key(Key key) { return (keys_ & (1 << to_underlying(key))) != 0; }

  bool get_key_down(Key key) {
    return ((keys_ ^ keys_prev_) & keys_ & (1 << to_underlying(key))) != 0;
  }

  bool get_key_up(Key key) {
    return ((keys_ ^ keys_prev_) & keys_prev_ & (1 << to_underlying(key))) != 0;
  }

private:
  UART_HandleTypeDef *huart_;
  std::array<uint8_t, 8> buf_ = {};
  std::array<float, 4> axes_ = {};
  uint16_t keys_;
  uint16_t keys_prev_;
};

} // namespace tutrc_harurobo_lib

#endif
