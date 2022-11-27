#include "pulse_width.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pulse_width {

static const char *const TAG = "pulse_width";

void IRAM_ATTR PulseWidthSensorStore::gpio_intr(PulseWidthSensorStore *arg) {
  const bool new_level = arg->pin_.digital_read();
  const uint32_t now = micros();
  if (new_level) {
    arg->last_period_ = (now - arg->last_rise_);
    arg->last_rise_ = now;
  } else {
    arg->last_width_ = (now - arg->last_rise_);
  }

  // ignore first cycle after resetting
  if ( !new_level) {
    return;
  }
  else if ( arg->skip_ ) {
    arg->skip_ = false;
  }
  else {
    arg->valid_ = true;
  }
}

void PulseWidthSensorStore::reset() {
  this->skip_        = true;      // skip first received edge
  this->valid_       = false;     // data no longer valid
  this->last_rise_   = micros();  // consider this the new previous rise time for new analysis
  this->last_period_ = 0;
}


//  ESP_LOGD("KAUF PW TEST","------------ AVG: %d", arg->avg_num_ / arg->avg_sum_);



void PulseWidthSensor::dump_config() {
  LOG_SENSOR("", "Pulse Width", this)
  LOG_UPDATE_INTERVAL(this)
  LOG_PIN("  Pin: ", this->pin_);
}
void PulseWidthSensor::update() {
  float width = this->store_.get_pulse_width_s();
  ESP_LOGCONFIG(TAG, "'%s' - Got pulse width %.3f s", this->name_.c_str(), width);
  this->publish_state(width);
}

}  // namespace pulse_width
}  // namespace esphome