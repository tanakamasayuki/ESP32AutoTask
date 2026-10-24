# 05_gpio_blink

> 日本語版: [README.ja.md](README.ja.md)

Toggle an LED from a periodic hook task.

## Prepare and run

1. Check the [example guide](../../docs/GUIDE_EXAMPLES.md) for setup and hardware. Use the root README for supported boards and dependencies.
2. Open [05_gpio_blink.ino](05_gpio_blink.ino) and select the actual board/port. Prepare the dependencies used by these includes:

   `<ESP32AutoTask.h>`

3. Upload and open the Serial monitor at 115200 baud. Check startup messages and input conditions in the source.

## Follow the sketch

These APIs are listed in their first source order. Registering a callback does not execute its body at that point.

1. `ESP32AutoTask::AutoTask.begin(...)` — Initialize this component after its prerequisites; inspect the returned status where provided.

## API usage and precautions

Initialize Serial and peripherals before `AutoTask.begin()`, then implement the exact global hook names used by the example. These functions run in worker tasks, independently of Arduino loop. Their local static variables retain state, but shared data still needs synchronization.

On a single-core target the Core1 hooks map to the available core; a hook name does not prove simultaneous execution. `periodMs` is a scheduling interval, not a deadline guarantee: a hook that overruns it cannot be made faster by the scheduler. Stack settings are bytes in the ESP32 port.

Change only the fields you need; a full-config example illustrates an explicit configuration rather than a permanent list of defaults.

## Inputs and observable output

These are selected output formats from the source. printf specifiers are replaced by runtime values; inspect the surrounding conditions rather than assuming these lines always occur in this order.

```text
[LED] pin=%d state=%s millis=%lu
```

## Adapt this particular example

A toggle occurs at each hook invocation, so a complete on/off cycle spans two invocations. An addressable RGB LED cannot necessarily be driven with digitalWrite; choose an ordinary GPIO LED or the correct driver.

## Verify and troubleshoot

Toggle an LED from a periodic hook task and verify the result on its output. First use the supplied input, then change one configuration or data source at a time.

| Symptom | First check |
|---|---|
| No output | Board/dependencies, initialization results, output destination and Serial baud |
| Starts but stops making progress | Wait conditions, buffer space, peer/input readiness and service calls |
| Unexpected values | Lengths/units, format, data lifetime, shared state and partial results |

See the [guide](../../docs/GUIDE_EXAMPLES.md) for shared principles and the [example index](../README.md) for the next sketch.
