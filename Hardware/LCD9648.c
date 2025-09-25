#include "LCD9648.h"

/* --- GPIO 控制巨集 --- */
#define SCL_H() GPIO_SetBits(LCD_GPIO_PORT, LCD_PIN_SCL)
#define SCL_L() GPIO_ResetBits(LCD_GPIO_PORT, LCD_PIN_SCL)
#define SDA_H() GPIO_SetBits(LCD_GPIO_PORT, LCD_PIN_SDA)
#define SDA_L() GPIO_ResetBits(LCD_GPIO_PORT, LCD_PIN_SDA)
#define CS_H()  GPIO_SetBits(LCD_GPIO_PORT, LCD_PIN_CS)
#define CS_L()  GPIO_ResetBits(LCD_GPIO_PORT, LCD_PIN_CS)
#define RS_H()  GPIO_SetBits(LCD_GPIO_PORT, LCD_PIN_RS)
#define RS_L()  GPIO_ResetBits(LCD_GPIO_PORT, LCD_PIN_RS)
#define RST_H() GPIO_SetBits(LCD_GPIO_PORT, LCD_PIN_RST)
#define RST_L() GPIO_ResetBits(LCD_GPIO_PORT, LCD_PIN_RST)

/* --- Framebuffer --- */
static uint8_t fb[LCD_PAGES][LCD_W];

/* --- 短延時 --- */
static void delay_short(void){ for(volatile int i=0;i<50;i++); }

/* --- GPIO 初始化 --- */
static void LCD_GPIO_Init(void)
{
    RCC_APB2PeriphClockCmd(LCD_GPIO_PERIPH, ENABLE);
    GPIO_InitTypeDef io;
    io.GPIO_Speed = GPIO_Speed_50MHz;
    io.GPIO_Mode  = GPIO_Mode_Out_PP;
    io.GPIO_Pin   = LCD_PIN_SCL | LCD_PIN_SDA | LCD_PIN_CS | LCD_PIN_RS | LCD_PIN_RST;
    GPIO_Init(LCD_GPIO_PORT, &io);

    CS_H(); SCL_L(); SDA_L(); RS_H(); RST_H();
}

/* --- 傳輸 bit --- */
static void LCD_WriteBit(uint8_t bit)
{
    if(bit) SDA_H(); else SDA_L();
    SCL_H(); delay_short();
    SCL_L();
}

/* --- 傳輸 byte --- */
static void LCD_WriteByte(uint8_t dat)
{
    for(int i=7;i>=0;i--)
        LCD_WriteBit((dat>>i)&0x01);
}

/* --- 寫命令 --- */
static void LCD_WriteCmd(uint8_t cmd)
{
    CS_L();
    RS_L();
    LCD_WriteByte(cmd);
    CS_H();
}

/* --- 寫資料 --- */
static void LCD_WriteData(uint8_t dat)
{
    CS_L();
    RS_H();
    LCD_WriteByte(dat);
    CS_H();
}

/* --- 設定頁與列位置 --- */
static void LCD_SetAddr(uint8_t page, uint8_t col)
{
    LCD_WriteCmd(0xB0 | (page & 0x0F));
    LCD_WriteCmd(0x10 | ((col >> 4) & 0x0F));
    LCD_WriteCmd(0x00 | (col & 0x0F));
}

/* --- 初始化 --- */
void LCD_Init(void)
{
    LCD_GPIO_Init();

    /* 硬體重置 */
    RST_L(); for(volatile int i=0;i<20000;i++);
    RST_H(); for(volatile int i=0;i<20000;i++);

    /* 初始化序列 */
    LCD_WriteCmd(0xE2);
    LCD_WriteCmd(0xAE);
    LCD_WriteCmd(0xA2);
    LCD_WriteCmd(0xA0);
    LCD_WriteCmd(0xC8);
    LCD_WriteCmd(0x2F);
    LCD_WriteCmd(0x25);
    LCD_WriteCmd(0x81);
    LCD_WriteCmd(0x28);
    LCD_WriteCmd(0x40);
    LCD_WriteCmd(0xAF);

    LCD_Clear();
}

/* --- 清屏 --- */
void LCD_Clear(void)
{
    for(uint8_t p=0;p<LCD_PAGES;p++)
        for(uint8_t x=0;x<LCD_W;x++)
            fb[p][x] = 0x00;
    LCD_Refresh();
}

/* --- 對比度 --- */
void LCD_SetContrast(uint8_t val)
{
    LCD_WriteCmd(0x81);
    LCD_WriteCmd(val & 0x3F);
}

/* --- 更新顯示 --- */
void LCD_Refresh(void)
{
    for(uint8_t p=0;p<LCD_PAGES;p++){
        LCD_SetAddr(p,0);
        for(uint8_t x=0;x<LCD_W;x++)
            LCD_WriteData(fb[p][x]);
    }
}

/* --- 畫像素 --- */
void LCD_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{

    x = LCD_W  - 1 - x;  // X 軸鏡像
    y = LCD_H  - 1 - y;  // Y 軸鏡像

    uint8_t page = y >> 3;
    uint8_t bit  = 1 << (y & 7);
    if(color) fb[page][x] |= bit;
    else      fb[page][x] &= ~bit;
}

void LCD_ShowChar(uint8_t col, uint8_t row, char c)
{
    if(c < 32 || c > 126) c = '?';
    uint8_t x = col * 6;
    uint8_t y = row * 8;
    const uint8_t *glyph = LCD9648_F6x8[c - 32];

    for(uint8_t i=0; i<6; i++) {
        uint8_t line = glyph[i];
        for(uint8_t b=0; b<8; b++) {
            LCD_DrawPixel(x+i, y+b, (line >> b) & 0x01);
        }
    }
}

void LCD_ShowString(uint8_t col, uint8_t row, const char* s)
{
    while(*s) {
        if(*s=='\n'){ row++; col=0; s++; continue; }
        LCD_ShowChar(col, row, *s++);
        if(++col >= (LCD_W/6)) { col=0; row++; }
        if(row >= (LCD_H/8)) break;
    }
}

