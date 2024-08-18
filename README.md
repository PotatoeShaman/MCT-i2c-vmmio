This program is written for the the RaspberryPi Pico (RP2040) platform.

Facilitates virtual memory mapped I/O functionailty over an i2C bus.
This is only a slave driver, any other i2C capable microcontroller can be used as a master if using the same message format: *[RP2040 Address] , [“Register” (Servo) Number] , [Value]*.

The intended use is to deligate non-time-critical auxiliary tasks to a slower device (RP2040) so that a faster device may handle system critical operations without hindering performance if extra auxiliary functionality is desired in the future.
This bus configuration allows for multiple slaves to be addressed individually for a modular distributed control system.
