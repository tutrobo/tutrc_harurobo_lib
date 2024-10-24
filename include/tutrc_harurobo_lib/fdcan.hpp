#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "can_base.hpp"

namespace tutrc_harurobo_lib {

class FDCAN : public CANBase {
public:
  FDCAN(FDCAN_HandleTypeDef *hfdcan, uint32_t filter_id = 0,
        uint32_t filter_mask = 0);
  bool transmit(uint32_t id, uint8_t *data, size_t size) override;
  void set_rx_callback(
      std::function<void(uint32_t, uint8_t *, size_t)> &&callback) override;

private:
  FDCAN_HandleTypeDef *hfdcan_;
  std::function<void(uint32_t, uint8_t *, size_t)> rx_callback_;

  static std::unordered_map<FDCAN_HandleTypeDef *, FDCAN *> instances_;
  friend void ::HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                                          uint32_t RxFifo0ITs);
};

} // namespace tutrc_harurobo_lib
