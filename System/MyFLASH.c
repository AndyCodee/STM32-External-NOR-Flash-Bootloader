#include "stm32f10x.h"                  // Device header
#include <string.h>

uint32_t MyFLASH_ReadWord(uint32_t Address)
{
	return *((volatile uint32_t *)(Address));
}

uint16_t MyFLASH_ReadHalfWord(uint32_t Address)
{
	return *((volatile uint16_t *)(Address));
}

uint8_t MyFLASH_ReadByte(uint32_t Address)
{
	return *((volatile uint8_t *)(Address));
}

// 全擦除
void MyFLASH_EraseAllPages(void)
{
	FLASH_Unlock();
	FLASH_EraseAllPages();
	FLASH_Lock();
}

// 指定page擦除
void MyFLASH_ErasePage(uint32_t PageAddress)
{
	FLASH_Unlock();
	FLASH_ErasePage(PageAddress);
	FLASH_Lock();
}

// 指定地址寫入 1 word (4bytes, 32bits)
void MyFLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramWord(Address, Data);
	FLASH_Lock();
}

// 指定地址寫入 Half word (2bytes, 16bits)
void MyFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramHalfWord(Address, Data);
	FLASH_Lock();
}

// 擦除APP區
void MyFLASH_EraseApp(void)
{
    for(uint32_t addr = 0x08004000; addr < 0x08008000; addr += 0x400) {
        MyFLASH_ErasePage(addr);   // 1KB per page (F103 系列)
    }
}

// 從 buffer 寫入 APP 區
void MyFLASH_WriteApp(uint32_t dest_addr, uint8_t *data, uint32_t size)
{
    for(uint32_t i = 0; i < size; i += 4) {
        uint32_t word = 0xFFFFFFFF;
        memcpy(&word, &data[i], (size - i >= 4) ? 4 : (size - i));
        MyFLASH_ProgramWord(dest_addr + i, word);
    }
}
