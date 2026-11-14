# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**GliderTimer PRO** is the M5Stack (ESP32) timer/scoring unit of the GliderKeeper system for RC glider competitions. It manages competitions (rounds, groups, pilots), receives flight data from GliderKeeper units over WiFi/UDP, plays MP3 announcements (DFPlayer) and drives optional I2C (MCP23017) panels.

This repo is the PlatformIO conversion of the Arduino IDE sketch [Japicasa/GliderTimer](https://github.com/Japicasa/GliderTimer), taken from branch `Verbose-Pilots-search` at commit `6ae4be5`.

## Build System

PlatformIO, single environment `m5stack` ([platformio.ini](platformio.ini)):

```
pio run                 # build
pio run -t upload       # flash (921600 baud)
pio run -t uploadfs     # upload data/ to SPIFFS
pio device monitor      # serial 115200
```

- **Core: `espressif32@1.12.4` = arduino-esp32 1.0.4.** That is the core the Arduino IDE builds used (see `C:\buildM5`). Do NOT upgrade to 2.x: with core 2.0.9 the image is ~1.41 MB and does not fit the 0x150000 app partition (with 1.0.4 it is ~1.32 MB, 96.0%, after stripping unused M5Stack modules). Flash usage is tight; watch it on every change.
- Partitions: [Flashutils/timer4M.csv](Flashutils/timer4M.csv) (nvs, otadata, app0/app1 0x150000, spiffs 0x110000, `keeper` data partition subtype 99 at 0x3C0000).
- Board `esp32dev`, 4MB, QIO 80 MHz, CPU 240 MHz, `CORE_DEBUG_LEVEL=0`.
- **CPU stays at 240 MHz at all times.** Do not call `setCpuFrequencyMhz()`: changing the frequency with WiFi running causes instability. In STA mode, 80 MHz plus modem sleep caused the `async_tcp` task watchdog to reset the ESP when the web UI was opened. Both the STA and AP paths call `WiFi.setSleep(false)`.

## Libraries

Vendored in [lib/](lib/), copied from the Arduino sketchbook (`C:\arduino-1.8.9v2\portable\sketchbook\libraries`):

| Library | Version | Notes |
|---|---|---|
| M5Stack | 0.3.1 | Trimmed (~41 KB saved). In `utility/In_eSPI_Setup.h` only `LOAD_GLCD`, `LOAD_FONT7` and `LOAD_GFXFF` are enabled; fonts 2/4/6/8 and `SMOOTH_FONT` are off. `SD.begin()` is commented out in `M5Stack::begin()`. Re-enable a font there if code starts using it |
| ESPAsyncWebServer | 1.2.3 | me-no-dev master `f71e3d4` (from `ESPAsyncWebServer-master.zip`), NOT the wi-se-patches fork. `dependencies` removed from its `library.json` so PIO does not pull another AsyncTCP |
| AsyncTCP | 1.1.4 | |
| DFRobotDFPlayerMini | 1.0.2 | |
| Adafruit_SSD1306 | 1.1.2 | |
| Adafruit_GFX_Library | 1.7.5 | `depends=` removed from `library.properties` |

ESPNtpClient comes from `lib_deps` as git tag `0.2.5` (not in the PIO registry). 0.2.6+ needs the lwIP of core 2.x and fails on 1.0.4 (`pbuf::type_internal`).

## Source layout

Everything lives flat in [src/](src/):

- [src/main.cpp](src/main.cpp): the former `GliderTimer.ino` (globals) + `H_Setup.ino` (`setup()`) + `J_loop.ino` (`loop()`, `beginUdp()`, `blink_ontime()`), concatenated in Arduino order. `J_CPU0loop.ino` / `J_CPU1loop.ino` were comment-only and were dropped.
- Former root files (`KeeperESP32WIFI`, `RTC`, `NVSFunctions`, `QueueRegister`, `DebugPrint`, `keeper.h`, `updatefunction.h`) and former `src/` files (`GTimer`, `GTdisplay`, `FlightManager`, `CompetitionSavedRW`, `Myudp`, `MP3player`, `MCP23017Display`, `Timer_flight_data`, `contests`, `Gthrow`) are siblings now. Includes use plain `"X.h"` (no `src/` or `../` prefixes).
- Many `.cpp` files reach globals defined in `main.cpp` via `extern`; keep those definitions in `main.cpp`.

Main objects (globals in `main.cpp`): `GT` (GTimer: menus, competition results), `GD` (GTdisplay), `competition` (CompetitionSaved: flash storage in the `keeper` partition), `FLTdata` (Timer_flight_data, received by WiFi), `flight` (Flight_Manager), `myudp`, `local_RTC`, `MP3`. `loop()` is a state machine on `FLT_mode`.

## SPIFFS data

[data/](data/) holds the web UI (`index.html`/`index.js`, `sched.html`), `GliderKeeperConfig.txt`, `settings.txt`, pilot/draw CSVs and `version.txt`. Upload with `pio run -t uploadfs`. This overwrites the whole SPIFFS, including the settings and WiFi config on the device.

The UI has no Bootstrap or jQuery. `gk.css` is a hand-picked subset of Bootstrap 4.3 classes; if markup starts using a new Bootstrap class, add it there. The collapse, tab and alert-dismiss behaviour (`data-toggle` / `data-dismiss`) is a small vanilla-JS click handler in `index.js`. `MainMenu.*` and `CompMenu.*` are legacy pages that load Bootstrap from a CDN and are not linked from `index.html`.

## Flashutils

[Flashutils/](Flashutils/) keeps the original Arduino-era `.bat` scripts (esptool/mkspiffs). Their paths point to the old Arduino setup; the `.bin` files are git-ignored and not in the repo.
