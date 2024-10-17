#pragma once

#include "main.h"

#if defined(HAL_CAN_MODULE_ENABLED)

#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace tutrc_harurobo_lib {

class CAN {
public:
  CAN(CAN_HandleTypeDef *hcan, uint32_t filter_id = 0, uint32_t filter_mask = 0)
      : hcan_(hcan) {
    if (hcan_->State == HAL_CAN_STATE_READY) {
      CAN_FilterTypeDef filter = {};
      filter.FilterIdHigh = filter_id << 5;
      filter.FilterIdLow = filter_id << 21;
      filter.FilterMaskIdHigh = filter_mask << 5;
      filter.FilterMaskIdLow = filter_mask << 21;
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
  }

  HAL_StatusTypeDef transmit(uint32_t id, uint8_t *data, size_t size) {
    CAN_TxHeaderTypeDef tx_header = {};
    tx_header.StdId = id;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = size > 8 ? 8 : size;
    tx_header.TransmitGlobalTime = DISABLE;

    uint32_t tx_mailbox;

    return HAL_CAN_AddTxMessage(hcan_, &tx_header, data, &tx_mailbox);
  }

  void
  set_rx_callback(std::function<void(uint32_t, uint8_t *, size_t)> &&callback) {
    rx_callbacks_[hcan_] = std::move(callback);
  }

private:
  CAN_HandleTypeDef *hcan_;

  static std::unordered_map<CAN_HandleTypeDef *,
                            std::function<void(uint32_t, uint8_t *, size_t)>>
      rx_callbacks_;
  friend void ::HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
};

} // namespace tutrc_harurobo_lib

#elif defined(HAL_FDCAN_MODULE_ENABLED)

#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace tutrc_harurobo_lib {

class CAN {
public:
  CAN(FDCAN_HandleTypeDef *hfdcan, uint32_t filter_id = 0,
      uint32_t filter_mask = 0)
      : hfdcan_(hfdcan) {
    if (hfdcan_->State == HAL_FDCAN_STATE_READY) {
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
  }

  HAL_StatusTypeDef transmit(uint32_t id, uint8_t *data, size_t size) {
    FDCAN_TxHeaderTypeDef tx_header = {};
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
    default:
      tx_header.DataLength = FDCAN_DLC_BYTES_8;
      break;
    }
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    tx_header.MessageMarker = 0;

    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan_, &tx_header, data);
  }

  void
  set_rx_callback(std::function<void(uint32_t, uint8_t *, size_t)> &&callback) {
    rx_callbacks_[hfdcan_] = std::move(callback);
  }

private:
  FDCAN_HandleTypeDef *hfdcan_;

  static std::unordered_map<FDCAN_HandleTypeDef *,
                            std::function<void(uint32_t, uint8_t *, size_t)>>
      rx_callbacks_;
  friend void ::HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                                          uint32_t RxFifo0ITs);
};

} // namespace tutrc_harurobo_lib

#endif
