#pragma once

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include <cstdio>

namespace tutrc_harurobo_lib {

void enable_stdin(UART_HandleTypeDef *huart);

}

#endif
