#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H
#include"ti_msp_dl_config.h"

void step_motor_init(void);
void step_motor_step_set(uint8_t step,uint8_t stepper_id);
void step_motor_dir_set(uint8_t dirction, uint8_t stepper_id);

#endif  //STEP_MOTOR_H