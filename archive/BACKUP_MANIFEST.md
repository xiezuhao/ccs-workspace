# CCS project backup manifest

Backup date: 2026-07-18 (Asia/Shanghai)

This repository contains every user CCS project instance found on the local
fixed drives. Recycle Bin contents and the two example projects installed with
Code Composer Studio were intentionally excluded.

The scan found 39 user project instances and 25 distinct source snapshots.
Identical copies were deduplicated. Build outputs (`Debug`, `Release`, generated
`.clangd` directories, compiler objects, linker maps, logs, and temporary files)
are not stored because CCS can regenerate them. Project-level `.clangd`
configuration files are preserved.

## Canonical projects

The following ten distinct projects remain at the repository root:

- `01_LED`
- `02_CLOCK_GPIO`
- `03_Debug_BLOCK`
- `05_OLED`
- `06_uart`
- `08_KEY_ADC`
- `09_PWM_SERVO`
- `14_step_motor_1`
- `15_step_motor_2`
- `SPI_OLED`

## Preserved variants

Fifteen additional, non-identical snapshots are preserved under
`archive/variants`. The source-to-backup mapping is:

| Backup path | Original source |
| --- | --- |
| `archive/variants/desktop-temp/01_LED_completed` | `C:\Users\x\Desktop\临时文件\2026_04_地猛星电赛控制题配套资料\01_LED_completed\01_LED_completed` |
| `archive/variants/desktop-temp/06_uart` | `C:\Users\x\Desktop\临时文件\2026_04_地猛星电赛控制题配套资料\05_OLED_completed\06_uart` |
| `archive/variants/desktop-temp/0000` | `C:\Users\x\Desktop\临时文件\2026_04_地猛星电赛控制题配套资料\06_uart_已完成\0000` |
| `archive/variants/desktop-temp/AAA` | `C:\Users\x\Desktop\临时文件\2026_04_地猛星电赛控制题配套资料\08_KEY_ADC_已完成\AAA` |
| `archive/variants/desktop-work/06_uart` | `C:\Users\x\Desktop\work\06_uart` |
| `archive/variants/workspace-ccstheia/06_uart` | `C:\Users\x\workspace_ccstheia\06_uart` |
| `archive/variants/workspace-ccstheia/08_KEY_ADC` | `C:\Users\x\workspace_ccstheia\08_KEY_ADC` |
| `archive/variants/qq-data/01_LED` | `D:\qq数据\05_OLED\01_LED` |
| `archive/variants/qq-data/01LED` | `D:\qq数据\05_OLED\01LED` |
| `archive/variants/qq-data/02_CLOCK_GPIO` | `D:\qq数据\05_OLED\02_CLOCK_GPIO` |
| `archive/variants/qq-data/05_OLED` | `D:\qq数据\05_OLED\05_OLED` |
| `archive/variants/qq-data/06_uart` | `D:\qq数据\05_OLED\06_uart` |
| `archive/variants/qq-data/14_step_motor_1` | `D:\qq数据\05_OLED\14_step_motor_1` |
| `archive/variants/qq-data/15_step_motor_2` | `D:\qq数据\05_OLED\15_step_motor_2` |
| `archive/variants/qq-data/SPI_OLED` | `D:\qq数据\05_OLED\SPI_OLED` |

Other scanned locations were byte-for-byte duplicates (after excluding build
outputs) of one of the snapshots above or of a canonical root project.
