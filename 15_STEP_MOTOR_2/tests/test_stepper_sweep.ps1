$projectRoot = Split-Path -Parent $PSScriptRoot
$header = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'stepmotor.h')
$driver = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'stepmotor.c')
$syscfg = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'empty.syscfg')

function Assert-Match([string]$Text, [string]$Pattern, [string]$Message) {
    if ($Text -notmatch $Pattern) { throw $Message }
}

Assert-Match $header 'uint8_t\s+stepmotor_is_busy\s*\(uint8_t\s+stepper_id\s*\)\s*;' 'Missing busy-state declaration'
Assert-Match $driver 'static\s+volatile\s+uint32_t\s+step_remain_2\s*=\s*0\s*;' 'Remaining-step counter must be static volatile'
Assert-Match $driver 'static\s+volatile\s+uint8_t\s+stepper_busy_2\s*=\s*0\s*;' 'Missing volatile busy flag'
Assert-Match $driver 'uint8_t\s+stepmotor_is_busy\s*\(uint8_t\s+stepper_id\s*\)' 'Missing busy-state implementation'
Assert-Match $driver 'stepmotor_stop\(2\);\s*stepper_busy_2\s*=\s*0\s*;' 'ISR must clear busy only after stopping the timer'
Assert-Match $header '#define\s+STEPMOTOR_STEP_ANGLE_DEG\s+\(1\.8f\)' 'Driver must use the motor full-step angle of 1.8 degrees'
Assert-Match $driver 'speed\s*/\s*STEPMOTOR_STEP_ANGLE_DEG' 'Speed conversion must use the configured step angle'
Assert-Match $driver 'angle\s*/\s*STEPMOTOR_STEP_ANGLE_DEG' 'Angle conversion must use the configured step angle'
Assert-Match $syscfg 'PWM1\.clockPrescale\s*=\s*3\s*;' 'PWM timer prescale must keep the 60-degree-per-second period within 16 bits'

$main = Get-Content -Raw -LiteralPath (Join-Path $projectRoot 'main.c')
Assert-Match $main '#define\s+STEPPER_LEFT\s+\(0U\)' 'Left direction must be 0'
Assert-Match $main '#define\s+STEPPER_RIGHT\s+\(1U\)' 'Right direction must be 1'
Assert-Match $main '#define\s+SWEEP_ANGLE\s+\(90U\)' 'Sweep angle must be 90 degrees'
Assert-Match $main '#define\s+ENDPOINT_PAUSE_MS\s+\(1000U\)' 'Endpoint pause must be one second'
Assert-Match $main 'while\s*\(stepmotor_is_busy\(STEPPER_ID\)\s*!=\s*0U\)' 'Motion helper must wait for completion'
Assert-Match $main 'stepmotor_move_and_wait\(STEPPER_LEFT\);\s*stepmotor_move_and_wait\(STEPPER_RIGHT\);' 'Main loop must alternate left then right'

Write-Output 'Stepper sweep contract passed.'
