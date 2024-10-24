#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>

namespace tutrc_harurobo_lib {

class CANBase {
public:
  virtual ~CANBase() {}
  virtual bool transmit(uint32_t id, uint8_t *data, size_t size) = 0;
  virtual void set_rx_callback(
      std::function<void(uint32_t, uint8_t *, size_t)> &&callback) = 0;
};

} // namespace tutrc_harurobo_lib
