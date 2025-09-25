#ifndef __LCD9648_H__
#define __LCD9648_H__

#include "stm32f10x.h"
#include <stdint.h>
#include "LCD9648_Font.h"

/* --- LCD 對應腳位 --- */
#define LCD_GPIO_PERIPH      RCC_APB2Periph_GPIOB
#define LCD_GPIO_PORT        GPIOB
#define LCD_PIN_SCL          GPIO_Pin_10
#define LCD_PIN_SDA          GPIO_Pin_11
#define LCD_PIN_CS           GPIO_Pin_13
#define LCD_PIN_RS           GPIO_Pin_14
#define LCD_PIN_RST          GPIO_Pin_12

/* --- LCD 解析度 --- */
#define LCD_W 96
#define LCD_H 48
#define LCD_PAGES (LCD_H/8)

/* --- 公開函式 --- */
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetContrast(uint8_t val);
void LCD_Refresh(void);

void LCD_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void LCD_ShowChar(uint8_t col, uint8_t row, char c);
void LCD_ShowString(uint8_t col, uint8_t row, const char* s);

#endif
