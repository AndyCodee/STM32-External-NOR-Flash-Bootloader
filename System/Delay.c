#include "stm32f10x.h"

/**
  * @brief  微秒級延遲
  * @param  xus 延遲時間 (單位：微秒)，範圍：0 ~ 233015
  * @retval 無
  */
void Delay_us(uint32_t xus)
{
    SysTick->LOAD = 72 * xus;               // 設定定時器重裝值 (72MHz 時鐘 → 1us 需72個時鐘週期)
    SysTick->VAL = 0x00;                    // 清空當前計數值
    SysTick->CTRL = 0x00000005;             // 設定時鐘源為 HCLK，並啟動定時器
    while(!(SysTick->CTRL & 0x00010000));   // 等待計數歸零
    SysTick->CTRL = 0x00000004;             // 關閉定時器
}

/**
  * @brief  毫秒級延遲
  * @param  xms 延遲時間 (單位：毫秒)，範圍：0 ~ 4294967295
  * @retval 無
  */
void Delay_ms(uint32_t xms)
{
    while(xms--)
    {
        Delay_us(1000);    // 1 毫秒 = 1000 微秒
    }
}
 
/**
  * @brief  秒級延遲
  * @param  xs 延遲時間 (單位：秒)，範圍：0 ~ 4294967295
  * @retval 無
  */
void Delay_s(uint32_t xs)
{
    while(xs--)
    {
        Delay_ms(1000);    // 1 秒 = 1000 毫秒
    }
}
