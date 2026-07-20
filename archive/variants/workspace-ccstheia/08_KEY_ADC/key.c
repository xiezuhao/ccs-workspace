#include "key.h"

extern volatile uint8_t status;

uint8_t get_key_state(uint32_t key)
{
    return (DL_GPIO_readPins(KEY_PORT, key) != 0U) ? 1U : 0U;
}

void GROUP1_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(KEY_PORT)) {
    case KEY_KEY9_IIDX:
        DL_GPIO_clearInterruptStatus(KEY_PORT, KEY_KEY9_PIN);
        status = (uint8_t) ((status + 1U) % 3U);
        break;

    default:
        break;
    }
}
