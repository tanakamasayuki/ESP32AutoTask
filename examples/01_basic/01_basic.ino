// en: Start a named hook with the default task configuration
// ja: 既定設定で名前付きフックを開始する
#include <ESP32AutoTask.h>

// en: Called on Core1 at "Normal" priority (default period 1ms).
// ja: コア1・Normal優先度で呼ばれる（既定は1ms周期）。
void LoopCore1_Normal()
{
  static uint32_t count = 0;
  if ((count++ % 1000) == 0)
  {
    Serial.printf("[Core1 Normal] millis=%lu\n", millis());
  }
}

void setup()
{
  Serial.begin(115200);
  // en: Initialize this component after its prerequisites; inspect the returned status where provided.
  // ja: 前提となる機器を準備してから初期化し、結果を返すAPIでは成否を確認します。
  ESP32AutoTask::AutoTask.begin(); // en: Use defaults (period=1ms, stack=ARDUINO_LOOP_STACK_SIZE)
                                   // ja: デフォルト設定で開始（period=1ms, stack=ARDUINO_LOOP_STACK_SIZE）
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メインloopも動くので、フックが回るよう軽くしておく。
  delay(1);
}
