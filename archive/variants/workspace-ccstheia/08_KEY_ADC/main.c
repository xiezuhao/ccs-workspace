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
#include "key.h"
#include "oled.h"
#include <stdio.h>

volatile uint8_t status = 0U;

#define ADC_FULL_SCALE_COUNTS (4096UL)
#define ADC_REFERENCE_MILLIVOLTS \
    ((uint32_t) (xuanniu_ADCMEM_0_REF_VOLTAGE_V * 1000.0f))

static const char *get_status_text(uint8_t current_status)
{
    switch (current_status) {
        case 0U:
            return "status: 0";
        case 1U:
            return "status: 1";
        default:
            return "status: 2";
    }
}

int main(void)
{
    SYSCFG_DL_init();
    OLED_Init();
    OLED_ColorTurn(0);//0正常显示，1 反色显示
    OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    OLED_Clear();
    NVIC_EnableIRQ(KEY_INT_IRQN);
    DL_ADC12_startConversion(xuanniu_INST);

    while (1) {
        uint16_t adc_result;
        uint32_t adc_millivolts;
        char oled_str[24];

        delay_ms(10);

        adc_result = DL_ADC12_getMemResult(xuanniu_INST, xuanniu_ADCMEM_0);
        adc_millivolts = ((uint32_t) adc_result * ADC_REFERENCE_MILLIVOLTS +
                             (ADC_FULL_SCALE_COUNTS / 2UL)) /
                         ADC_FULL_SCALE_COUNTS;
        (void) snprintf(oled_str, sizeof(oled_str), "ADC: %lu.%03lu V",
            (unsigned long) (adc_millivolts / 1000UL),
            (unsigned long) (adc_millivolts % 1000UL));

        OLED_Clear();
        OLED_ShowString(0, 0, (u8 *) get_status_text(status), 16);
        OLED_ShowString(0, 16, (u8 *) oled_str, 16);
        OLED_Refresh();

    }
}
