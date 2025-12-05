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
    TaskHandle_t hCore0Low = nullptr;
    TaskHandle_t hCore0Normal = nullptr;
    TaskHandle_t hCore0High = nullptr;
    TaskHandle_t hCore1Low = nullptr;
    TaskHandle_t hCore1Normal = nullptr;
    TaskHandle_t hCore1High = nullptr;

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

    void createTask(const char *name,
                    TaskFunction_t fn,
                    const TaskConfig &cfg,
                    UBaseType_t core,
                    TaskHandle_t *handle)
    {
      if (*handle)
      {
        vTaskDelete(*handle);
        *handle = nullptr;
      }
      xTaskCreatePinnedToCore(fn, name, cfg.stackSize, nullptr, cfg.priority, handle, core);
    }

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

  void AutoTaskClass::end()
  {
    stopTasks();
  }

  void AutoTaskClass::startTasks(const Config &config)
  {
    g_config = config;

    createTask("AT0L", TaskCore0Low, g_config.core0.low, 0, &hCore0Low);
    createTask("AT0N", TaskCore0Normal, g_config.core0.normal, 0, &hCore0Normal);
    createTask("AT0H", TaskCore0High, g_config.core0.high, 0, &hCore0High);
    createTask("AT1L", TaskCore1Low, g_config.core1.low, 1, &hCore1Low);
    createTask("AT1N", TaskCore1Normal, g_config.core1.normal, 1, &hCore1Normal);
    createTask("AT1H", TaskCore1High, g_config.core1.high, 1, &hCore1High);
  }

  void AutoTaskClass::stopTasks()
  {
    if (hCore0Low)
    {
      vTaskDelete(hCore0Low);
      hCore0Low = nullptr;
    }
    if (hCore0Normal)
    {
      vTaskDelete(hCore0Normal);
      hCore0Normal = nullptr;
    }
    if (hCore0High)
    {
      vTaskDelete(hCore0High);
      hCore0High = nullptr;
    }
    if (hCore1Low)
    {
      vTaskDelete(hCore1Low);
      hCore1Low = nullptr;
    }
    if (hCore1Normal)
    {
      vTaskDelete(hCore1Normal);
      hCore1Normal = nullptr;
    }
    if (hCore1High)
    {
      vTaskDelete(hCore1High);
      hCore1High = nullptr;
    }
  }

  AutoTaskClass AutoTask;

} // namespace ESP32AutoTask
