// en: Override only the common stack size when you suspect the default is too small.
// ja: デフォルトのスタックが足りない場合に全タスク共通のスタックサイズを上書き。
#include <ESP32AutoTask.h>

volatile uint32_t acc = 0;

void LoopCore0_High()
{
  // en: Simulate heavier work.
  // ja: ちょっと重い処理の例。
  for (int i = 0; i < 1000; ++i)
  {
    acc += i;
  }
  if ((acc & 0xFFF) == 0)
  {
    Serial.printf("[Core0 High] acc=%lu\n", acc);
  }
}

void setup()
{
  Serial.begin(115200);
  // en: Increase stack for all tasks to 16 KB.
  // ja: 全タスクのスタックを16KBに増やす。
  ESP32AutoTask::AutoTask.begin(/*stackBytes=*/16384);
}

void loop()
{
  // en: Main loop still runs; keep it light to let the hooks execute.
  // ja: メインloopも動くので、フックが回るよう軽くしておく
  delay(1);
}
