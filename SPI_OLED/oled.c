#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

// ==================== OLED 显存 ====================
// [0]0 1 2 3 ... 127
// [1]0 1 2 3 ... 127
// ...
// [7]0 1 2 3 ... 127
uint8_t OLED_GRAM[128][8];

// ==================== SPI 写一个字节 ====================
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
    if (cmd) OLED_DC_Set();   // 数据
    else     OLED_DC_Clr();   // 命令

    OLED_CS_Clr();                                  // 片选拉低
    DL_SPI_transmitDataBlocking8(SPI_0_INST, dat);  // SPI 阻塞发送
    OLED_CS_Set();                                  // 片选拉高
    OLED_DC_Set();                                  // 恢复 DC 为数据模式
}

// ==================== 更新显存到 OLED ====================
void OLED_Refresh_Gram(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);  // 设置页地址 (0~7)
        OLED_WR_Byte(0x00, OLED_CMD);      // 设置列低地址
        OLED_WR_Byte(0x10, OLED_CMD);      // 设置列高地址
        for (n = 0; n < 128; n++)
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}

// ==================== 开启 OLED 显示 ====================
void OLED_Display_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  // 电荷泵使能
    OLED_WR_Byte(0x14, OLED_CMD);  // 开启电荷泵
    OLED_WR_Byte(0xAF, OLED_CMD);  // 点亮屏幕
}

// ==================== 关闭 OLED 显示 ====================
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  // 电荷泵使能
    OLED_WR_Byte(0x10, OLED_CMD);  // 关闭电荷泵
    OLED_WR_Byte(0xAE, OLED_CMD);  // 关闭屏幕
}

// ==================== 清屏 ====================
void OLED_ClearBuffer(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
        for (n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0x00;
}

void OLED_Clear(void)
{
    OLED_ClearBuffer();
    OLED_Refresh_Gram();
}

// ==================== 画点 ====================
// x: 0~127 (列), y: 0~63 (行), t: 1=填充 0=清除
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t pos, bx, temp = 0;
    if (x > 127 || y > 63) return;
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t) OLED_GRAM[x][pos] |= temp;
    else   OLED_GRAM[x][pos] &= ~temp;
}

// ==================== 矩形填充 ====================
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;
    for (x = x1; x <= x2; x++)
        for (y = y1; y <= y2; y++)
            OLED_DrawPoint(x, y, dot);
    OLED_Refresh_Gram();
}

// ==================== 显示字符 ====================
// size: 12/16/24, mode: 0=反白 1=正常
void OLED_ShowChar(uint8_t x, uint8_t y, char chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    chr = chr - ' ';
    for (t = 0; t < csize; t++) {
        if (size == 12)      temp = asc2_1206[(uint8_t)chr][t];
        else if (size == 16) temp = asc2_1608[(uint8_t)chr][t];
        else if (size == 24) temp = asc2_2412[(uint8_t)chr][t];
        else return;

        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80) OLED_DrawPoint(x, y, mode);
            else             OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == size) { y = y0; x++; break; }
        }
    }
}

// ==================== m^n 辅助函数 ====================
static uint32_t mypow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

// ==================== 显示数字 ====================
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                OLED_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
                continue;
            } else enshow = 1;
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
    }
}

// ==================== 显示字符串 ====================
void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' ')) {
        if (x > (128 - (size / 2))) { x = 0; y += size; }
        if (y > (64 - size)) { y = x = 0; OLED_Clear(); }
        OLED_ShowChar(x, y, *p, size, 1);
        x += size / 2;
        p++;
    }
}

// ==================== OLED 初始化 ====================
void OLED_Init(void)
{
    // 硬件复位
    OLED_RES_Clr();
    delay_ms(100);
    OLED_RES_Set();
    delay_ms(100);

    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    OLED_WR_Byte(0xD5, OLED_CMD); // 设置时钟分频因子
    OLED_WR_Byte(80, OLED_CMD);   // [3:0]分频, [7:4]震荡频率
    OLED_WR_Byte(0xA8, OLED_CMD); // 设置驱动路数
    OLED_WR_Byte(0x3F, OLED_CMD); // 1/64 duty
    OLED_WR_Byte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0x00, OLED_CMD); // 偏移为 0

    OLED_WR_Byte(0x40, OLED_CMD); // 设置显示开始行 [5:0]

    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵设置
    OLED_WR_Byte(0x14, OLED_CMD); // bit2 开启
    OLED_WR_Byte(0x20, OLED_CMD); // 设置内存地址模式
    OLED_WR_Byte(0x02, OLED_CMD); // 页地址模式
    OLED_WR_Byte(0xA1, OLED_CMD); // 段重定义
    OLED_WR_Byte(0xC0, OLED_CMD); // COM 扫描方向
    OLED_WR_Byte(0xDA, OLED_CMD); // COM 硬件引脚配置
    OLED_WR_Byte(0x12, OLED_CMD); // [5:4]配置

    OLED_WR_Byte(0x81, OLED_CMD); // 对比度
    OLED_WR_Byte(0xEF, OLED_CMD); // 亮度 (越大越亮)
    OLED_WR_Byte(0xD9, OLED_CMD); // 预充电周期
    OLED_WR_Byte(0xf1, OLED_CMD); // PHASE 1/2
    OLED_WR_Byte(0xDB, OLED_CMD); // VCOMH 电压倍率
    OLED_WR_Byte(0x30, OLED_CMD); // 0.83*Vcc

    OLED_WR_Byte(0xA4, OLED_CMD); // 全局显示
    OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
    OLED_WR_Byte(0xAF, OLED_CMD); // 开启显示

    OLED_Clear();
}

// ==================== 显示 GBK 汉字 ====================
void OLED_ShowGBK(uint8_t x, uint8_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * size;

    for (t = 0; t < csize; t++) {
        if (size == 12)      temp = gbk_1212[num][t];
        else if (size == 16) temp = gbk_1616[num][t];
        else return;

        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80) OLED_DrawPoint(x, y, mode);
            else             OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == size) { y = y0; x++; break; }
        }
    }
}

// ==================== 显示 GBK 字符串 ====================
void OLED_ShowGBK_String(uint8_t x, uint8_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t i;
    for (i = 0; i < num; i++) {
        OLED_ShowGBK(x, y, i, size, mode);
        if (size == 12)      x += 12;
        else if (size == 16) x += 16;
    }
}

// ==================== 显示图片 ====================
void OLED_ShowPicture(uint8_t x, uint8_t y, uint8_t index, uint8_t clom, uint8_t row, uint8_t mode)
{
    uint16_t temp, t, t1, num;
    uint8_t y0 = y;
    num = row * clom;
    if (num % 8 == 0) num = num / 8;
    else              num = num / 8 + 1;

    for (t = 0; t < num; t++) {
        temp = gImage[index][t];
        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80) OLED_DrawPoint(x, y, mode);
            else             OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == row) { y = y0; x++; break; }
        }
    }
}

// ==================== GIF 逐帧显示 ====================
void OLED_ShowGIF(uint8_t x, uint8_t y, uint8_t index, uint8_t clom, uint8_t row, uint8_t mode)
{
    uint8_t i;
    for (i = 0; i < index; i++) {
        OLED_ShowPicture(x, y, i, clom, row, mode);
        OLED_Refresh_Gram();
    }
}

// ==================== 显示浮点数 ====================
void OLED_ShowFloat(uint8_t x, uint8_t y, float num, uint8_t len, uint8_t dec_places, uint8_t size)
{
    uint8_t i, sign_flag = 0;
    uint32_t int_part, frac_part;
    float frac;
    uint8_t char_width = size / 2;

    // 处理负数
    if (num < 0) { sign_flag = 1; num = -num; }

    // 分离整数和小数
    int_part = (uint32_t)num;
    frac = num - int_part;
    for (i = 0; i < dec_places; i++) frac *= 10;
    frac_part = (uint32_t)(frac + 0.5f);

    // 处理四舍五入进位
    if (frac_part >= mypow(10, dec_places)) { int_part++; frac_part = 0; }

    // 计算整数位数
    uint8_t int_len;
    if (int_part == 0) int_len = 1;
    else { int_len = 0; uint32_t tmp = int_part; while (tmp) { int_len++; tmp /= 10; } }

    // 总长度不足 len 时左侧补空格
    uint8_t total_len = sign_flag + int_len + 1 + dec_places;
    while (total_len < len) { OLED_ShowChar(x, y, ' ', size, 1); x += char_width; len--; }

    // 负号
    if (sign_flag) { OLED_ShowChar(x, y, '-', size, 1); x += char_width; }

    // 整数部分
    if (int_part == 0) { OLED_ShowChar(x, y, '0', size, 1); x += char_width; }
    else {
        uint8_t buf[10], idx = 0;
        uint32_t tmp = int_part;
        while (tmp) { buf[idx++] = tmp % 10; tmp /= 10; }
        while (idx > 0) { OLED_ShowChar(x, y, buf[--idx] + '0', size, 1); x += char_width; }
    }

    // 小数点
    OLED_ShowChar(x, y, '.', size, 1); x += char_width;

    // 小数部分
    for (i = 0; i < dec_places; i++) {
        uint8_t digit = (frac_part / mypow(10, dec_places - i - 1)) % 10;
        OLED_ShowChar(x, y, digit + '0', size, 1);
        x += char_width;
    }
}
