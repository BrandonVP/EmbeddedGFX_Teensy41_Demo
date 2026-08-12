# EmbeddedGFX Teensy 4.1 Demo

Reference project that drives the
[EmbeddedGFX](https://github.com/BrandonVP/EmbeddedGFX) library on a Teensy 4.1
with an **ILI9488** display and **FT6206** capacitive touch. It shows the
complete pattern for consuming the library — as a **git submodule** — from a
Visual Studio / Visual Micro project.

## What it demonstrates

- Writing **display / touch adapters** (`ILI9488Adapter.h`, `FT6206Adapter.h`)
  that implement `IDisplay` / `ITouch`.
- **Project-owned button arrays** placed in `DMAMEM`.
- **Runtime app registration** from `setup()` (`registerApps()`), with the
  library's generic menu pages (`GFX_menuInput` / `GFX_createMenu`).
- The optional **ThemeApp** registered on the Settings menu (RAM-only here — no
  persistence wired up).
- A tiny custom app (`DemoApps.cpp`: About + a live Counter).

Navigation: the top bar has **Home / Tools / Settings**. Tools > Counter has
working +/- buttons; Settings > Themes switches the palette live.

## Prerequisites

Arduino libraries installed where Visual Micro can find them:

- `ILI9488_t3`
- `Adafruit_FT6206_Library` (+ `Adafruit_BusIO`)
- Teensy core (Teensyduino)

## The EmbeddedGFX library is a git submodule

This is the reference for how the projects consume the framework. EmbeddedGFX is
pulled in as a **git submodule** at:

```
EmbeddedGFX_Teensy41_Demo/Libraries/EmbeddedGFX
```

It lives in the sketch's `Libraries/` folder, which is exactly where Visual Micro
looks for project-local libraries — so `#include <EmbeddedGFX.h>` resolves and the
library is **compiled from this folder**, no global install or symlink needed. The
submodule is pinned to a specific commit, so the project always builds against a
known-good version and travels with the repo.

**Clone with the submodule:**

```bash
git clone --recurse-submodules <this-repo-url>
# already cloned without it?
git submodule update --init --recursive
```

**Update the library to its latest** (do this deliberately, then rebuild/test):

```bash
git submodule update --remote EmbeddedGFX_Teensy41_Demo/Libraries/EmbeddedGFX
git add EmbeddedGFX_Teensy41_Demo/Libraries/EmbeddedGFX
git commit -m "Bump EmbeddedGFX"
```

The `.sln` also imports `Libraries\EmbeddedGFX\EmbeddedGFX.vcxitems`, so you can
browse and edit the library source in the same Visual Studio solution.

## Build & upload

1. Make sure the submodule is present (`git submodule update --init --recursive`).
2. Open `EmbeddedGFX_Teensy41_Demo.sln` in Visual Studio.
3. In the Visual Micro toolbar, confirm the board is **Teensy 4.1** and pick the
   USB **Serial** type + COM port. (On first open Visual Micro fills in the
   toolchain include paths for your installed Teensyduino.)
4. Build and upload.

## Porting to other displays

Only the two adapter headers are hardware-specific. To target ESP32-S3 or the
Arduino Due (UTFT/UTouch), write new `IDisplay` / `ITouch` adapters and a matching
font mapping in `setTextSize()`. Nothing in the library changes.
