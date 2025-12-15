# ESP32AutoTask Technical Notes

This is a spec and design memo for a helper library (under design) that makes creating and managing FreeRTOS tasks easy in the ESP32 Arduino environment. A single `begin()` (no config) prepares multiple-priority tasks on Core 0 / Core 1; when the user overrides the weakly provided hook functions, that code runs periodically. If hooks are not defined, the tasks end immediately so no wasted tasks remain.

## Goals

- Hide FreeRTOS task creation / pinning / priority settings so Arduino sketches can start parallel work with minimal steps.
- Keep the default configuration "do nothing" so unused tasks impose zero overhead.
- Offer hook points separated by core (0/1) and priority levels so users can choose the right place for their workload.

## Hook mechanism

The library declares hooks as weak symbols and calls them periodically from tasks. If the user defines the same-named functions in the sketch, they override the weak ones and run in a loop.

```cpp
__attribute__((weak)) void LoopCore0_Low() {
    vTaskDelete(NULL);  // Exit immediately by default
}

void TaskCore0_Low(void *pv) {
    for (;;) {
        LoopCore0_Low();  // Runs if the user overrides the hook
        delay(1);         // Execution interval is planned to be configurable
    }
}
```

Notes:
- Using weak symbols achieves "zero cost unless defined."
- Each task is pinned with `xTaskCreatePinnedToCore`, with priority, stack size, and interval supplied from configuration.
- Inside the loop, use `delay` / `vTaskDelay` to avoid blocking other tasks.

## Proposed API (draft)

`begin()` without arguments is the baseline; only priority or stack size can be overridden for a minimal, beginner-friendly surface. Actual function and struct names may change during implementation.

```cpp
#include <ESP32AutoTask.h>

using namespace ESP32AutoTask;

void setup() {
    AutoTask.begin();              // Start with defaults (config is optional)
    // AutoTask.begin(4096);       // Override a single common stack size for all tasks
}

// Defining a hook keeps the corresponding task alive
void LoopCore0_Low() {
    // Periodic execution on Core 0, low priority
}

void LoopCore1_Normal() {
    // Periodic execution on Core 1, normal priority
}

void setupWithConfig() {
  ESP32AutoTask::Config cfg;
  cfg.core0.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.normal = {2, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core0.high = {3, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.low = {1, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.normal = {2, ARDUINO_LOOP_STACK_SIZE, 1};
  cfg.core1.high = {3, ARDUINO_LOOP_STACK_SIZE, 1};

  AutoTask.begin(cfg);  // Provide all parameters explicitly
}
```

Points:
- If config is omitted, tasks are created with defaults. Hooks that remain undefined call `vTaskDelete(NULL)` and exit immediately, so wasted overhead is minimized.
- Default priorities keep the order "Low < Normal < High" within the same core while avoiding contention with Arduino's `loop()` (priority ~1): Low=1 / Normal=2 / High=3. Core1 Low matches `loop()`, so avoid long blocks there.
- Execution period will be specified in milliseconds. `periodMs = 0` means run again immediately, but 1 ms or longer is recommended to avoid monopolizing the CPU.

## Design notes: how to split hooks by core x priority

- **Why separate Core 0 / Core 1**  
  The Wi-Fi/Bluetooth stack and `loop()` usually run on Core 1, so moving heavy work to Core 0 helps keep UI responsiveness. Having core-specific hooks is reasonable.

- **Priority level presets**  
  Roughly three levels (Low/Normal/High) are easy to handle. Let users override `priority` directly while documenting defaults to help beginners.  
  - Low: background (logging, light maintenance)  
  - Normal: general periodic work  
  - High: timing-sensitive processing; ISR is still preferred for interrupt-grade work.

- **Task count upper bound**  
  At most six tasks (2 cores x 3 priorities). Beyond that, opt-in should be explicit because of ESP32 RAM and scheduler load.

- **Default behavior**  
  Calling `begin()` with no arguments creates per-core, per-priority tasks with defaults. Hooks that are not defined call `vTaskDelete(NULL)` and exit, leaving virtually zero resident overhead. Override only priority or period as needed.

- **Common pitfalls to preempt**  
  - Document default stack sizes and describe symptoms and remedies when stack is insufficient.

## Stack size considerations (prioritize simplicity for beginners)

- Target audience is ESP32 beginners. Fix the priority presets (Low/Normal/High) and expose only stack size as the minimal knob for safety.
- Typical usage expects only 1-2 tasks, so choosing a single stack size shared by all tasks causes the least confusion.
- API idea:
  1. `begin()` ... use defaults.
  2. `begin(stackBytes)` ... override only the shared stack size for all tasks (e.g., `AutoTask.begin(/*stackBytes=*/16384);`).
  3. `begin(config)` ... advanced users only. Set priority, period, and stack per hook. Beginners should not need this.
- Weak functions that return sizes are avoided because they add more definition points and are not type-safe, so they are a poor fit for beginners.
- Default guidance: `periodMs = 1`, `stackSize = ARDUINO_LOOP_STACK_SIZE` (ESP32 Arduino default is 8192 words ~ 32 KB, shared by all tasks; FreeRTOS stack sizes are in words). Include symptoms of shortage (Guru Meditation / WDT / exceptions) and how to bump the size (e.g., 8192 -> 16384).
- `periodMs = 0` runs at full speed but can starve lower-priority tasks, so recommend 1 or higher.
- Priority range follows FreeRTOS `0-24` (higher is higher). Beginner presets stay around 1-4.

## Core and priority examples; differences from Arduino

- Arduino `loop()` in ESP32 Arduino is a task pinned to Core 1 (priority ~1, stack 8192 words ~ 32 KB). System tasks like Wi-Fi/BT mostly run on Core 0 at higher priority.
- This library provides Low / Normal / High hooks on both Core 0 and Core 1, with default priorities roughly `Low=1 / Normal=2 / High=3`. Work that should match `loop()` priority can go to Core1 Low (avoid long blocks); slightly heavier work fits Normal; timing-sensitive work goes to High; light background work goes to Low.
- On single-core ESP32 parts (ESP32-SOLO / ESP32-C3 / C2 / C6 / S2, etc.), Core1 hooks also execute on Core0 (the names just imply ordering).

## License

MIT License (see `LICENSE`).
