#include "can.hpp"

#if defined(HAL_CAN_MODULE_ENABLED)

#include <array>

std::unordered_map<CAN_HandleTypeDef *, tutrc_harurobo_lib::CAN *>
    tutrc_harurobo_lib::CAN::instances_;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rx_header;
  std::array<uint8_t, 8> data;

  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data.data()) ==
      HAL_OK) {
    auto itr = tutrc_harurobo_lib::CAN::instances_.find(hcan);
    if (itr != tutrc_harurobo_lib::CAN::instances_.end()) {
      tutrc_harurobo_lib::CAN *can = itr->second;
      if (can->rx_callback_) {
        can->rx_callback_(rx_header.StdId, data.data(), rx_header.DLC);
      }
    }
  }
}

#elif defined(HAL_FDCAN_MODULE_ENABLED)

#include <array>

std::unordered_map<FDCAN_HandleTypeDef *, tutrc_harurobo_lib::CAN *>
    tutrc_harurobo_lib::CAN::instances_;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  FDCAN_RxHeaderTypeDef rx_header;
  std::array<uint8_t, 8> data;

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, data.data()) ==
      HAL_OK) {
    auto itr = tutrc_harurobo_lib::CAN::instances_.find(hfdcan);
    if (itr != tutrc_harurobo_lib::CAN::instances_.end()) {
      tutrc_harurobo_lib::CAN *can = itr->second;
      if (can->rx_callback_) {
        size_t size;
        switch (rx_header.DataLength) {
        case FDCAN_DLC_BYTES_0:
          size = 0;
          break;
        case FDCAN_DLC_BYTES_1:
          size = 1;
          break;
        case FDCAN_DLC_BYTES_2:
          size = 2;
          break;
        case FDCAN_DLC_BYTES_3:
          size = 3;
          break;
        case FDCAN_DLC_BYTES_4:
          size = 4;
          break;
        case FDCAN_DLC_BYTES_5:
          size = 5;
          break;
        case FDCAN_DLC_BYTES_6:
          size = 6;
          break;
        case FDCAN_DLC_BYTES_7:
          size = 7;
          break;
        default:
          size = 8;
          break;
        }
        can->rx_callback_(rx_header.Identifier, data.data(), size);
      }
    }
  }
}

#endif
