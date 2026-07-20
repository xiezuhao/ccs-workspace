#ifndef KEY_H
#define KEY_H

#include "ti_msp_dl_config.h"

/**
 * @brief 读取指定按键的原始电平 (未消抖)
 * @param key  按键引脚宏，例如 TKEY_KEY0_PIN
 * @return 1=高电平(松开), 0=低电平(按下)
 */
uint8_t get_key_state(uint32_t key);

/**
 * @brief 带消抖的按键检测 — 按下后返回 1，含松手检测
 * @param port 按键所在端口，例如 TKEY_PORT
 * @param pin  按键引脚宏，例如 TKEY_KEY0_PIN
 * @return 1=检测到完整按下, 0=未按下
 */
uint8_t key_pressed(GPIO_Regs *port, uint32_t pin);

#endif
