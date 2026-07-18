#ifndef STEPMOTOR_H
#define STEPMOTOR_H

// 接线
// 第二路
// PA12 PWM
// PA13 DIR
// PA14 DCY
// PA15 SLP
// PA16 RST

// 一脉冲 0.05625度
// 角速度 = 0.05625度 * 脉冲频率
// 脉冲频率 = 角速度 / 0.05625度
// 30角速度：30 / 0.05625 = 533.33Hz

#include "ti_msp_dl_config.h"

void stepmotor_init(void);
void stepmotor_dir_set(uint8_t direction, uint8_t stepper_id);
void stepmotor_start(uint8_t stepper_id);
void step_set_speed(uint8_t speed,uint8_t stepper_id);
void stepmotor_set_angle(uint8_t angle,uint8_t stepper_id);
uint8_t stepmotor_is_busy(uint8_t stepper_id);
// void stepmotor_step_set(uint8_t step, uint8_t stepper_id);

#endif // STEPMOTOR_H
