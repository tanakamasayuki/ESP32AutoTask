# ESP32AutoTask

[日本語 README](README.ja.md)

Helper library for ESP32 Arduino to run FreeRTOS tasks easily. Call `begin()` once to prepare Low / Normal / High priority tasks on each core (0/1). Implement the provided hook functions in your sketch to run code periodically; if you leave a hook undefined, the task exits immediately so it doesn’t waste resources.

Defaults:
- `periodMs = 1`
- `stackSize = ARDUINO_LOOP_STACK_SIZE` (ESP32 Arduino’s default: 8192 bytes). If you need more, pass `begin(stackBytes)`.
- Setting `periodMs = 0` runs as fast as possible but can starve lower-priority tasks; keep it ≥1.
- Priorities must stay within FreeRTOS limits (`0–24`, higher is higher). For beginners, stick to ~`1–4`.
  - Default priorities: Low=1, Normal=3, High=4 (offset above the Arduino `loop()` task which is around 1).

## Cores and priority (mental model)

- Arduino’s `loop()` runs as a task pinned to Core 1 (priority ~1, stack 8192B). Wi‑Fi/BT system tasks mainly occupy Core 0 at higher priority.
- This library offers Low / Normal / High hooks on both Core 0 and Core 1, with default priorities `1 / 3 / 4`. Use Normal for work similar to `loop()` but a bit higher priority, High for heavier/urgent work, Low for light background chores.
- On single-core ESP32 parts (ESP32-SOLO / ESP32-C3 / C2 / C6 / S2, etc.), Core1 hooks also run on Core0 (only the ordering is separated).

## Quick start

1. Include `ESP32AutoTask.h`.
2. Call `AutoTask.begin()` in `setup()`.
3. Define the hook functions you need (e.g., `LoopCore0_Low`, `LoopCore1_Normal`).

```cpp
#include <ESP32AutoTask.h>

void setup() {
    ESP32AutoTask::AutoTask.begin();  // Use defaults
}

// Define only the hooks you need
void LoopCore0_Low() {
    // Core0, low priority loop
}

void LoopCore1_Normal() {
    // Core1, normal priority loop
}

void LoopCore1_High() {
    // Core1, high priority loop
}
```

- Each core exposes Low / Normal / High hooks.
- If a hook is not defined, the underlying task calls `vTaskDelete(NULL)` and exits, so there is no resident overhead.

## When you only need a bigger stack

If you suspect the default stack is too small, override the common stack size (applies to all tasks) via the argument:

```cpp
void setup() {
    ESP32AutoTask::AutoTask.begin(/*stackBytes=*/16384);  // Override only the stack size
}
```

- Beginners can stick with `begin()`. Increase the number only when you see stack-related issues.

## Advanced tweaks (Config)

Pass a `Config` when you need to adjust priority, stack size, or period per hook.

```cpp
void setup() {
  ESP32AutoTask::Config cfg;
  cfg.core0.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.normal = {3, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.high = {4, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.normal = {3, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.high = {4, ARDUINO_LOOP_STACK_SIZE, 1};

  ESP32AutoTask::AutoTask.begin(cfg);  // Use fully specified parameters
}
```

Common tweak: keep defaults but bump a single value.

```cpp
ESP32AutoTask::Config cfg;  // Initialized with defaults
cfg.core1.high.priority = 5;  // Raise only Core1 high-priority hook
ESP32AutoTask::AutoTask.begin(cfg);
```

## Multicore & Priority Tips (for beginners leveling up)

- What multicore means: ESP32 typically has two cores (Core0/Core1); the OS schedules tasks across them.  
- Core0 and Wi‑Fi/BT: When wireless is enabled, Core0 runs high-priority system tasks, so user tasks on Core0 may jitter. Without wireless, Core0 is lighter and can host heavier work.  
- Core1 and `loop()`: Arduino `loop()` runs on Core1 at priority ~1. When placing user tasks on Core1, choose priorities relative to `loop()`.  
- Same priority: Tasks on the same core and priority time-slice cooperatively. If you need distinct responsiveness, offset priorities instead of keeping them equal.  
- Interrupts: They outrank tasks but should stay tiny. Do heavy work in tasks; send an event via notification/queue from the ISR.  
- Why notifications: A high-priority task can block on a notification; when an interrupt sends the notification, that task resumes immediately, giving near–real-time response.  
- Time accuracy: Task timing is not exact. Use hardware timers (interrupts) plus a notification to the task when you need tighter timing. `vTaskDelayUntil` helps reduce drift compared with plain delays.  
- I2C/shared buses: Concurrent access from multiple tasks can panic/reset. Funnel I2C/SPI/Serial access through a dedicated task and send requests via queue/notification.  
- High-priority task etiquette: Keep runtime short; heavier work should run at lower priority. Always yield with `delay`/`vTaskDelay` after work so lower-priority tasks can run; skipping waits can cause WDT/panics.  
- Panic checklist: (1) Stack shortage → try `begin(stackBytes)`, (2) missing `delay` in loops, (3) heavy work at same priority, (4) simultaneous access to shared resources.  
- Serial/logging: Heavy `Serial.print` in high-priority tasks can block others. Buffer and flush from a lower-priority or dedicated logging task.  
- When you need more control: If this helper feels limiting, switch to raw FreeRTOS APIs (`xTaskCreatePinnedToCore`, notifications, queues, mutexes) directly.

## More

Design notes and rationale are in `SPEC.ja.md` (Japanese).

## License

MIT License (see `LICENSE`).
