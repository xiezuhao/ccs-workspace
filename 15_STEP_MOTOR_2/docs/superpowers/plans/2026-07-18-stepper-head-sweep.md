# Stepper Head Sweep Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make stepper motor 2 continuously sweep left 90 degrees and right 90 degrees, pausing one second at both endpoints.

**Architecture:** Keep the existing timer-PWM and interrupt pulse counter. Expose a small busy-state query from the stepper driver so `main.c` can wait for actual motion completion before applying the one-second endpoint pause and reversing direction.

**Tech Stack:** TI MSPM0 DriverLib C, CCS generated GNU make build, PowerShell source-contract test

## Global Constraints

- Run at 60 degrees per second.
- Use the existing 0.05625 degree-per-pulse conversion.
- Do not modify SysConfig, pin assignments, driver microstepping, OLED, or UART behavior.
- Treat direction `0` as left and `1` as right; swap only these constants if the physical wiring reverses them.

---

### Task 1: Add a trustworthy motion-complete interface

**Files:**
- Create: `15_step_motor_2/tests/test_stepper_sweep.ps1`
- Modify: `15_step_motor_2/stepmotor.h`
- Modify: `15_step_motor_2/stepmotor.c`

**Interfaces:**
- Consumes: existing `stepmotor_set_angle(uint8_t angle, uint8_t stepper_id)` and timer load interrupt
- Produces: `uint8_t stepmotor_is_busy(uint8_t stepper_id)`, returning `1U` while motor 2 is moving and `0U` after its PWM timer is stopped

- [ ] **Step 1: Write the failing driver contract test**

```powershell
$projectRoot = Split-Path -Parent $PSScriptRoot
$header = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'stepmotor.h')
$driver = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'stepmotor.c')

function Assert-Match([string]$Text, [string]$Pattern, [string]$Message) {
    if ($Text -notmatch $Pattern) { throw $Message }
}

Assert-Match $header 'uint8_t\s+stepmotor_is_busy\s*\(uint8_t\s+stepper_id\s*\)\s*;' 'Missing busy-state declaration'
Assert-Match $driver 'static\s+volatile\s+uint32_t\s+step_remain_2\s*=\s*0\s*;' 'Remaining-step counter must be static volatile'
Assert-Match $driver 'static\s+volatile\s+uint8_t\s+stepper_busy_2\s*=\s*0\s*;' 'Missing volatile busy flag'
Assert-Match $driver 'uint8_t\s+stepmotor_is_busy\s*\(uint8_t\s+stepper_id\s*\)' 'Missing busy-state implementation'
Assert-Match $driver 'stepmotor_stop\(2\);\s*stepper_busy_2\s*=\s*0\s*;' 'ISR must clear busy only after stopping the timer'

Write-Output 'Stepper driver contract passed.'
```

- [ ] **Step 2: Run the test and verify it fails**

Run:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\15_step_motor_2\tests\test_stepper_sweep.ps1
```

Expected: failure containing `Missing busy-state declaration`.

- [ ] **Step 3: Implement the busy-state contract**

Add to `stepmotor.h`:

```c
uint8_t stepmotor_is_busy(uint8_t stepper_id);
```

Replace the shared counter declaration and update the angle function in `stepmotor.c`:

```c
static volatile uint32_t step_remain_2 = 0;
static volatile uint8_t stepper_busy_2 = 0;

void stepmotor_set_angle(uint8_t angle, uint8_t stepper_id)
{
    if (stepper_id == 2) {
        step_remain_2 = (uint32_t)(angle / 0.05625);
        stepper_busy_2 = (step_remain_2 > 0U) ? 1U : 0U;
        if (stepper_busy_2 != 0U) {
            stepmotor_start(stepper_id);
        }
    }
}

uint8_t stepmotor_is_busy(uint8_t stepper_id)
{
    return (stepper_id == 2) ? stepper_busy_2 : 0U;
}
```

When the timer ISR sees no remaining steps, stop the timer before clearing the flag:

```c
if (step_remain_2 == 0U) {
    stepmotor_stop(2);
    stepper_busy_2 = 0U;
    break;
}
step_remain_2--;
```

- [ ] **Step 4: Run the driver contract test**

Run the command from Step 2.

Expected: `Stepper driver contract passed.`

- [ ] **Step 5: Commit the driver interface**

```powershell
git add -- 15_step_motor_2/tests/test_stepper_sweep.ps1 15_step_motor_2/stepmotor.h 15_step_motor_2/stepmotor.c
git commit -m "feat: expose stepper motion completion"
```

### Task 2: Implement the continuous left-right sweep

**Files:**
- Modify: `15_step_motor_2/tests/test_stepper_sweep.ps1`
- Modify: `15_step_motor_2/main.c`

**Interfaces:**
- Consumes: `uint8_t stepmotor_is_busy(uint8_t stepper_id)` from Task 1
- Produces: continuous `left 90 -> pause 1 s -> right 90 -> pause 1 s` main-loop behavior

- [ ] **Step 1: Extend the source-contract test for the sweep sequence**

Append to `test_stepper_sweep.ps1` before its final success output:

```powershell
$main = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'main.c')
Assert-Match $main '#define\s+STEPPER_LEFT\s+\(0U\)' 'Left direction must be 0'
Assert-Match $main '#define\s+STEPPER_RIGHT\s+\(1U\)' 'Right direction must be 1'
Assert-Match $main '#define\s+SWEEP_ANGLE\s+\(90U\)' 'Sweep angle must be 90 degrees'
Assert-Match $main '#define\s+ENDPOINT_PAUSE_MS\s+\(1000U\)' 'Endpoint pause must be one second'
Assert-Match $main 'while\s*\(stepmotor_is_busy\(STEPPER_ID\)\s*!=\s*0U\)' 'Motion helper must wait for completion'
Assert-Match $main 'stepmotor_move_and_wait\(STEPPER_LEFT\);\s*stepmotor_move_and_wait\(STEPPER_RIGHT\);' 'Main loop must alternate left then right'
```

- [ ] **Step 2: Run the test and verify the new assertions fail**

Run:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\15_step_motor_2\tests\test_stepper_sweep.ps1
```

Expected: failure containing `Left direction must be 0`.

- [ ] **Step 3: Replace the one-way loop with a focused movement helper**

Add the constants and helper above `main()` in `main.c`:

```c
#define STEPPER_ID          (2U)
#define STEPPER_LEFT        (0U)
#define STEPPER_RIGHT       (1U)
#define SWEEP_ANGLE         (90U)
#define SWEEP_SPEED         (60U)
#define ENDPOINT_PAUSE_MS   (1000U)

static void stepmotor_move_and_wait(uint8_t direction)
{
    stepmotor_dir_set(direction, STEPPER_ID);
    stepmotor_set_angle(SWEEP_ANGLE, STEPPER_ID);
    while (stepmotor_is_busy(STEPPER_ID) != 0U) {
    }
    delay_ms(ENDPOINT_PAUSE_MS);
}
```

Initialize without directly starting the PWM timer, then use the helper in the infinite loop:

```c
stepmotor_init();
NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
step_set_speed(SWEEP_SPEED, STEPPER_ID);

while (1) {
    stepmotor_move_and_wait(STEPPER_LEFT);
    stepmotor_move_and_wait(STEPPER_RIGHT);
}
```

- [ ] **Step 4: Run the complete source-contract test**

Run the command from Step 2.

Expected: `Stepper driver contract passed.`

- [ ] **Step 5: Build the CCS Debug target**

Run:

```powershell
& 'D:\ti\ccs2050\ccs\utils\bin\gmake.exe' -C 'D:\git\code\ccs-workspace\15_step_motor_2\Debug' -j 1 all
```

Expected: `15_step_motor_2.out` links successfully with exit code `0` and no compiler errors.

- [ ] **Step 6: Commit the sweep behavior**

```powershell
git add -- 15_step_motor_2/tests/test_stepper_sweep.ps1 15_step_motor_2/main.c
git commit -m "feat: sweep stepper between endpoints"
```

### Task 3: Final verification

**Files:**
- Verify: `15_step_motor_2/main.c`
- Verify: `15_step_motor_2/stepmotor.c`
- Verify: `15_step_motor_2/stepmotor.h`
- Verify: `15_step_motor_2/tests/test_stepper_sweep.ps1`

**Interfaces:**
- Consumes: completed driver and main-loop changes from Tasks 1 and 2
- Produces: source-test and CCS-build evidence ready for flashing and physical direction confirmation

- [ ] **Step 1: Run both automated checks from a clean shell**

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\15_step_motor_2\tests\test_stepper_sweep.ps1
& 'D:\ti\ccs2050\ccs\utils\bin\gmake.exe' -C 'D:\git\code\ccs-workspace\15_step_motor_2\Debug' -j 1 all
```

Expected: contract test passes and build exits with code `0`.

- [ ] **Step 2: Confirm the worktree contains no unintended changes**

```powershell
git status --short
git diff --check HEAD~2..HEAD
```

Expected: clean status and no whitespace errors. Physical verification after flashing should show 90-degree motion to each side and a one-second pause at each endpoint.
