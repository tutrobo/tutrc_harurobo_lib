#pragma once

#include <array>

#include "uart.hpp"

namespace tutrc_harurobo_lib {

class BNO055 {
public:
  BNO055(UART &uart) : uart_(uart) {
    int start = HAL_GetTick();
    while (HAL_GetTick() - start < 500) {
      uint8_t data = 0x08;
      write(0x3D, &data, 1);
    }
  }

  void update() { read(0x1A, reinterpret_cast<uint8_t *>(euler_.data()), 6); }

  float get_euler_x() { return to_rad(euler_[0]); }

  float get_euler_y() { return to_rad(euler_[1]); }

  float get_euler_z() { return to_rad(euler_[2]); }

private:
  UART &uart_;
  std::array<int16_t, 3> euler_ = {};

  bool write(uint8_t addr, uint8_t *data, size_t size) {
    std::array<uint8_t, 4> buf = {0xAA, 0x00, addr, size};
    uart_.flush();
    uart_.transmit(buf.data(), 4);
    uart_.transmit(data, size);
    HAL_Delay(5);
    if (uart_.available() < 2) {
      return false;
    }
    uart_.receive(buf.data(), 2);
    return true; // TODO: ステータスチェック
  }

  bool read(uint8_t addr, uint8_t *data, size_t size) {
    std::array<uint8_t, 4> buf = {0xAA, 0x01, addr, size};
    uart_.flush();
    uart_.transmit(buf.data(), 4);
    HAL_Delay(5);
    if (uart_.available() < 2) {
      return false;
    }
    uart_.receive(buf.data(), 2);
    if (buf[0] == 0xEE || uart_.available() < size) {
      return false;
    }
    uart_.receive(data, size);
    return true;
  }

  float to_rad(int16_t x) { return x * 2 * M_PI / (360 * 16); }
};

} // namespace tutrc_harurobo_lib
