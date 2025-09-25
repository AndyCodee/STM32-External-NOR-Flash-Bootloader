#include "stm32f10x.h"
#include "Key.h"
#include "LCD9648.h"
#include "Delay.h"
#include "W25Q64.h"
#include "MyFlash.h"

// ======== 結構定義 ========
typedef void (*pFunction)(void);

typedef struct {
    uint32_t ValidFlag;   // 0xA5A5A5A5 = 有效
    uint32_t Address;     // 外部 Flash 起始位址
    uint32_t Size;        // 程式大小 (bytes)
    uint32_t CRC32;       // 校驗碼
} ImageInfo_t;

// ======== APP 區固定在 0x08004000 ========
#define APP_ADDR 0x08004000

// ======== 跳轉函式 ========
void JumpToApplication(uint32_t app_addr)
{
    uint32_t appStack        = *(__IO uint32_t*)app_addr;
    uint32_t appResetHandler = *(__IO uint32_t*)(app_addr + 4);
    pFunction JumpToApp      = (pFunction)appResetHandler;

    __disable_irq();

    SCB->VTOR = app_addr;
    __DSB(); __ISB();

    __set_MSP(appStack);

    __enable_irq();

    JumpToApp();
}

// ======== CRC32 計算 (硬體 CRC) ========
uint32_t Calc_CRC32(uint8_t *data, uint32_t length)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    CRC_ResetDR();

    uint32_t i;
    for (i = 0; i < (length / 4); i++) {
        uint32_t word = ((uint32_t)data[4*i]) |
                        ((uint32_t)data[4*i+1] << 8) |
                        ((uint32_t)data[4*i+2] << 16) |
                        ((uint32_t)data[4*i+3] << 24);
        CRC_CalcCRC(word);
    }

    if (length % 4) {
        uint32_t lastWord = 0;
        for (uint32_t j = 0; j < (length % 4); j++) {
            lastWord |= ((uint32_t)data[4*i + j] << (8 * j));
        }
        CRC_CalcCRC(lastWord);
    }

    return CRC_GetCRC();
}

// ======== 驗證內部 APP CRC ========
uint8_t VerifyAppCRC(ImageInfo_t *meta)
{
    uint32_t crc = Calc_CRC32((uint8_t*)APP_ADDR, meta->Size);
    return (crc == meta->CRC32) ? 1 : 0;
}

// ======== 從外部 Flash 載入 App 到內部 ========
void LoadRepoFromW25Q64(ImageInfo_t *meta)
{
    uint8_t buffer[256];
    uint32_t offset = 0;

    MyFLASH_EraseApp();

    while (offset < meta->Size) {
        uint32_t len = (meta->Size - offset > sizeof(buffer)) ? sizeof(buffer) : (meta->Size - offset);
        W25Q64_ReadData(meta->Address + offset, buffer, len);
        MyFLASH_WriteApp(APP_ADDR + offset, buffer, len);
        offset += len;
    }
}

// ======== LCD 選單顯示 ========
static uint8_t current_repo = 0;  // 0=Repo1, 1=Repo2

void LCD_ShowMenu(uint8_t repo)
{
	LCD_Clear();
	
	LCD_ShowString(0, 0, "Repo Menu");
    if (repo == 0) {
        LCD_ShowString(0, 1, "> Repo1");
        LCD_ShowString(0, 2, "  Repo2");
    } else {
        LCD_ShowString(0, 1, "  Repo1");
        LCD_ShowString(0, 2, "> Repo2");
    }
	LCD_ShowString(0, 5, "STM32 Bootloader");
    LCD_Refresh();
}

// ======== 主程式 ========
int main(void)
{
    Key_Init();
    LCD_Init();
    W25Q64_Init();

    ImageInfo_t meta_array[2];
    W25Q64_ReadData(0x000000, (uint8_t*)meta_array, sizeof(meta_array));  // 讀 metadata
	
	LCD_Clear();
	LCD_ShowString(0, 1, "STM32 Bootloader");
	LCD_ShowString(5, 3, "W25Q64");
	LCD_ShowString(3, 4, "RepoLoader");
	LCD_Refresh();
	Delay_ms(2500);


    LCD_ShowMenu(current_repo);

    while (1)
    {
        uint8_t key = Key_GetNum();

        if (key == 1) {  // PA1 切換選單
            current_repo ^= 1;
            LCD_ShowMenu(current_repo);
        }
        else if (key == 2) { // PC15 確認
            LCD_Clear();
            if (current_repo == 0) {
                LCD_ShowString(0, 0, "Load Repo1...");
                LoadRepoFromW25Q64(&meta_array[0]);
                if (VerifyAppCRC(&meta_array[0])) {
                    LCD_ShowString(0, 1, "CRC OK, Jump");
                    LCD_Refresh();
                    JumpToApplication(APP_ADDR);
                } else {
                    LCD_ShowString(0, 1, "CRC FAIL!");
                }
            } else {
                LCD_ShowString(0, 0, "Load Repo2...");
                LoadRepoFromW25Q64(&meta_array[1]);
                if (VerifyAppCRC(&meta_array[1])) {
                    LCD_ShowString(0, 1, "CRC OK, Jump");
                    LCD_Refresh();
                    JumpToApplication(APP_ADDR);
                } else {
                    LCD_ShowString(0, 1, "CRC FAIL!");
                }
            }
            LCD_Refresh();
        }
    }
}
