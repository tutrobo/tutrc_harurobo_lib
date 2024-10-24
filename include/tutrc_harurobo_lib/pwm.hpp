#pragma once

#include "main.h"

#include <cstdint>

namespace tutrc_harurobo_lib {

class PWM {
public:
  PWM(TIM_HandleTypeDef *htim, uint32_t channel);
  uint32_t get_compare();
  void set_compare(uint32_t compare);

private:
  TIM_HandleTypeDef *htim_;
  uint32_t channel_;
};

} // namespace tutrc_harurobo_lib
