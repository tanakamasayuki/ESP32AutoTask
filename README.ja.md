# ESP32AutoTask

[English README](README.md) is available.

ESP32 の Arduino 環境で FreeRTOS タスクを手軽に使うためのヘルパーです。`begin()` を呼ぶだけで各コア（0/1）の Low / Normal / High 優先度タスクを用意し、決められたフック関数をスケッチ側に書けば定期的に実行されます。フックを定義しなければタスクは即終了するため、余計なリソースは使いません。

デフォルト値:
- 実行周期 `periodMs = 1`。
- スタックサイズ `stackSize = ARDUINO_LOOP_STACK_SIZE`（ESP32 Arduino の標準は 8192 バイト）。足りないときは `begin(stackBytes)` で増やす。
- `periodMs` を 0 にすると最速で実行されますが、低優先度タスクが走りにくくなるため 1 以上を推奨します。
- 優先度は FreeRTOS の範囲（0〜24; 数字が大きいほど高い）に収めてください。初心者は 1〜4 程度にとどめるのが安全です。
  - デフォルト優先度: Low=1, Normal=3, High=4（Arduino の `loop()` が優先度 ~1 なので少し上にずらしています）。

## コアと優先度のイメージ

- Arduino の `loop()` は ESP32 Arduino ではコア1にピン留めされたタスク（優先度 ~1、スタック 8192B）として動いています。Wi-Fi/BT などのシステムタスクは主にコア0の高優先度で動きます。
- 本ライブラリはコア0/1それぞれに Low / Normal / High フックを用意し、デフォルト優先度を `1 / 3 / 4` に設定する想定です。`loop()` の少し上の優先度で回したい処理は Normal、より重い処理は High、バックグラウンドの軽作業は Low を使う、と覚えると良いです。
- シングルコアの ESP32シリーズ (例: ESP32-SOLO / ESP32-C3 / C2 / C6 / S2) では、Core1 向けフックも Core0 で実行されます（実行順だけ分けているイメージ）。

## 使い方（最短）

1. `ESP32AutoTask.h` をインクルード。
2. `setup()` で `AutoTask.begin()` を呼ぶ。
3. 呼ばせたいフック関数（例: `LoopCore0_Low` や `LoopCore1_Normal`）を定義する。

```cpp
#include <ESP32AutoTask.h>

void setup() {
    ESP32AutoTask::AutoTask.begin();  // デフォルト設定でタスクを用意
}

// 必要なフックだけ定義すれば動く
void LoopCore0_Low() {
    // コア0・低優先度のループ
}

void LoopCore1_Normal() {
    // コア1・通常優先度のループ
}

void LoopCore1_High() {
    // コア1・高優先度のループ
}
```

- 各コアに Low / Normal / High のフックが用意されています。
- フックを定義しない場合、内部で `vTaskDelete(NULL)` されるため常駐負荷はありません。

## スタックサイズだけ変えたいとき

「デフォルトより足りないかも」と思ったら全タスク共通のスタックサイズを引数で渡します（例: 16384）。

```cpp
void setup() {
    ESP32AutoTask::AutoTask.begin(/*stackBytes=*/16384);  // 既定値からスタックだけ上書き
}
```

- 初心者は基本 `begin()` のみで OK。スタック不足が疑わしいときだけ数字を足す運用を想定。

## さらに細かく設定したいとき（上級者向け）

優先度やスタックサイズ、実行周期を変えたい場合だけ `Config` を渡します。

```cpp
void setup() {
  ESP32AutoTask::Config cfg;
  cfg.core0.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.normal = {3, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.high = {4, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.normal = {3, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.high = {4, ARDUINO_LOOP_STACK_SIZE, 1};

  ESP32AutoTask::AutoTask.begin(cfg);  // 全パラメータを指定
}
```

よくある使い方（例）: デフォルト値をベースに一部だけ変える。

```cpp
ESP32AutoTask::Config cfg;  // デフォルト値で初期化される想定
cfg.core1.high.priority = 5;  // コア1・高優先度だけ強めに
ESP32AutoTask::AutoTask.begin(cfg);
```

## もっと詳しく

設計の考え方や優先度の扱いなど、詳しい技術メモは `SPEC.ja.md` を参照してください。

## ライセンス

MIT License (`LICENSE` を参照)。
