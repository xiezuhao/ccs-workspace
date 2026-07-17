#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H
#include"ti_msp_dl_config.h"

// PA12   PWM
// PA13   DIR
// PA14   DCY
// PA15   SLP
// PA16   RST








void step_motor_init(void);
void step_motor_start(uint8_t stepper_id);
void step_motor_dir_set(uint8_t direction, uint8_t stepper_id);
void step_set_speed(uint8_t speed, uint8_t stepper_id);

#endif  //STEP_MOTOR_H
