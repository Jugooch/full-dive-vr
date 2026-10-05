// emg-streamer — sample MyoWare 2.0 ENV outputs and stream them over USB serial.
// Output line: E,<millis>,<v0>,<v1>,...   (12-bit ADC counts, 0..4095)
// Status lines start with '#' and are ignored by partialdive.biosignal.SerialEMGSource.
//
// ELECTRICAL SAFETY: while electrodes are on a person, this board must not be coupled to mains.
// Power the MyoWare sensors from the MyoWare battery Power Shield, and connect the ESP32 to a laptop
// running on battery (charger unplugged) or through a USB isolator. See docs/safety.md#electrical.

#include <Arduino.h>

#ifndef FW_VERSION
#define FW_VERSION "dev"
#endif

// ADC1 pins only (ADC2 is unusable while WiFi/BLE is active, which a wireless build will need).
static const uint8_t EMG_PINS[] = {34, 39, 36, 32};  // HUZZAH32 A2, A3, A4, A7 (all ADC1)
static const uint8_t NUM_CHANNELS = 1;          // set to the number of sensors connected (<= 4)
static const uint32_t SAMPLE_RATE_HZ = 1000;
static const uint32_t PERIOD_US = 1000000UL / SAMPLE_RATE_HZ;

uint32_t nextSample;

void setup() {
  Serial.begin(921600);
  analogReadResolution(12);
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) analogSetPinAttenuation(EMG_PINS[i], ADC_11db);
  Serial.printf("# emg-streamer %s channels=%u rate=%lu\n", FW_VERSION, NUM_CHANNELS, SAMPLE_RATE_HZ);
  nextSample = micros();
}

void loop() {
  if ((int32_t)(micros() - nextSample) < 0) return;
  nextSample += PERIOD_US;

  char buf[64];
  int len = snprintf(buf, sizeof(buf), "E,%lu", millis());
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    len += snprintf(buf + len, sizeof(buf) - len, ",%u", analogRead(EMG_PINS[i]));
  }
  buf[len++] = '\n';
  Serial.write((const uint8_t *)buf, len);
}
