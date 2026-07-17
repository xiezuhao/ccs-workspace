#include"key.h"

volatile uint8_t status = 0;

uint8_t get_key_state(uint32_t key) {

uint32_t high_bits = DL_GPIO_readPins(KEY_PORT,key);

 if((high_bits & key) != 0) return 1;

 else return 0;
}
void GROUP1_IRQHandler(void)
{
    switch (DL_GPIO_getPendingInterrupt(KEY_PORT))
    {
        case KEY_KEY9_IIDX:
        status = (status + 1)%3;
         break;
         case KEY_KEY10_IIDX:
        status = (status + 3 -1)%3;
        break;

        default:
        break;
    }
}
