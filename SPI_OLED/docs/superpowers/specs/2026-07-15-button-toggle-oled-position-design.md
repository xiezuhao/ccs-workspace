# PB21 Button Toggles OLED Text Position

## Goal

Pressing the PB21 button once moves `hello,ti` between the top and lower portions of the 128x64 SPI OLED.

## Behavior

- PB21 is an active-low input with an internal pull-up.
- On startup, initialize the board and OLED, then display `hello,ti` at `(0, 0)` using the 16-pixel font.
- Poll the button in the main loop using the existing `key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN)` debounce function.
- After each complete button press, toggle the text Y coordinate between `0` and `32`.
- For each position change, clear the framebuffer, draw the text once, and refresh the OLED once.
- Holding the button must not cause repeated position changes; the existing function waits for release.

## Scope

- Modify only the application flow needed for the button-driven position change.
- Reuse the existing OLED, key, delay, GPIO, and SPI configuration.
- Do not add interrupts, new fonts, or unrelated refactoring.

## Verification

- Run a complete CCS build and require successful compilation and linking.
- Confirm the generated output contains the PB21 key configuration.
- Hardware acceptance: after flashing, each full PB21 press alternates the text between the two positions.
