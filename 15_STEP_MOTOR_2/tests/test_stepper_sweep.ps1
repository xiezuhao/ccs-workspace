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
