// en: Toggle an LED from a periodic hook task
// ja: 周期フックでLED出力を切り替える
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
  // ja: ループ内で長時間処理をしてもperiodMsで次回呼び出しが調整される。
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

  // en: Initialize this component after its prerequisites; inspect the returned status where provided.
  // ja: 前提となる機器を準備してから初期化し、結果を返すAPIでは成否を確認します。
  ESP32AutoTask::AutoTask.begin(cfg);
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メイン loop も動くので、フックが回るよう軽くしておく。
  delay(1);
}
