#include"key.h"

uint8_t get_key_state(uint32_t key){
uint32_t state = DL_GPIO_readPins(KEY_PORT,key);
 if(high_bits & key ! = 0) return 1 ;
 else return 0;



  
}