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
void step_set_speed(uint8_t speed, uint8_t stepper_id)
{
    if (stepper_id == 2) {
        // 根据速度设置PWM频率
        uint32_t frequency = (uint32_t) (speed / 0.05625f);
        frequency = frequency > 0U ? frequency : 1U;

        uint32_t period = DCC_100_PWM2_INST_CLK_FREQ / frequency;
        DL_Timer_setLoadValue(DCC_100_PWM2_INST, period);
        DL_Timer_setCaptureCompareValue(
            DCC_100_PWM2_INST, period / 2U, DL_TIMER_CC_0_INDEX);
    }
}
