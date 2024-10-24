#include "tutrc_harurobo_lib/fdcan.hpp"

#ifdef HAL_FDCAN_MODULE_ENABLED

#include <array>

namespace tutrc_harurobo_lib {

std::unordered_map<FDCAN_HandleTypeDef *, FDCAN *> FDCAN::instances_;

FDCAN::FDCAN(FDCAN_HandleTypeDef *hfdcan, uint32_t filter_id,
             uint32_t filter_mask)
    : hfdcan_(hfdcan) {
  instances_[hfdcan_] = this;
  if (hfdcan_->State != HAL_FDCAN_STATE_READY) {
    Error_Handler();
  }

  FDCAN_FilterTypeDef filter = {};
  filter.IdType = FDCAN_STANDARD_ID;
  filter.FilterIndex = 0;
  filter.FilterType = FDCAN_FILTER_MASK;
  filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  filter.FilterID1 = filter_id;
  filter.FilterID2 = filter_mask;

  if (HAL_FDCAN_ConfigFilter(hfdcan_, &filter) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_ConfigGlobalFilter(hfdcan_, FDCAN_REJECT, FDCAN_REJECT,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(hfdcan_, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,
                                     0) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_FDCAN_Start(hfdcan_) != HAL_OK) {
    Error_Handler();
  }
}

bool FDCAN::transmit(uint32_t id, uint8_t *data, size_t size) {
  static FDCAN_TxHeaderTypeDef tx_header = {};
  tx_header.Identifier = id;
  tx_header.IdType = FDCAN_STANDARD_ID;
  tx_header.TxFrameType = FDCAN_DATA_FRAME;
  switch (size) {
  case 0:
    tx_header.DataLength = FDCAN_DLC_BYTES_0;
    break;
  case 1:
    tx_header.DataLength = FDCAN_DLC_BYTES_1;
    break;
  case 2:
    tx_header.DataLength = FDCAN_DLC_BYTES_2;
    break;
  case 3:
    tx_header.DataLength = FDCAN_DLC_BYTES_3;
    break;
  case 4:
    tx_header.DataLength = FDCAN_DLC_BYTES_4;
    break;
  case 5:
    tx_header.DataLength = FDCAN_DLC_BYTES_5;
    break;
  case 6:
    tx_header.DataLength = FDCAN_DLC_BYTES_6;
    break;
  case 7:
    tx_header.DataLength = FDCAN_DLC_BYTES_7;
    break;
  case 8:
    tx_header.DataLength = FDCAN_DLC_BYTES_8;
    break;
  default:
    return false;
  }
  tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  tx_header.BitRateSwitch = FDCAN_BRS_OFF;
  tx_header.FDFormat = FDCAN_CLASSIC_CAN;
  tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  tx_header.MessageMarker = 0;

  return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan_, &tx_header, data) == HAL_OK;
}

void FDCAN::set_rx_callback(
    std::function<void(uint32_t, uint8_t *, size_t)> &&callback) {
  rx_callback_ = std::move(callback);
}

} // namespace tutrc_harurobo_lib

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                               uint32_t RxFifo0ITs) {
  static FDCAN_RxHeaderTypeDef rx_header;
  static std::array<uint8_t, 8> data;

  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, data.data()) ==
      HAL_OK) {
    auto itr = tutrc_harurobo_lib::FDCAN::instances_.find(hfdcan);
    if (itr != tutrc_harurobo_lib::FDCAN::instances_.end()) {
      tutrc_harurobo_lib::FDCAN *can = itr->second;
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
        case FDCAN_DLC_BYTES_8:
          size = 8;
          break;
        default:
          return;
        }
        can->rx_callback_(rx_header.Identifier, data.data(), size);
      }
    }
  }
}

#endif
