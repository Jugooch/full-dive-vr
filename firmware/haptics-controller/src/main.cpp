// haptics-controller — ESP32 + TCA9548A I2C mux + one DRV2605L per vibration zone, plus fan PWM.
// Protocol: schemas/haptic-command.md (haptic-command/1). Zone names are NOT known here.
//
// Every DRV2605L has the same fixed I2C address (0x5A), so each sits behind its own TCA9548A channel.
// Up to 8 drivers per mux; add a second mux at 0x71 for more zones.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

#ifndef FW_VERSION
#define FW_VERSION "dev"
#endif

static const uint8_t MUX_ADDR = 0x70;
static const uint8_t NUM_CHANNELS = 8;             // DRV2605L boards on mux ports 0..7
static const uint8_t FAN_PINS[] = {25, 26, 27};    // PWM -> MOSFET/fan driver, never direct
static const uint8_t NUM_FANS = sizeof(FAN_PINS);

static const uint32_t MAX_ON_MS = 2000;            // hard cap per pulse, regardless of request
static const uint32_t WATCHDOG_MS = 5000;          // no host command for this long -> stop all

Adafruit_DRV2605 drv;
bool present[NUM_CHANNELS] = {false};
uint32_t offAt[NUM_CHANNELS] = {0};                // 0 = idle
uint8_t fanDuty[NUM_FANS] = {0};
uint32_t lastCommand = 0;
String line;

void muxSelect(uint8_t ch) {
  Wire.beginTransmission(MUX_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

void setRealtime(uint8_t ch, uint8_t value) {
  muxSelect(ch);
  drv.setMode(DRV2605_MODE_REALTIME);
  drv.setRealtimeValue(value);
}

void channelOff(uint8_t ch) {
  if (!present[ch]) return;
  setRealtime(ch, 0);
  offAt[ch] = 0;
}

void stopAll() {
  for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) channelOff(ch);
  for (uint8_t f = 0; f < NUM_FANS; f++) { analogWrite(FAN_PINS[f], 0); fanDuty[f] = 0; }
}

bool anythingActive() {
  for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) if (offAt[ch]) return true;
  for (uint8_t f = 0; f < NUM_FANS; f++) if (fanDuty[f]) return true;
  return false;
}

void handle(const String &cmd) {
  char op = cmd.length() ? cmd.charAt(0) : 0;
  long a = 0, b = 0, c = 0;
  int n = sscanf(cmd.c_str() + 1, "%ld %ld %ld", &a, &b, &c);

  switch (op) {
    case 'H': {  // H <ch> <strength 0-255> <duration_ms>
      if (n != 3 || a < 0 || a >= NUM_CHANNELS || !present[a] || b < 0 || b > 255 || c < 0) {
        Serial.println("ERR bad H"); return;
      }
      // RTP default format is signed: 0x00..0x7F = 0..full drive.
      setRealtime(a, (uint8_t)(b >> 1));
      offAt[a] = b > 0 ? millis() + min((uint32_t)c, MAX_ON_MS) : 0;
      Serial.println("OK"); return;
    }
    case 'E': {  // E <ch> <effect 1-123>
      if (n != 2 || a < 0 || a >= NUM_CHANNELS || !present[a] || b < 1 || b > 123) {
        Serial.println("ERR bad E"); return;
      }
      muxSelect(a);
      drv.setMode(DRV2605_MODE_INTTRIG);
      drv.setWaveform(0, b);
      drv.setWaveform(1, 0);
      drv.go();
      Serial.println("OK"); return;
    }
    case 'F': {  // F <fan> <duty 0-255>
      if (n != 2 || a < 0 || a >= NUM_FANS || b < 0 || b > 255) { Serial.println("ERR bad F"); return; }
      analogWrite(FAN_PINS[a], b);
      fanDuty[a] = b;
      Serial.println("OK"); return;
    }
    case 'S': stopAll(); Serial.println("OK"); return;
    case 'P': Serial.printf("PONG %lu\n", millis()); return;
    case 'I': {
      uint8_t count = 0;
      for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) count += present[ch];
      Serial.printf("ID haptics-controller %s channels=%u fans=%u\n", FW_VERSION, count, NUM_FANS);
      return;
    }
    default: Serial.println("ERR unknown");
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  for (uint8_t f = 0; f < NUM_FANS; f++) { pinMode(FAN_PINS[f], OUTPUT); analogWrite(FAN_PINS[f], 0); }

  for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
    muxSelect(ch);
    present[ch] = drv.begin();
    if (present[ch]) {
#ifdef USE_LRA
      drv.useLRA();
#else
      drv.useERM();
#endif
      drv.selectLibrary(1);
      drv.setRealtimeValue(0);
    }
  }
  lastCommand = millis();
  handle("I");
}

void loop() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      if (line.length()) { lastCommand = millis(); handle(line); line = ""; }
    } else if (line.length() < 64) {
      line += ch;
    }
  }

  uint32_t now = millis();
  for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
    if (offAt[ch] && (int32_t)(now - offAt[ch]) >= 0) channelOff(ch);
  }
  if (anythingActive() && now - lastCommand > WATCHDOG_MS) stopAll();
}
