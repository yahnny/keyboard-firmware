# GMMK Pro QMK firmware

My custom QMK firmware for the Glorious GMMK Pro: Caps Lock as Esc/Meh with
permissive hold, and a fixed lighting profile.

**Quick reference**

| Task | How |
|---|---|
| Build | Copy keymap into QMK, then `qmk compile` (see [Build](#build)) |
| Flash | QMK Toolbox on Windows, open the `.bin`, Fn + \\, Flash |
| Bootloader mode | **Fn + \\** |
| Edit keys / behavior | `keyboards/gmmkpro/keymap.c` and `config.h` |
| Firmware output | `qmk_firmware/gmmk_pro_rev1_ansi_gmmkpro.bin` |

---

## My keyboard

| | |
|---|---|
| Board | Glorious GMMK Pro |
| Layout | ANSI (US: wide one-row Enter, `\` above Enter, long left Shift) |
| Revision | rev1 (STM32F303 MCU) |
| QMK target | `gmmk/pro/rev1/ansi` |
| Keymap name | `gmmkpro` |

---

## What the current firmware does

All of this is in `keyboards/gmmkpro/`.

- **Caps Lock** (`CL_MEH`): tap = **Esc**, hold = **Meh** (Ctrl+Shift+Alt,
  no Win).
  - Uses **permissive hold** on that key only: hold Caps, press *and
    release* another key, and you get Meh + that key even if it was faster
    than the 200 ms tapping term.
  - Rolling (letting go of Caps before the other key) stays Esc + key, so
    fast `Esc` → `j` in vim is safe.
- **Fn + Caps Lock**: real Caps Lock.
- **Fn + \\**: bootloader mode (`QK_BOOT`).
- **Lighting**: solid white at ~75% brightness, applied at every power-on in
  `keyboard_post_init_user()`. Fn + RGB keys still work but reset on restart.
- **Fn layer** otherwise matches the GMMK Pro default:
  - F1–F12 row: My Computer, Browser, Calculator, Media Select, Prev, Next,
    Play, Stop, Mute, Vol-, Vol+
  - Fn + 1: RGB on/off, Fn + W / S: brightness up/down, Fn + X: hue
  - Fn + ↑ / ↓: next / previous effect, Fn + ← / →: effect speed down/up
  - Fn + N: toggle NKRO
- **Knob**: volume.

### Known issues

- **Caps Word doesn't do anything yet.** `CAPS_WORD_ENABLE = yes` is in
  `rules.mk`, but nothing turns it on. Fix: add
  `#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD` to `config.h`.

---

## Getting started (fresh machine or after forgetting everything)

Everything runs in **WSL (Ubuntu)** except flashing, which runs on **Windows**.

### 1. Install the QMK CLI (once per machine)

```bash
python3 -m pip install --user qmk     # or: pipx install qmk
```

Make sure `~/.local/bin` is on your PATH (`source ~/.bashrc` after editing it).

### 2. Download QMK into this repo (once)

From `/root/source/keyboard`:

```bash
qmk setup -H /root/source/keyboard/qmk_firmware -y
```

This clones QMK into `qmk_firmware/`, makes it QMK's home folder, and
installs or checks the ARM compiler. `qmk_firmware/` is in `.gitignore`, so
the QMK source never gets committed; only my files in `keyboards/gmmkpro/`
are tracked.

### 3. Get QMK Toolbox (Windows, once)

Download `qmk_toolbox.exe` from
<https://github.com/qmk/qmk_toolbox/releases>. (A copy is in
`C:\Users\John\Downloads`.)

### 4. Install the bootloader driver (Windows, once)

Windows doesn't give the keyboard's bootloader a driver by itself, so
flashing fails until it has **WinUSB**:

1. Download Zadig from <https://zadig.akeo.ie>.
2. Put the keyboard into [bootloader mode](#bootloader-mode).
3. In Zadig: **Options → List All Devices**, pick **STM32 BOOTLOADER**
   (USB ID `0483` `DF11`; don't pick anything else), choose **WinUSB**,
   click **Install Driver**.
4. Unplug and replug the keyboard.

Toolbox's **Tools → Install Drivers** (run as administrator) is an
alternative, but Zadig is more reliable.

---

## Build

From `/root/source/keyboard`:

```bash
cp -r keyboards/gmmkpro qmk_firmware/keyboards/gmmk/pro/rev1/ansi/keymaps/
qmk compile --compiledb -kb gmmk/pro/rev1/ansi -km gmmkpro
```

- The `cp` is needed because QMK only builds keymaps that are inside
  `qmk_firmware/`. Always edit `keyboards/gmmkpro/`, never the copy inside
  `qmk_firmware/`, because the copy gets overwritten on every build.
- `--compiledb` also refreshes `qmk_firmware/compile_commands.json`, which
  is what lets nvim/clangd understand `keymap.c` (see [Editor setup](#editor-setup)).
- Output: `qmk_firmware/gmmk_pro_rev1_ansi_gmmkpro.bin`. From Windows that's
  `\\wsl.localhost\Ubuntu\root\source\keyboard\qmk_firmware\`.

## Flash

WSL can't see the keyboard over USB, so flash from Windows:

1. Open **QMK Toolbox**, select the `.bin` above, and leave MCU on the default.
2. Put the keyboard into **bootloader mode** (below). Toolbox should print
   `STM32 DFU device connected (WinUSB)`. If it says **NO DRIVER**, do
   [step 4 of Getting started](#4-install-the-bootloader-driver-windows-once) first.
3. Click **Flash**. Success looks like `Download done.` /
   `File downloaded successfully`. Toolbox prints "Flash complete" even
   when it failed, so check for `No DFU capable USB device available`
   (= driver problem, nothing was flashed).
4. The keyboard restarts on its own. Add a row to [Firmware history](#firmware-history).

## Bootloader mode

- **Fn + \\** (`QK_BOOT`) while plugged in.
- Last resort: hold **Esc** while plugging in. This also resets saved
  settings (RGB, NKRO) to defaults. My lighting comes back anyway because
  it's set in the firmware.

The keyboard stops typing and the lights go off. Unplug and replug to exit
without flashing.

---

## Customizing

| File | What it's for |
|---|---|
| `keymap.c` | What each key does (layers), tap/hold functions, lighting |
| `config.h` | `#define` settings: tap-hold behavior, tapping term, Caps Word, etc. |
| `rules.mk` | Turning QMK features on/off (`CAPS_WORD_ENABLE = yes`, …) |

After any change: [Build](#build), then [Flash](#flash).

### Change what a key does

In `keymap.c`, layer `[0]` is the normal layer and `[1]` is the Fn layer.
Each row in `LAYOUT(...)` matches a physical row on the keyboard. Replace
the keycode in that spot. `_______` on the Fn layer means "same as layer 0".

Keycode list: <https://docs.qmk.fm/keycodes>

Useful tap/hold keycodes:

- `MT(MOD_MEH, KC_ESC)`: tap Esc, hold Meh (what Caps uses)
- `MT(MOD_HYPR, KC_ESC)`: same, but hold = Hyper (adds Win)
- `LCTL_T(KC_A)`: tap A, hold Ctrl (home-row mod)
- `LT(1, KC_SPC)`: tap Space, hold Fn layer

### Change tap/hold timing

In `config.h`:

- `#define TAPPING_TERM 200` sets how long (ms) a hold takes to count as a
  hold. Lower = holds register sooner, but slow taps might become holds.
- **Permissive hold** (current, `PERMISSIVE_HOLD_PER_KEY` +
  `get_permissive_hold()` in `keymap.c`): hold triggers when another key is
  pressed **and released** inside the hold. Safe for fast typing.
- **Hold on other key press** (`HOLD_ON_OTHER_KEY_PRESS_PER_KEY` +
  `get_hold_on_other_key_press()`): hold triggers the instant another key
  goes **down**. No wait at all, but fast Esc → key rolls turn into
  Meh + key. Switch to this if holding Caps + a key (e.g. a repeating
  arrow) feels laggy.

To apply either one to more keys, add a `case` to the function in `keymap.c`.

### Change the lighting

In `keymap.c`, `keyboard_post_init_user()`:

```c
rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
rgb_matrix_sethsv_noeeprom(0, 0, RGB_MATRIX_MAXIMUM_BRIGHTNESS * 3 / 4);
//                          hue, saturation, brightness (each 0–255)
```

- **White**: saturation `0`. For a color, use saturation `255` and set hue:
  red `0`, orange `21`, yellow `43`, green `85`, cyan `128`, blue `170`,
  purple `191`, pink `234`.
- **Brightness**: `* 3 / 4` is ~75%, `/ 2` is ~50%, `/ 4` is ~25%; or use a number like `128`.
- **Effect**: swap `RGB_MATRIX_SOLID_COLOR` for another one, e.g.
  `RGB_MATRIX_BREATHING`, `RGB_MATRIX_CYCLE_LEFT_RIGHT`,
  `RGB_MATRIX_SOLID_REACTIVE_SIMPLE`, `RGB_MATRIX_SPLASH`. The full list
  enabled for this board is under `rgb_matrix.animations` in
  `qmk_firmware/keyboards/gmmk/pro/info.json`. Names there are lowercase,
  e.g. `cycle_left_right` → `RGB_MATRIX_CYCLE_LEFT_RIGHT`.
- The `_noeeprom` functions mean this lighting is forced at every power-on.
  Using the versions without `_noeeprom` would *save* it instead, and then
  Fn-key changes would stick.

### Editor setup

`.clangd` in the repo root points clangd at
`qmk_firmware/compile_commands.json`, so nvim shows real errors and
completions instead of "unknown type `uint16_t`" everywhere. If those
errors come back: run the build with `--compiledb`, then `:LspRestart` in nvim.

---

## Troubleshooting

- **`qmk compile` can't find keymap `gmmkpro`**: you skipped the `cp` step.
- **Lots of red errors in nvim on `keymap.c`**: compile database missing or
  stale. Build with `--compiledb`, then `:LspRestart`.
- **Toolbox doesn't see the keyboard**: it isn't in bootloader mode. If
  Fn + \\ doesn't work (e.g. a different firmware is flashed), hold Esc
  while plugging in.
- **`NO DRIVER` / `No DFU capable USB device available`**: the bootloader
  needs the WinUSB driver; see
  [Getting started step 4](#4-install-the-bootloader-driver-windows-once).
- **Wrong target**: always build `gmmk/pro/rev1/ansi`.
- **Lighting/settings reset**: the Esc-on-plug-in reset clears saved
  settings. Lighting comes back from the firmware; anything else set with
  Fn keys needs redoing.

---

## Firmware history

| Date | What happened |
|---|---|
| 2026-02-22 | Flashed a QMK Configurator build: stock layout with Caps = tap Esc / hold Meh. |
| 2026-11-07 | Built from this repo: Caps = tap Esc / hold Meh with permissive hold, Fn + Caps = Caps Lock, solid white lighting at ~75% forced at power-on. **Not flashed yet**; update this row once flashed. |

When you flash a new build, add a row here with what changed.
