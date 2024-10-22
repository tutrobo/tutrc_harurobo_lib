#pragma once

#include <array>

#include "uart.hpp"

namespace tutrc_harurobo_lib {

class BNO055 {
public:
  BNO055(UART *uart) : uart_(uart) {
    uint32_t start = osKernelGetTickCount();
    while (osKernelGetTickCount() - start < 500) {
      uint8_t data = 0x00;
      if (!write_reg(0x3D, &data, 1)) {
        continue;
      }
      data = 0x04;
      if (!write_reg(0x3B, &data, 1)) {
        continue;
      }
      data = 0x08;
      if (!write_reg(0x3D, &data, 1)) {
        continue;
      }
      break;
    }
  }

  void update() {
    std::array<int16_t, 3> euler;
    if (read_reg(0x1A, reinterpret_cast<uint8_t *>(euler.data()), 6)) {
      euler_ = euler;
    }
  }

  float get_euler_x() { return euler_[0] / 900.0f; }

  float get_euler_y() { return euler_[1] / 900.0f; }

  float get_euler_z() { return euler_[2] / 900.0f; }

private:
  UART *uart_;
  std::array<int16_t, 3> euler_ = {};

  bool write_reg(uint8_t addr, uint8_t *data, uint8_t size) {
    std::array<uint8_t, 4> buf = {0xAA, 0x00, addr, size};
    uart_->flush();
    if (!uart_->transmit(buf.data(), 4)) {
      return false;
    }
    if (!uart_->transmit(data, size)) {
      return false;
    }
    if (!uart_->receive(buf.data(), 2, 5)) {
      return false;
    }
    return buf[0] == 0xEE && buf[1] == 0x01;
  }

  bool read_reg(uint8_t addr, uint8_t *data, uint8_t size) {
    std::array<uint8_t, 4> buf = {0xAA, 0x01, addr, size};
    uart_->flush();
    if (!uart_->transmit(buf.data(), 4)) {
      return false;
    }
    if (!uart_->receive(buf.data(), 2, 5)) {
      return false;
    }
    if (buf[0] != 0xBB || buf[1] != size) {
      return false;
    }
    return uart_->receive(data, size, 5);
  }
};

} // namespace tutrc_harurobo_lib
