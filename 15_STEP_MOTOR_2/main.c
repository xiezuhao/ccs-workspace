/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "delay.h"
#include "oled.h"
#include "stdio.h"
#include "uart.h"
#include "stepmotor.h"

#define STEPPER_ID          (2U)
#define STEPPER_LEFT        (0U)
#define STEPPER_RIGHT       (1U)
#define SWEEP_ANGLE         (90U)
#define SWEEP_SPEED         (60U)
#define ENDPOINT_PAUSE_MS   (1000U)

static void stepmotor_move_and_wait(uint8_t direction)
{
    stepmotor_dir_set(direction, STEPPER_ID);
    stepmotor_set_angle(SWEEP_ANGLE, STEPPER_ID);
    while (stepmotor_is_busy(STEPPER_ID) != 0U) {
    }
    delay_ms(ENDPOINT_PAUSE_MS);
}

int main(void)
{
    SYSCFG_DL_init();
    // OLED_Init();
    // OLED_ColorTurn(0);//0显示正常，1反色显示
    // OLED_DisplayTurn(0);//0显示正常，1屏幕反转显示
    // OLED_Clear();
    stepmotor_init();
    NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
    step_set_speed(SWEEP_SPEED, STEPPER_ID);

    while (1) {
        stepmotor_move_and_wait(STEPPER_LEFT);
        stepmotor_move_and_wait(STEPPER_RIGHT);
    }
}
