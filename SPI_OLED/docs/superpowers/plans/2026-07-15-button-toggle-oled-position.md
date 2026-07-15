# PB21 Button Toggles OLED Text Position Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make each complete PB21 button press move `hello,ti` between Y coordinates 0 and 32 on the SPI OLED.

**Architecture:** Keep button polling in `main()` and reuse the existing active-low, release-waiting `key_pressed()` function. Add a framebuffer-only clear operation so a position change can clear, draw, and refresh exactly once without briefly displaying a blank frame.

**Tech Stack:** C11, TI MSPM0 DriverLib, SysConfig-generated GPIO/SPI definitions, CCS GNU Make build.

## Global Constraints

- PB21 remains an active-low input with an internal pull-up.
- The initial text is `hello,ti` at `(0, 0)` with the 16-pixel font.
- Each complete press toggles the Y coordinate between `0` and `32`.
- Holding the button does not repeatedly toggle because `key_pressed()` waits for release.
- Do not add interrupts, fonts, dependencies, or unrelated refactoring.

---

### Task 1: Wire PB21 polling to framebuffer redraw

**Files:**
- Modify: `C:/Users/x/workspace_ccstheia/SPI_OLED/empty.c:33-44`
- Modify: `C:/Users/x/workspace_ccstheia/SPI_OLED/oled.h:22-29`
- Modify: `C:/Users/x/workspace_ccstheia/SPI_OLED/oled.c:54-62`
- Test: source integration assertion plus full `SPI_OLED.out` build

**Interfaces:**
- Consumes: `uint8_t key_pressed(GPIO_Regs *port, uint32_t pin)`, `KEY_PORT`, `KEY_KEY1_PB21_PIN`, `OLED_ShowString()`, and `OLED_Refresh_Gram()`.
- Produces: `void OLED_ClearBuffer(void)` and button-driven Y-position toggling in `main()`.

- [ ] **Step 1: Run the source integration assertion and verify it fails**

```powershell
$source = Get-Content -Raw 'C:\Users\x\workspace_ccstheia\SPI_OLED\empty.c'
if ($source -notmatch '#include "key\.h"' -or
    $source -notmatch 'key_pressed\(KEY_PORT, KEY_KEY1_PB21_PIN\)' -or
    $source -notmatch 'text_y\s*=\s*\(text_y\s*==\s*0U\)\s*\?\s*32U\s*:\s*0U') {
    throw 'PB21 OLED position toggle is not implemented'
}
```

Expected: command fails with `PB21 OLED position toggle is not implemented`.

- [ ] **Step 2: Add a framebuffer-only clear API**

Add this declaration to `oled.h`:

```c
void OLED_ClearBuffer(void);
```

Replace the current `OLED_Clear()` body in `oled.c` with:

```c
void OLED_ClearBuffer(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0x00;
}

void OLED_Clear(void)
{
    OLED_ClearBuffer();
    OLED_Refresh_Gram();
}
```

- [ ] **Step 3: Implement the PB21 position toggle in `main()`**

Use this application flow in `empty.c`:

```c
#include "ti_msp_dl_config.h"
#include "oled.h"
#include "key.h"

int main(void)
{
    uint8_t text_y = 0U;

    SYSCFG_DL_init();
    OLED_Init();
    OLED_ShowString(0, text_y, "hello,ti", 16);
    OLED_Refresh_Gram();

    while (1) {
        if (key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN)) {
            text_y = (text_y == 0U) ? 32U : 0U;
            OLED_ClearBuffer();
            OLED_ShowString(0, text_y, "hello,ti", 16);
            OLED_Refresh_Gram();
        }
    }
}
```

- [ ] **Step 4: Re-run the source integration assertion**

Run the Step 1 PowerShell assertion again.

Expected: exit code `0` with no exception.

- [ ] **Step 5: Build the complete CCS target**

Run:

```powershell
& 'D:\ti\ccs2050\ccs\utils\bin\gmake.exe' -j4 all
```

Working directory: `C:\Users\x\workspace_ccstheia\SPI_OLED\Debug`

Expected: exit code `0` and `Finished building target: "SPI_OLED.out"`.

- [ ] **Step 6: Hardware acceptance after flashing**

Flash `Debug/SPI_OLED.out`. Confirm startup text is at Y=0, one PB21 press moves it to Y=32, the next press returns it to Y=0, and holding PB21 causes only one move.

- [ ] **Step 7: Commit if the directory is later placed under Git**

```bash
git add empty.c oled.c oled.h docs/superpowers/specs/2026-07-15-button-toggle-oled-position-design.md docs/superpowers/plans/2026-07-15-button-toggle-oled-position.md
git commit -m "feat: toggle OLED text position with PB21"
```

The current directory is not a Git repository, so this step is intentionally skipped in the present environment.
