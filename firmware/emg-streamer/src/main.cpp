// emg-streamer — sample MyoWare 2.0 ENV outputs and stream them over USB serial.
// Output line: E,<millis>,<env_mv0>,<env_mv1>,...   (ENV in millivolts at the SENSOR, divider undone)
// Status lines start with '#' and are ignored by partialdive.biosignal.SerialEMGSource.
//
// INPUT PROTECTION (required): MyoWare ENV spans 0..VIN, and the Power Shield's LiPo is up to 4.2 V
// (unregulated). An ESP32 pin must stay below ~3.3 V and reads accurately only up to ~2.45 V at 11 dB.
// Every ENV line therefore goes through a divider:
//
//   MyoWare ENV ──[ R_TOP 12k ]──┬── ESP32 ADC pin      4.2 V * 15/27 = 2.33 V at the pin
//                                │
//                           [ R_BOTTOM 15k ]           (optional 10 nF from pin to GND)
//                                │
//   MyoWare GND ─────────────────┴── ESP32 GND          (common ground is required)
//
// See firmware/emg-streamer/README.md for the bring-up checklist (measure before connecting a person).
//
// ELECTRICAL SAFETY: while electrodes are on a person, this board must not be coupled to mains.
// MyoWare runs from its battery Power Shield; the ESP32 connects through the USB isolator (Olimex
// USB-ISO, never its external power jack) or to a laptop on battery. See docs/safety.md#electrical.

#include <Arduino.h>

#ifndef FW_VERSION
#define FW_VERSION "dev"
#endif

// Divider resistors (ohms). Change these if you build a different divider; the output stays in sensor mV.
#ifndef R_TOP_OHMS
#define R_TOP_OHMS 12000.0f
#endif
#ifndef R_BOTTOM_OHMS
#define R_BOTTOM_OHMS 15000.0f
#endif
static const float DIVIDER_GAIN = (R_TOP_OHMS + R_BOTTOM_OHMS) / R_BOTTOM_OHMS;  // pin mV -> sensor mV

// ADC1 pins only (ADC2 is unusable while WiFi/BLE is active, which a wireless build will need).
static const uint8_t EMG_PINS[] = {34, 39, 36, 32};  // HUZZAH32 A2, A3, A4, A7 (all ADC1)
static const uint8_t NUM_CHANNELS = 1;               // set to the number of sensors connected (<= 4)
static const uint32_t SAMPLE_RATE_HZ = 1000;
static const uint32_t PERIOD_US = 1000000UL / SAMPLE_RATE_HZ;

uint32_t nextSample;

void setup() {
  Serial.begin(921600);
  analogReadResolution(12);
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) analogSetPinAttenuation(EMG_PINS[i], ADC_11db);
  Serial.printf("# emg-streamer %s channels=%u rate=%lu units=mV divider=%.3f\n",
                FW_VERSION, NUM_CHANNELS, SAMPLE_RATE_HZ, DIVIDER_GAIN);
  nextSample = micros();
}

void loop() {
  if ((int32_t)(micros() - nextSample) < 0) return;
  nextSample += PERIOD_US;

  char buf[64];
  int len = snprintf(buf, sizeof(buf), "E,%lu", millis());
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    // analogReadMilliVolts applies the chip's factory ADC calibration (pin voltage, mV).
    uint32_t sensorMv = (uint32_t)(analogReadMilliVolts(EMG_PINS[i]) * DIVIDER_GAIN + 0.5f);
    len += snprintf(buf + len, sizeof(buf) - len, ",%lu", sensorMv);
  }
  buf[len++] = '\n';
  Serial.write((const uint8_t *)buf, len);
}
