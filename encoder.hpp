#pragma once

#include "main.h"

#ifdef HAL_TIM_MODULE_ENABLED

#include <cstdint>

namespace tutrc_harurobo_lib {

class Encoder {
public:
  Encoder(TIM_HandleTypeDef *htim, uint16_t ppr, float period)
      : htim_(htim), ppr_(ppr), period_(period) {
    HAL_TIM_Encoder_Start(htim_, TIM_CHANNEL_ALL);
  }

  void update() {
    int16_t delta = __HAL_TIM_GET_COUNTER(htim_);
    __HAL_TIM_SET_COUNTER(htim_, 0);

    float cpr = ppr_ * 4;
    count_ += delta;
    rps_ = delta / period_ / cpr;
    position_ = count_ / cpr;
  }

  float get_rps() { return rps_; }

  float get_rpm() { return get_rps() * 60; }

  float get_position() { return position_; }

private:
  TIM_HandleTypeDef *htim_;
  uint16_t ppr_;
  float period_;

  int64_t count_ = 0;
  float rps_;
  float position_;
};

} // namespace tutrc_harurobo_lib

#endif
