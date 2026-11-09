# GliderTimer PRO (PlatformIO)

Versión PlatformIO del sketch Arduino [GliderTimer](https://github.com/Japicasa/GliderTimer)
(convertido desde la rama `Verbose-Pilots-search`, commit `6ae4be5`).

## Uso

```
pio run                 # compilar
pio run -t upload       # flashear (921600 baud)
pio run -t uploadfs     # subir data/ a SPIFFS
pio device monitor      # monitor serie 115200
```

## Cambios respecto al proyecto Arduino

- Los `.ino` (`GliderTimer`, `H_Setup`, `J_loop`) se han unido en `src/main.cpp`.
  `J_CPU0loop.ino` y `J_CPU1loop.ino` solo contenían comentarios y se han eliminado.
- Todos los `.cpp/.h` (raíz y `src/`) están ahora en `src/`; los `#include "src/..."`
  y `#include "../..."` se han ajustado.
- Core: `espressif32@1.12.4` = arduino-esp32 **1.0.4**, el mismo con el que se
  compilaba en Arduino IDE. Con el core 2.0.9 el binario no cabe en la partición
  `timer4M` (app 0x150000).
- Particiones: `Flashutils/timer4M.csv`.
- Librerías copiadas en `lib/` (mismas versiones que el sketchbook de Arduino);
  ESPNtpClient 0.2.5 se descarga desde GitHub (ver `platformio.ini`).
