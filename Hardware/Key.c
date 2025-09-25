#include "stm32f10x.h"   // Device header
#include "Delay.h"

void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;   // 上拉輸入

    // PA1 = 選擇鍵
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PC15 = 確認鍵
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  掃描按鍵
  * @retval 0 = 無按鍵
  *         1 = 選擇鍵 (PA1)
  *         2 = 確認鍵 (PC15)
  */
uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;

    // PA1 → 選擇
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0); // 等待釋放
            KeyNum = 1;
        }
    }

    // PC15 → 確認
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0)
    {
        Delay_ms(20);
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0)
        {
            while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0); // 等待釋放
            KeyNum = 2;
        }
    }

    return KeyNum;
}
