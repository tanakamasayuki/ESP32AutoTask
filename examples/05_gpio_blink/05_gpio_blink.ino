// en: Blink GPIO (on-board LED on many ESP32 boards) every 1000 ms using a hook task.
// ja: 多くのESP32ボードでLEDが接続されているGPIOを、1秒周期で点滅させる例。
#include <ESP32AutoTask.h>

// en: Define LED pin: prefer LED_BUILTIN if available, otherwise fallback to GPIO2.
// ja: LEDピンを定義。LED_BUILTIN があれば優先し、無ければ GPIO2 を使う。
#ifndef LED_PIN
#if defined(LED_BUILTIN)
#define LED_PIN LED_BUILTIN
#else
#define LED_PIN 2
#endif
#endif

// en: Hook running on Core1 Normal; period is configured to 1000 ms in setup.
// ja: Core1 Normal のフックで点滅。周期は setup で 1000ms に設定。
void LoopCore1_Normal()
{
  static bool state = false;
  digitalWrite(LED_PIN, state ? HIGH : LOW);
  Serial.printf("[LED] pin=%d state=%s millis=%lu\n", LED_PIN, state ? "ON" : "OFF", millis());
  state = !state;

  // en: Even if there's a long processing in the loop, the next call is adjusted by periodMs.
  // ja: Loop内で長時間処理をしてもperiodMsで次回呼び出しが調整される。
  delay(100);
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // en: Configure only the period for Core1 Normal to 1000 ms (1 second).
  // ja: Core1 Normal の周期だけ 1000ms に設定。
  ESP32AutoTask::Config cfg;
  cfg.core1.normal.periodMs = 1000;

  ESP32AutoTask::AutoTask.begin(cfg);
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メイン loop も動くので、フックが回るよう軽くしておく。
  delay(1);
}
