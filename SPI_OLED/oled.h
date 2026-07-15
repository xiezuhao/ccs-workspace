#ifndef __OLED_H
#define __OLED_H

#include "ti_msp_dl_config.h"
#include "stdlib.h"

// ==================== OLED 引脚宏 (SysConfig 自动生成) ====================
// RES: PA8  DC: PB2  CS: PA7

#define OLED_RES_Set()  DL_GPIO_setPins(SPI_OLED_SPI_OLED_RES_PORT , SPI_OLED_SPI_OLED_RES_PIN)
#define OLED_RES_Clr()  DL_GPIO_clearPins(SPI_OLED_SPI_OLED_RES_PORT, SPI_OLED_SPI_OLED_RES_PIN)

#define OLED_DC_Set()   DL_GPIO_setPins(SPI_OLED_SPI_OLED_DC_PORT, SPI_OLED_SPI_OLED_DC_PIN)
#define OLED_DC_Clr()   DL_GPIO_clearPins(SPI_OLED_SPI_OLED_DC_PORT, SPI_OLED_SPI_OLED_DC_PIN)

#define OLED_CS_Set()   DL_GPIO_setPins(SPI_OLED_SPI_OLED_CS_PORT, SPI_OLED_SPI_OLED_CS_PIN)
#define OLED_CS_Clr()   DL_GPIO_clearPins(SPI_OLED_SPI_OLED_CS_PORT, SPI_OLED_SPI_OLED_CS_PIN)

#define OLED_CMD  0   // 写命令
#define OLED_DATA 1   // 写数据

// ==================== OLED 控制函数 ====================
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Init(void);
void OLED_ClearBuffer(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);
void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size, uint8_t mode);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);
void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size);
void OLED_ShowGBK(uint8_t x, uint8_t y, uint8_t num, uint8_t size, uint8_t mode);
void OLED_ShowGBK_String(uint8_t x, uint8_t y, uint8_t num, uint8_t size, uint8_t mode);
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t index, uint8_t clom, uint8_t row, uint8_t mode);
void OLED_ShowGIF(uint8_t x, uint8_t y, uint8_t index, uint8_t clom, uint8_t row, uint8_t mode);
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t len, uint8_t dec_places, uint8_t size);

#endif
