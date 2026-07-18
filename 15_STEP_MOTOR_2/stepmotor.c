#include "stepmotor.h"

void stepmotor_init(void)
{
    DL_GPIO_setPins(step_motor_PORT,step_motor_DIR2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_DCY2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_SLP2_PIN);
    DL_GPIO_setPins(step_motor_PORT,step_motor_RST2_PIN);
    NVIC_EnableIRQ(DCC_100_PWM2_INST_INT_IRQN);
}

void stepmotor_start(uint8_t stepper_id)
{
    DL_Timer_startCounter(DCC_100_PWM2_INST);
}

void stepmotor_stop(uint8_t stepper_id)
{
    DL_Timer_stopCounter(DCC_100_PWM2_INST);
}

// 角速度设置 角度/s
void step_set_speed(uint8_t speed,uint8_t stepper_id)
{
    if(stepper_id == 2){
        //根据速度设置PWM频率
        uint32_t frequency =
            (uint32_t)(speed / STEPMOTOR_STEP_ANGLE_DEG);//计算所需的PWM频率
        frequency = frequency > 0 ? frequency:1;
        // float period_sec = 1.0f/frequency;
        // 1/DCC_100_PWM2_INST_CLK_FREQ
        // period_sec/(1/DCC_100_PWM2_INST_CLK_FREQ)
        //计算定时器溢出值
        uint32_t period = DCC_100_PWM2_INST_CLK_FREQ / frequency; 
        period = period < 65536 ? period : 65536;
        period = period > 800 ? period : 800;
        DL_Timer_setLoadValue(DCC_100_PWM2_INST, period);
        DL_Timer_setCaptureCompareValue(DCC_100_PWM2_INST, period / 2,GPIO_DCC_100_PWM2_C0_IDX);//设置占空比为50%
    }
}


static volatile uint32_t step_remain_2 = 0;
static volatile uint8_t stepper_busy_2 = 0;

void stepmotor_set_angle(uint8_t angle,uint8_t stepper_id)
{
    if (stepper_id == 2){
        //根据角度设置步数
        step_remain_2 =
            (uint32_t)(angle / STEPMOTOR_STEP_ANGLE_DEG);//计算所需步数
        stepper_busy_2 = (step_remain_2 > 0U) ? 1U : 0U;
        if (stepper_busy_2 != 0U) {
            stepmotor_start(stepper_id);
        }
    }
}

uint8_t stepmotor_is_busy(uint8_t stepper_id)
{
    return (stepper_id == 2) ? stepper_busy_2 : 0U;
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
        delay_cycles(
            (CPUCLK_FREQ / 1000000U) * STEPMOTOR_DIR_SETUP_US);
    }
}

void DCC_100_PWM2_INST_IRQHandler()
{
    switch (DL_Timer_getPendingInterrupt(DCC_100_PWM2_INST))
     {
    case DL_TIMER_IIDX_LOAD:
        {  
            if (step_remain_2 == 0U){
                stepmotor_stop(2);
                stepper_busy_2 = 0;
                break;
             }
             step_remain_2--;
             break;
        }

        default:
            break;
}
}

// // 步进控制
// void stepmotor_step_set(uint8_t step, uint8_t stepper_id)
// {
//      if (stepper_id == 2){
//         if(step == 0){
//             DL_GPIO_clearPins(step_motor_PORT, step_motor_PWM2_PIN);
//         }else{
//             DL_GPIO_setPins(step_motor_PORT, step_motor_PWM2_PIN);
//         }
//     }
// }
