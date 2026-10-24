// en: Change selected hook periods and priorities
// ja: 一部のフックの周期と優先度を変更する
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
  // en: boost Core1 High
  // ja: Core1 Highを少し上げる
  cfg.core1.high.priority = 4;
  cfg.core1.high.periodMs = 1; // en/ja: keep tight loop

  // en: Initialize this component after its prerequisites; inspect the returned status where provided.
  // ja: 前提となる機器を準備してから初期化し、結果を返すAPIでは成否を確認します。
  ESP32AutoTask::AutoTask.begin(cfg);
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メインloopも動くので、フックが回るよう軽くしておく
  delay(100);
}
