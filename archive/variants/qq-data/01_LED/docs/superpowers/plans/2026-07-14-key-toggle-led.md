# Key Toggle LED Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make MSP-LITO-G3507-EVM D1 start off and toggle exactly once after every complete S2 press-and-release.

**Architecture:** `key.c` converts the active-low PB21 signal into a debounced one-shot event. `empty.c` owns application behavior: it initializes PB14 low and toggles PB14 only when `key_pressed()` reports one complete event. SysConfig supplies PB21's required internal pull-up and generates the target pin configuration.

**Tech Stack:** C11-style embedded C, TI MSPM0 DriverLib/SDK 2.10.00.04, SysConfig 1.26.2, TI Arm Clang 4.0.4.LTS, CCS 20.5.0, MSVC host compiler, PowerShell contract checks.

## Global Constraints

- D1 is PB14, active high, and must be off after initialization.
- S2 is PB21, active low, and must use the MCU internal pull-up.
- One complete press-and-release produces one LED toggle; holding S2 must not repeat.
- Keep the existing blocking debounce approach: 10 ms press confirmation, wait for release, then 10 ms release debounce.
- Do not introduce GPIO interrupts, a non-blocking state machine, or unrelated refactoring.
- The project has no `.git` directory. Do not initialize Git; replace commit steps with verification checkpoints.

## File Structure

- Modify `tests/key_test.c.test`: executable host-side tests for raw reads, bounce rejection, and one-shot press detection.
- Modify `key.c`: correct raw-pin reading while preserving the debounced event interface.
- Create `tests/firmware_contract_test.ps1`: verify the exact SysConfig and main-loop wiring before target compilation.
- Modify `empty.syscfg`: enable the PB21 internal pull-up.
- Modify `empty.c`: replace periodic blinking with initialization-off plus event-driven toggling.
- Generated under `Debug/`: SysConfig sources, makefiles, objects, and `01_LED.out`; never hand-edit generated files.

---

### Task 1: Verify and Correct the Key Module

**Files:**
- Modify: `tests/key_test.c.test`
- Modify: `key.c:7-10`

**Interfaces:**
- Consumes: `DL_GPIO_readPins(GPIO_Regs *port, uint32_t pins)` and `delay_ms(uint32_t ms)`.
- Produces: `uint8_t get_key_state(uint32_t key)` and `uint8_t key_pressed(GPIO_Regs *port, uint32_t pin)`.

- [ ] **Step 1: Replace the host test with complete behavior tests**

```c
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define KEY_H
#define DELAY_H

typedef struct {
    uint32_t unused;
} GPIO_Regs;

static GPIO_Regs mock_key_port;
#define KEY_PORT (&mock_key_port)
#define KEY_KEY1_PB21_PIN (1u << 21)

static uint32_t read_script[16];
static size_t read_count;
static size_t read_index;
static uint32_t delay_call_count;
static uint32_t delay_total_ms;

static void set_read_script(const uint32_t *values, size_t count)
{
    size_t index;
    for (index = 0; index < count; ++index) {
        read_script[index] = values[index];
    }
    read_count = count;
    read_index = 0;
    delay_call_count = 0;
    delay_total_ms = 0;
}

static uint32_t DL_GPIO_readPins(GPIO_Regs *port, uint32_t pins)
{
    uint32_t value;
    (void) port;
    value = read_script[(read_index < read_count) ? read_index++ : (read_count - 1)];
    return value & pins;
}

static void delay_ms(uint32_t ms)
{
    ++delay_call_count;
    delay_total_ms += ms;
}

#include "../key.c"

#define CHECK(condition, message) \
    do { \
        if (!(condition)) { \
            fputs("FAIL: " message "\n", stderr); \
            return 1; \
        } \
    } while (0)

int main(void)
{
    const uint32_t requested_pin = 1u << 3;
    const uint32_t released[] = {KEY_KEY1_PB21_PIN};
    const uint32_t bounce[] = {0u, KEY_KEY1_PB21_PIN};
    const uint32_t complete_press[] = {0u, 0u, 0u, KEY_KEY1_PB21_PIN};
    const uint32_t requested_high[] = {requested_pin};
    const uint32_t requested_low[] = {0u};

    set_read_script(requested_high, 1);
    CHECK(get_key_state(requested_pin) == 1u,
          "get_key_state did not read the requested pin");

    set_read_script(requested_low, 1);
    CHECK(get_key_state(requested_pin) == 0u,
          "get_key_state did not report an active-low press");

    set_read_script(released, 1);
    CHECK(key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN) == 0u,
          "released key produced an event");
    CHECK(delay_call_count == 0u, "released key was unnecessarily delayed");

    set_read_script(bounce, 2);
    CHECK(key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN) == 0u,
          "bounce produced an event");
    CHECK(delay_call_count == 1u && delay_total_ms == 10u,
          "press debounce timing is incorrect");

    set_read_script(complete_press, 4);
    CHECK(key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN) == 1u,
          "complete press-and-release did not produce an event");
    CHECK(delay_call_count == 2u && delay_total_ms == 20u,
          "complete event debounce timing is incorrect");

    set_read_script(released, 1);
    CHECK(key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN) == 0u,
          "one completed press produced a second event");

    puts("PASS: key module");
    return 0;
}
```

- [ ] **Step 2: Compile and run the host test to verify RED**

Run from `C:\Users\x\workspace_ccstheia\01_LED`:

```powershell
New-Item -ItemType Directory -Force 'C:\Users\x\Documents\Codex\2026-07-14\xai\work' | Out-Null
cmd /c 'call "E:\新建文件夹\VC\Auxiliary\Build\vcvars64.bat" >nul && cl /nologo /W4 /TC tests\key_test.c.test /Fo:"C:\Users\x\Documents\Codex\2026-07-14\xai\work\key_test.obj" /Fe:"C:\Users\x\Documents\Codex\2026-07-14\xai\work\key_test.exe"'
& 'C:\Users\x\Documents\Codex\2026-07-14\xai\work\key_test.exe'
```

Expected: compilation succeeds, executable exits 1, and prints `FAIL: get_key_state did not read the requested pin`. This proves the test detects the current hard-coded PB21 bug.

- [ ] **Step 3: Make the minimal production correction**

Replace `get_key_state()` in `key.c` with:

```c
uint8_t get_key_state(uint32_t key)
{
    return (DL_GPIO_readPins(KEY_PORT, key) != 0u) ? 1u : 0u;
}
```

Do not change `key_pressed()` unless the test exposes a separate failure.

- [ ] **Step 4: Compile and run the host test to verify GREEN**

Run the two commands from Step 2 again.

Expected: compiler exits 0 with no warnings, executable exits 0, and prints `PASS: key module`.

- [ ] **Step 5: Verification checkpoint**

Review only `key.c` and `tests/key_test.c.test`. Confirm no production API was added and no unrelated files changed. No commit is possible because the project is not a Git repository.

---

### Task 2: Configure the Pull-Up and Wire the Main Loop

**Files:**
- Create: `tests/firmware_contract_test.ps1`
- Modify: `empty.syscfg:38-41`
- Modify: `empty.c:33-45`

**Interfaces:**
- Consumes: `key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN)` from Task 1.
- Produces: an application loop that calls `DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN)` once per reported event.

- [ ] **Step 1: Write a failing firmware contract test**

```powershell
$root = Split-Path $PSScriptRoot -Parent
$syscfg = Get-Content -LiteralPath (Join-Path $root 'empty.syscfg') -Raw
$main = Get-Content -LiteralPath (Join-Path $root 'empty.c') -Raw

$checks = [ordered]@{
    'LED is assigned to PB14' = $syscfg -match 'pin\.\$assign\s*=\s*"PB14"'
    'KEY is assigned to PB21' = $syscfg -match 'pin\.\$assign\s*=\s*"PB21"'
    'KEY uses internal pull-up' = $syscfg -match 'GPIO2\.associatedPins\[0\]\.internalResistor\s*=\s*"PULL_UP"'
    'main includes key.h' = $main -match '#include\s+"key\.h"'
    'main explicitly starts LED off' = $main -match 'DL_GPIO_clearPins\s*\(\s*LED_PORT\s*,\s*LED_LED0_PIN\s*\)'
    'main polls the configured key' = $main -match 'key_pressed\s*\(\s*KEY_PORT\s*,\s*KEY_KEY1_PB21_PIN\s*\)'
    'main toggles the configured LED' = $main -match 'DL_GPIO_togglePins\s*\(\s*LED_PORT\s*,\s*LED_LED0_PIN\s*\)'
    'old periodic blink is removed' = $main -notmatch 'delay_cycles\s*\(\s*8000000\s*\)'
}

$failed = @($checks.GetEnumerator() | Where-Object { -not $_.Value })
if ($failed.Count -ne 0) {
    $failed | ForEach-Object { Write-Error ("FAIL: " + $_.Key) }
    exit 1
}

Write-Output 'PASS: firmware contract'
```

- [ ] **Step 2: Run the contract test to verify RED**

```powershell
& 'C:\Users\x\workspace_ccstheia\01_LED\tests\firmware_contract_test.ps1'
```

Expected: exit 1. Failures must include internal pull-up, `key.h`, key polling, LED toggling, and removal of the old periodic blink.

- [ ] **Step 3: Enable PB21's internal pull-up in SysConfig**

Add this line after the PB21 direction setting and before its pin assignment in `empty.syscfg`:

```javascript
GPIO2.associatedPins[0].internalResistor = "PULL_UP";
```

Keep PB14 and PB21 assignments unchanged.

- [ ] **Step 4: Replace periodic blinking with event-driven toggling**

Keep the existing license header, then use this complete application body in `empty.c`:

```c
#include "ti_msp_dl_config.h"
#include "key.h"

int main(void)
{
    SYSCFG_DL_init();
    DL_GPIO_clearPins(LED_PORT, LED_LED0_PIN);

    while (1) {
        if (key_pressed(KEY_PORT, KEY_KEY1_PB21_PIN)) {
            DL_GPIO_togglePins(LED_PORT, LED_LED0_PIN);
        }
    }
}
```

- [ ] **Step 5: Run the contract test to verify GREEN**

Run the command from Step 2 again.

Expected: exit 0 and `PASS: firmware contract`.

- [ ] **Step 6: Verification checkpoint**

Review `empty.c`, `empty.syscfg`, and `tests/firmware_contract_test.ps1`. Confirm the main loop has no timing-based LED changes and only toggles after `key_pressed()` returns 1. No commit is possible because the project is not a Git repository.

---

### Task 3: Regenerate, Build, and Verify the Target Firmware

**Files:**
- Regenerate: `Debug/ti_msp_dl_config.c`
- Regenerate: `Debug/ti_msp_dl_config.h`
- Regenerate: `Debug/makefile`, `Debug/subdir_rules.mk`, `Debug/subdir_vars.mk`
- Generate: `Debug/01_LED.out`

**Interfaces:**
- Consumes: all source and SysConfig changes from Tasks 1 and 2.
- Produces: a linked MSPM0G3507 image containing `empty.o`, `key.o`, `delay.o`, and generated SysConfig objects.

- [ ] **Step 1: Run a full CCS project build**

```powershell
& 'D:\ti\ccs2050\ccs\eclipse\ccs-server-cli.bat' `
    -workspace 'C:\Users\x\workspace_ccstheia' `
    -application projectBuild `
    -ccs.projects 01_LED `
    -ccs.buildType full `
    -ccs.configuration Debug `
    -ccs.listProblems
```

Expected: exit 0, SysConfig validation succeeds, `key.c` and `delay.c` compile, linker finishes `01_LED.out`, and no build errors are listed.

- [ ] **Step 2: Verify generated configuration and linked inputs**

```powershell
Select-String -LiteralPath 'C:\Users\x\workspace_ccstheia\01_LED\Debug\ti_msp_dl_config.c' -Pattern 'DL_GPIO_RESISTOR_PULL_UP'
Select-String -LiteralPath 'C:\Users\x\workspace_ccstheia\01_LED\Debug\ti_msp_dl_config.h' -Pattern 'GPIOB\.14|GPIOB\.21'
Select-String -LiteralPath 'C:\Users\x\workspace_ccstheia\01_LED\Debug\subdir_vars.mk' -Pattern 'key\.c|delay\.c|key\.o|delay\.o'
Get-Item -LiteralPath 'C:\Users\x\workspace_ccstheia\01_LED\Debug\01_LED.out' | Select-Object FullName, Length, LastWriteTime
```

Expected: the generated GPIO input uses `DL_GPIO_RESISTOR_PULL_UP`; header reports GPIOB.14 and GPIOB.21; build variables contain key and delay sources/objects; `01_LED.out` has the current build timestamp.

- [ ] **Step 3: Re-run all automated checks**

Recompile and run `tests/key_test.c.test` using Task 1 Step 2, then run:

```powershell
& 'C:\Users\x\workspace_ccstheia\01_LED\tests\firmware_contract_test.ps1'
```

Expected: `PASS: key module` and `PASS: firmware contract`, with both processes exiting 0.

- [ ] **Step 4: Load and run the current target image**

In CCS, load `C:\Users\x\workspace_ccstheia\01_LED\Debug\01_LED.out`. If execution stops at `main()`, press F8/Resume. Do not load the stale `Debug\empty.out` from the older project state.

Expected: D1 near the two buttons is off immediately after reset; D2 remains on because it is the power LED.

- [ ] **Step 5: Perform physical acceptance checks**

Press and release S2 once: D1 turns on. Press and release again: D1 turns off. Hold S2 for at least one second: D1 changes at most once and does not repeatedly toggle. Release S2 and press again: D1 toggles once.

- [ ] **Step 6: Final verification checkpoint**

Record automated outputs, full-build exit code, generated pin/resistor evidence, output image path/timestamp, and whether the user observed all four physical acceptance checks. Do not claim physical success until the board observation is reported.
