#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "can.hpp"
#include "utility.hpp"

namespace tutrc_harurobo_lib {

class C610 {
public:
  enum class ID {
    ID1,
    ID2,
    ID3,
    ID4,
    ID5,
    ID6,
    ID7,
    ID8,
  };

  C610(CAN &can) : can_(can) {
    can_.set_rx_callback([this](uint32_t id, uint8_t *data, size_t size) {
      for (size_t i = 0; i < 8; ++i) {
        if (id == 0x201 + i) {
          rpm_[i] = static_cast<int16_t>(data[2] << 8 | data[3]);
          break;
        }
      }
    });
  }

  int16_t get_rpm(ID id) { return rpm_[to_underlying(id)]; }

  float get_rps(ID id) { return get_rpm(id) / 60.0f; }

  // -10000 ~ 10000 mA の間で指定
  void set_current(ID id, int16_t current) {
    current_[to_underlying(id)] = current;
  }

  void transmit() {
    std::array<uint8_t, 8> data;
    for (size_t i = 0; i < 4; ++i) {
      data[i * 2] = current_[i] >> 8;
      data[i * 2 + 1] = current_[i];
    }
    can_.transmit(0x200, data.data(), data.size());
    for (size_t i = 0; i < 4; ++i) {
      data[i * 2] = current_[i + 4] >> 8;
      data[i * 2 + 1] = current_[i + 4];
    }
    can_.transmit(0x1FF, data.data(), data.size());
  }

private:
  CAN &can_;
  std::array<int16_t, 8> rpm_ = {};
  std::array<int16_t, 8> current_ = {};
};

} // namespace tutrc_harurobo_lib
