#ifndef STEPMOTOR_H
#define STEPMOTOR_H

#include "ti_msp_dl_config.h"

void stepmotor_init(void);
void stepmotor_dir_set(uint8_t direction, uint8_t stepper_id);
void stepmotor_step_set(uint8_t step, uint8_t stepper_id);

#endif // STEPMOTOR_H