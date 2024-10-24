#include "../fdcan.hpp"

#ifdef HAL_CAN_MODULE_ENABLED

#include <array>

namespace tutrc_harurobo_lib {

std::unordered_map<CAN_HandleTypeDef *, CAN *> CAN::instances_;

CAN::CAN(CAN_HandleTypeDef *hcan, uint32_t filter_id, uint32_t filter_mask)
    : hcan_(hcan) {
  instances_[hcan_] = this;
  if (hcan_->State != HAL_CAN_STATE_READY) {
    Error_Handler();
  }

  CAN_FilterTypeDef filter = {};
  filter.FilterIdHigh = filter_id << 5;
  filter.FilterIdLow = 0;
  filter.FilterMaskIdHigh = filter_mask << 5;
  filter.FilterMaskIdLow = 0;
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter.FilterBank = 0;
#ifdef CAN2
  if (hcan_->Instance == CAN2) {
    filter.FilterBank = 14;
  }
#endif
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(hcan_, &filter) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(hcan_, CAN_IT_RX_FIFO0_MSG_PENDING) !=
      HAL_OK) {
    Error_Handler();
  }

  if (HAL_CAN_Start(hcan_) != HAL_OK) {
    Error_Handler();
  }
}

bool CAN::transmit(uint32_t id, uint8_t *data, size_t size) {
  static CAN_TxHeaderTypeDef tx_header = {};
  tx_header.StdId = id;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = size;
  tx_header.TransmitGlobalTime = DISABLE;

  uint32_t tx_mailbox;

  return HAL_CAN_AddTxMessage(hcan_, &tx_header, data, &tx_mailbox) == HAL_OK;
}

void CAN::set_rx_callback(
    std::function<void(uint32_t, uint8_t *, size_t)> &&callback) {
  rx_callback_ = std::move(callback);
}

} // namespace tutrc_harurobo_lib

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  static CAN_RxHeaderTypeDef rx_header;
  static std::array<uint8_t, 8> data;

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

#endif
