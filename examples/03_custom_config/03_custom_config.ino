// en: Use Config to tweak per-hook priority/period while keeping most defaults.
// ja: Config で一部の優先度/周期だけを調整し、他はデフォルトを活かす。
#include <ESP32AutoTask.h>

void LoopCore0_Low()
{
  // en: Slow background task on Core0.
  // ja: コア0のゆったりしたバックグラウンド処理。
  static uint32_t t = 0;
  if ((t++ % 500) == 0)
  {
    Serial.printf("[Core0 Low] millis=%lu\n", millis());
  }
}

void LoopCore1_High()
{
  // en: Faster task on Core1.
  // ja: コア1の高速タスク。
  static uint32_t t = 0;
  if ((t++ % 1000) == 0)
  {
    Serial.printf("[Core1 High] millis=%lu\n", millis());
  }
}

void setup()
{
  Serial.begin(115200);

  ESP32AutoTask::Config cfg;
  // en: Keep stacks at default; only adjust priorities/periods.
  // ja: スタックはデフォルトのまま、優先度と周期だけ調整。
  cfg.core0.low.priority = 1;  // en/ja: leave as default
  cfg.core0.low.periodMs = 10; // en: slow down Low to 10ms / ja: Lowを10ms周期に
  cfg.core1.high.priority = 5; // en: boost Core1 High / ja: Core1 Highを少し上げる
  cfg.core1.high.periodMs = 1; // en/ja: keep tight loop

  ESP32AutoTask::AutoTask.begin(cfg);
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メインloopも動くので、フックが回るよう軽くしておく
  delay(100);
}
