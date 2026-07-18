#ifndef STEPMOTOR_H
#define STEPMOTOR_H

// 接线
// 第二路
// PA12 PWM
// PA13 DIR
// PA14 DCY
// PA15 SLP
// PA16 RST

// DRV8825的M0/M1/M2未置高，当前硬件为整步模式
// 一脉冲1.8度，脉冲频率 = 角速度 / 1.8度

#include "ti_msp_dl_config.h"

#define STEPMOTOR_STEP_ANGLE_DEG (1.8f)

void stepmotor_init(void);
void stepmotor_dir_set(uint8_t direction, uint8_t stepper_id);
void stepmotor_start(uint8_t stepper_id);
void step_set_speed(uint8_t speed,uint8_t stepper_id);
void stepmotor_set_angle(uint8_t angle,uint8_t stepper_id);
uint8_t stepmotor_is_busy(uint8_t stepper_id);
// void stepmotor_step_set(uint8_t step, uint8_t stepper_id);

#endif // STEPMOTOR_H
