#include "key.h"
#include "delay.h"

/**
 * @brief 读取按键原始电平 (未消抖)
 */
uint8_t get_key_state(uint32_t key) {
    uint32_t pins = DL_GPIO_readPins(KEY_PORT, KEY_KEY1_PB21_PIN);
    return (pins & key) ? 1 : 0;
}

/**
 * @brief 带消抖的按键按下检测
 *        低电平有效，含 20ms 消抖 + 松手检测
 */
uint8_t key_pressed(GPIO_Regs *port, uint32_t pin) {
    // 第一步：检测按下
    if (DL_GPIO_readPins(port, pin) != 0) return 0;
    // 第二步：延时消抖
    delay_ms(10);
    // 第三步：再次确认（仍在按下）
    if (DL_GPIO_readPins(port, pin) != 0) return 0;
    // 第四步：等待松手
    while (DL_GPIO_readPins(port, pin) == 0);
    // 第五步：松手后不再消抖，直接返回
    delay_ms(10);
    return 1;
}
