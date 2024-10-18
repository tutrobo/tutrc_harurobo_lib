#pragma once

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include <cstdio>

namespace tutrc_harurobo_lib {

void enable_printf(UART_HandleTypeDef *huart);

}

#endif
