#pragma once

#include <Arduino.h>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Hook prototypes (user can override these in their sketch; global namespace for simplicity)
void LoopCore0_Low();
void LoopCore0_Normal();
void LoopCore0_High();
void LoopCore1_Low();
void LoopCore1_Normal();
void LoopCore1_High();

namespace ESP32AutoTask
{

  // Defaults
  constexpr uint32_t kDefaultPeriodMs = 1;
#ifndef ARDUINO_LOOP_STACK_SIZE
// Fallback if the core definition is not available (should be 8192 on ESP32 Arduino)
#define ARDUINO_LOOP_STACK_SIZE 8192
#endif
  constexpr uint32_t kDefaultStackSize = ARDUINO_LOOP_STACK_SIZE;

  struct TaskConfig
  {
    uint32_t priority;
    uint32_t stackSize;
    uint32_t periodMs;
  };

  struct CoreConfig
  {
    TaskConfig low;
    TaskConfig normal;
    TaskConfig high;
  };

  struct Config
  {
    CoreConfig core0;
    CoreConfig core1;

    Config(); // initializes with defaults
  };

  class AutoTaskClass
  {
  public:
    void begin();
    void begin(uint32_t stackBytes);
    void begin(const Config &config);

  private:
    void startTasks(const Config &config);
    bool initialized_ = false;
  };

  // Task handles (set after begin)
  extern TaskHandle_t handleCore0Low;
  extern TaskHandle_t handleCore0Normal;
  extern TaskHandle_t handleCore0High;
  extern TaskHandle_t handleCore1Low;
  extern TaskHandle_t handleCore1Normal;
  extern TaskHandle_t handleCore1High;

  extern AutoTaskClass AutoTask;

} // namespace ESP32AutoTask
