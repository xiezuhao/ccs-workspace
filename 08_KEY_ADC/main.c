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
#include <stdio.h>
#include "uart.h"

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    OLED_Clear();
    //NVIC_EnableIRQ(PRINT_INST_INT_IRQN);
    int sttus =0;

    while (1) {


        delay_ms(10);
        uint8_t key_state = get_key_state(KEY_KEY9_PIN);
        IF (key_state == 0){
status = (status+1)%3;

        }
        if(status ==0){
            OLED_Clear();
            OLED_ShowString(0, 0, (u8 *)"status: 0", 16);
            OLED_Refresh();

        }
        else if(status ==1){
            OLED_Clear();
            OLED_ShowString(0, 0, (u8 *)"status : 1", 16);
            OLED_Refresh();

    }
    else if(status ==2){
            OLED_Clear();
            OLED_ShowString(0, 0, (u8 *)"status : 2", 16);
            OLED_Refresh();
    }
        // Toggle the LED every 500 ms
        // char oled_str[50];
        // int int_a = 20;
        // sprintf(oled_str, "Integer: %d", int_a);
        // OLED_ShowString(0, 46, (u8 *)oled_str, 16);
        // OLED_Refresh();
        

        // OLED_ShowString(0, 0, (u8 *)"Hello, TI!", 16);
        // OLED_Refresh();
        //delay_ms(500);
       // DL_GPIO_clearPins(LED_PORT, LED_LED0_PIN);
        //DL_GPIO_clearPins(LED_PORT, LED_LED1_PIN);
       // delay_ms(500);
       // DL_GPIO_setPins(LED_PORT, LED_LED0_PIN);
       // DL_GPIO_setPins(LED_PORT, LED_LED1_PIN);
       // UART_send_string(PRINT_INST, "hello, ti!\n");

    }
}
