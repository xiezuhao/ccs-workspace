#include "stepmotor.h"

void stepmotor_init(void)
{
    DL_GPIO_setPins(step_motor_PORT,step_motor_PWM2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_DIR2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_DCY2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_SLP2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_RST2_PIN);
}


// 方向控制
void stepmotor_dir_set(uint8_t direction, uint8_t stepper_id)
{
    if (stepper_id == 2){
        if(direction == 0){
            DL_GPIO_clearPins(step_motor_PORT, step_motor_DIR2_PIN);
        }else{
            DL_GPIO_setPins(step_motor_PORT, step_motor_DIR2_PIN);
        }
    }
}

// 步进控制
void stepmotor_step_set(uint8_t step, uint8_t stepper_id)
{
     if (stepper_id == 2){
        if(step == 0){
            DL_GPIO_clearPins(step_motor_PORT, step_motor_PWM2_PIN);
        }else{
            DL_GPIO_setPins(step_motor_PORT, step_motor_PWM2_PIN);
        }
    }
}