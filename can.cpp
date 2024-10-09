#include "can.hpp"

#if defined(HAL_CAN_MODULE_ENABLED)

#include <array>

std::unordered_map<CAN_HandleTypeDef *,
                   std::function<void(uint32_t, uint8_t *, size_t)>>
    tutrc_harurobo_lib::CAN::rx_callbacks_;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rx_header;
  std::array<uint8_t, 8> data;

  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data.data()) ==
      HAL_OK) {
    auto itr = tutrc_harurobo_lib::CAN::rx_callbacks_.find(hcan);
    if (itr != tutrc_harurobo_lib::CAN::rx_callbacks_.end()) {
      itr->second(rx_header.StdId, data.data(), rx_header.DLC);
    }
  }
}

#elif defined(HAL_FDCAN_MODULE_ENABLED)

#include <array>

std::unordered_map<FDCAN_HandleTypeDef *,
                   std::function<void(uint32_t, uint8_t *, size_t)>>
    tutrc_harurobo_lib::CAN::rx_callbacks_;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  FDCAN_RxHeaderTypeDef rx_header;
  std::array<uint8_t, 8> data;

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, data.data()) ==
      HAL_OK) {
    auto itr = tutrc_harurobo_lib::CAN::rx_callbacks_.find(hfdcan);
    if (itr != tutrc_harurobo_lib::CAN::rx_callbacks_.end()) {
      itr->second(rx_header.Identifier, data.data(),
                  rx_header.DataLength >> 16);
    }
  }
}

#endif
