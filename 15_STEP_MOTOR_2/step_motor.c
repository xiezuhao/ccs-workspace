#include"step_motor.h"

void step_motor_init(void){

    DL_GPIO_setPins(STEP_MOTOR_PORT,STEP_MOTOR_RST2_PIN);
    DL_GPIO_setPins(STEP_MOTOR_PORT,STEP_MOTOR_SLP2_PIN);
    DL_GPIO_setPins(STEP_MOTOR_PORT,STEP_MOTOR_DIR2_PIN);
    DL_GPIO_setPins(STEP_MOTOR_PORT,STEP_MOTOR_DCY2_PIN);
}

//方向控制
void step_motor_dir_set(uint8_t direction, uint8_t stepper_id)
{
    if (stepper_id == 2) {
        if (direction == 0) {
            DL_GPIO_clearPins(STEP_MOTOR_PORT, STEP_MOTOR_DIR2_PIN);
        } else {
            DL_GPIO_setPins(STEP_MOTOR_PORT, STEP_MOTOR_DIR2_PIN);
        }
    }
}

void step_motor_start(uint8_t stepper_id)
{
    if (stepper_id == 2) {
        DL_Timer_startCounter(DCC_100_PWM2_INST);
    }
}
