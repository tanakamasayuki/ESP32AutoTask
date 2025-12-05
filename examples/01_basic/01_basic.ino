// en: Basic usage — start tasks with defaults and implement a single hook.
// ja: 基本的な使い方。デフォルト設定のままフックを1つだけ実装。
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
  ESP32AutoTask::AutoTask.begin(); // en: Use defaults (period=1ms, stack=ARDUINO_LOOP_STACK_SIZE)
                                   // ja: デフォルト設定で開始（period=1ms, stack=ARDUINO_LOOP_STACK_SIZE）
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メインloopも動くので、フックが回るよう軽くしておく。
  delay(1);
}
