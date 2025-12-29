# WebPad360 - Raspberry Pi Pico W-based XInput-like Web Controller

This project turns a Raspberry Pi Pico W into a Wi-Fi controlled, Xbox 360 (XInput) compatible gamepad. A built‑in HTTP server exposes a simple web UI where you can press buttons. Each press is translated into an authentic XInput report and sent over USB to the host computer, while a short LED blink provides visual feedback.

> [!IMPORTANT]
> WebPad360 works out of the box on Windows/macOS/Linux. Running it on an actual Xbox 360 requires a modded console (modchip or “BadUpdate” firmware with USBdsec patches; XeUnshakle already includes them). Stock retail dashboards reject non-authenticated devices, so please ensure the console is patched before attempting to use this project.

## Features

- ✅ Enumerates as a genuine XInput controller (VID/PID `0x045E/0x028E`)
- ✅ Serves a lightweight web interface for button presses
- ✅ Wi‑Fi client with configurable static IP
- ✅ Buttons auto‑release after 100 ms to avoid “stuck” inputs
- ✅ Onboard LED flashes on every press

## Known Issues/Bugs

- Only static IP address for now (for some reason, can't get DHCP to work)
- No sticks for now
- Simple web UI/UX (to serve as a protoype)

## Prerequisites

1. **Hardware**: Raspberry Pi Pico W, USB cable, (optional) UART adapter (logging uses UART).
2. **Toolchain**:
   - ARM GCC toolchain (`arm-none-eabi-gcc`).
   - CMake ≥ 3.13 and Ninja or Make.
   - Python 3 (required by the Pico SDK tools).
3. **Pico SDK**: Clone [pico-sdk](https://github.com/raspberrypi/pico-sdk) and initialize its submodules. Set `PICO_SDK_PATH` to that location.

## Building

### macOS / Linux
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
mkdir build
cd build
cmake -DPICO_BOARD=pico_w ..
cmake --build .
```

### Windows (PowerShell)
```powershell
$env:PICO_SDK_PATH="C:/path/to/pico-sdk"
mkdir build
cd build
cmake -G "Ninja" -DPICO_BOARD=pico_w ..
cmake --build .
```
Use `-G "Ninja"` or `-G "MinGW Makefiles"` depending on your installed tools.

### Configuration Notes
- Wi-Fi credentials and static IP settings live near the top of `main.c`. Change `ssid`, `password`, and the IP information in `configure_static_ip` before building.
- Logging only via UART (`pico_enable_stdio_uart(... 1)`). Connect to GP0/GP1 at 115200 baud if you need runtime logs.

## Flashing the Pico

1. Hold the **BOOTSEL** button on the Pico W and plug it into your computer.
2. A mass-storage drive (RPI-RP2) will appear.
3. Copy `build/WebPad360.uf2` onto that drive. The board will reboot into the firmware automatically.

## Using the Controller

1. Open a serial terminal (optional) to monitor logs via UART.
2. Once powered, the device will:
   - Enumerate over USB as an Xbox 360 controller.
   - Connect to Wi-Fi using the configured credentials and static IP (default `192.168.0.115`).
3. Navigate to `http://192.168.0.115/` (or the IP you configured). Press the on-screen buttons to send inputs to the host system.
4. Each press blinks the onboard LED and is sent as a 100 ms tap to avoid held buttons.

## Repository Layout

```
├── CMakeLists.txt        # Build configuration
├── main.c                # Application entry (Wi-Fi + XInput bridge)
├── index_html.c          # Embedded web UI + lwIP custom fs
├── xinput/               # XInput TinyUSB driver and descriptors
├── tusb_config.h         # TinyUSB configuration
├── config/               # lwIP configuration overrides
├── example/              # Reference PicoGamepadConverter sources
└── build/                # Generated build outputs (ignored)
```

## Troubleshooting

- **Not detected as controller**: Ensure TinyUSB has exclusive access (no other CDC interface). Replug after flashing.
- **Can’t reach web UI**: Confirm Wi-Fi credentials/IP and that your network allows the configured static IP.
- **Need logs**: Use a USB–UART adapter on GP0 (TX) / GP1 (RX) at 115200 baud.

## Credits

- Portions of the USB/XInput implementation take inspiration from [Loc15/PicoGamepadConverter](https://github.com/Loc15/PicoGamepadConverter). Many thanks to that project for serving as a reference.
