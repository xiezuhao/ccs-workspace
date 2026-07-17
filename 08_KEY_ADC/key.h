#ifndef KEY_H
#define KEY_H

#include"ti_msp_dl_config.h"

extern volatile uint8_t status;

uint8_t get_key_state(uint32_t key);


#endif