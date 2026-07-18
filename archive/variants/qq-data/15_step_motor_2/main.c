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

int main(void)
{
    SYSCFG_DL_init();
    // OLED_Init();
    // OLED_ColorTurn(0);//0显示正常，1反色显示
    // OLED_DisplayTurn(0);//0显示正常，1屏幕反转显示
    // OLED_Clear();
    stepmotor_init();
    NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
    stepmotor_dir_set(0,2);//设置步进电机2的方向为正转
    stepmotor_start(2);//启动步进电机2
    step_set_speed(60, 2);//设置步进电机2的速度为60角度/秒

    while (1) {
        stepmotor_dir_set(0, 2);
        stepmotor_set_angle(90, 2);//设置步进电机2转动90度
        delay_ms(2000);//延时5秒
        // stepmotor_dir_set(0,2);
        // step_set_speed(30,2);//设置步进电机2的速度为30角度/秒
        // delay_ms(1000);//延时3000毫秒
        // step_set_speed(180,2);//设置步进电机2的速度为180角度/秒
        // delay_ms(1000);//延时3000毫秒

        // stepmotor_dir_set(1,2);
        // step_set_speed(30,2);//设置步进电机2的速度为30角度/秒
        // delay_ms(1000);//延时3000毫秒
        // step_set_speed(180,2);//设置步进电机2的速度为180角度/秒
        // delay_ms(1000);//延时3000毫秒

        // Toggle the LED every 500 ms
        // char oled_str[50];
        // int int_a = 20;
        // sprintf(oled_str, (const char *)"Integer: %d", int_a);
        // OLED_ShowString(0, 46, (u8 *)oled_str, 16);
        // OLED_Refresh();
        
        // OLED_ShowString(0,0,(u8*)"Hello World!",16);
        // OLED_Refresh();
        // delay_ms(500);
        // DL_GPIO_clearPins(LED_PORT, LED_LED0_PIN);
        // DL_GPIO_clearPins(LED_PORT, LED_LED1_PIN);
        // delay_ms(500);
        // DL_GPIO_setPins(LED_PORT, LED_LED0_PIN);
        // DL_GPIO_setPins(LED_PORT, LED_LED1_PIN);
        // UART_send_string(PRINT_INST,"Hello World\n");
    }
}
