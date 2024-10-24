#pragma once

#include "main.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "can_base.hpp"

namespace tutrc_harurobo_lib {

class CAN : public CANBase {
public:
  CAN(CAN_HandleTypeDef *hcan, uint32_t filter_id = 0,
      uint32_t filter_mask = 0);
  bool transmit(uint32_t id, uint8_t *data, size_t size) override;
  void set_rx_callback(
      std::function<void(uint32_t, uint8_t *, size_t)> &&callback) override;

private:
  CAN_HandleTypeDef *hcan_;
  std::function<void(uint32_t, uint8_t *, size_t)> rx_callback_;

  static std::unordered_map<CAN_HandleTypeDef *, CAN *> instances_;
  friend void ::HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
};

} // namespace tutrc_harurobo_lib
