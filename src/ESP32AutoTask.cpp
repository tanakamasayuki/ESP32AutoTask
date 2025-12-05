#include "ESP32AutoTask.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace ESP32AutoTask
{

  // Default configuration initializer
  Config::Config()
  {
    core0.low = {1, kDefaultStackSize, kDefaultPeriodMs};
    core0.normal = {3, kDefaultStackSize, kDefaultPeriodMs};
    core0.high = {4, kDefaultStackSize, kDefaultPeriodMs};
    core1.low = {1, kDefaultStackSize, kDefaultPeriodMs};
    core1.normal = {3, kDefaultStackSize, kDefaultPeriodMs};
    core1.high = {4, kDefaultStackSize, kDefaultPeriodMs};
  }

  // Weak hook defaults: exit immediately to avoid overhead if not overridden.
  __attribute__((weak)) void LoopCore0_Low() { vTaskDelete(nullptr); }
  __attribute__((weak)) void LoopCore0_Normal() { vTaskDelete(nullptr); }
  __attribute__((weak)) void LoopCore0_High() { vTaskDelete(nullptr); }
  __attribute__((weak)) void LoopCore1_Low() { vTaskDelete(nullptr); }
  __attribute__((weak)) void LoopCore1_Normal() { vTaskDelete(nullptr); }
  __attribute__((weak)) void LoopCore1_High() { vTaskDelete(nullptr); }

  namespace
  {

    Config g_config{};

    void taskLoop(uint32_t periodMs, void (*fn)())
    {
      for (;;)
      {
        fn();
        delay(periodMs); // delay(0) yields; no special case needed
      }
    }

    void TaskCore0Low(void *) { taskLoop(g_config.core0.low.periodMs, LoopCore0_Low); }
    void TaskCore0Normal(void *) { taskLoop(g_config.core0.normal.periodMs, LoopCore0_Normal); }
    void TaskCore0High(void *) { taskLoop(g_config.core0.high.periodMs, LoopCore0_High); }
    void TaskCore1Low(void *) { taskLoop(g_config.core1.low.periodMs, LoopCore1_Low); }
    void TaskCore1Normal(void *) { taskLoop(g_config.core1.normal.periodMs, LoopCore1_Normal); }
    void TaskCore1High(void *) { taskLoop(g_config.core1.high.periodMs, LoopCore1_High); }

  } // namespace

  void AutoTaskClass::begin()
  {
    Config cfg;
    startTasks(cfg);
  }

  void AutoTaskClass::begin(uint32_t stackBytes)
  {
    Config cfg;
    cfg.core0.low.stackSize = stackBytes;
    cfg.core0.normal.stackSize = stackBytes;
    cfg.core0.high.stackSize = stackBytes;
    cfg.core1.low.stackSize = stackBytes;
    cfg.core1.normal.stackSize = stackBytes;
    cfg.core1.high.stackSize = stackBytes;
    startTasks(cfg);
  }

  void AutoTaskClass::begin(const Config &config)
  {
    startTasks(config);
  }

  void AutoTaskClass::startTasks(const Config &config)
  {
    if (initialized_)
    {
      return;
    }

    g_config = config;
    const UBaseType_t coreForCore1 = (CONFIG_FREERTOS_NUMBER_OF_CORES > 1) ? 1 : 0;

    xTaskCreatePinnedToCore(TaskCore0Low, "AT0L", g_config.core0.low.stackSize, nullptr, g_config.core0.low.priority, nullptr, 0);
    xTaskCreatePinnedToCore(TaskCore0Normal, "AT0N", g_config.core0.normal.stackSize, nullptr, g_config.core0.normal.priority, nullptr, 0);
    xTaskCreatePinnedToCore(TaskCore0High, "AT0H", g_config.core0.high.stackSize, nullptr, g_config.core0.high.priority, nullptr, 0);
    xTaskCreatePinnedToCore(TaskCore1Low, "AT1L", g_config.core1.low.stackSize, nullptr, g_config.core1.low.priority, nullptr, coreForCore1);
    xTaskCreatePinnedToCore(TaskCore1Normal, "AT1N", g_config.core1.normal.stackSize, nullptr, g_config.core1.normal.priority, nullptr, coreForCore1);
    xTaskCreatePinnedToCore(TaskCore1High, "AT1H", g_config.core1.high.stackSize, nullptr, g_config.core1.high.priority, nullptr, coreForCore1);

    initialized_ = true;
  }

  AutoTaskClass AutoTask;

} // namespace ESP32AutoTask
