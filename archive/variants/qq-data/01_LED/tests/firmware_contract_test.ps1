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
