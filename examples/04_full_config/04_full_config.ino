// en: Construct an explicit per-hook task configuration
// ja: 各フックの設定を明示してタスクを開始する
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

  // en: Initialize with defaults, then assign all fields explicitly
  // ja: デフォルト生成後、全フィールドを明示的に代入
  ESP32AutoTask::Config cfg;
  cfg.core0.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.normal = {2, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.high = {3, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.normal = {2, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.high = {3, ARDUINO_LOOP_STACK_SIZE, 1};

  // en: Initialize this component after its prerequisites; inspect the returned status where provided.
  // ja: 前提となる機器を準備してから初期化し、結果を返すAPIでは成否を確認します。
  ESP32AutoTask::AutoTask.begin(cfg);
}

void loop()
{
  // en: Main loop remains available.
  // ja: メインの loop も通常通り利用できます。
  delay(1);
}
