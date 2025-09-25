#ifndef __MYFLASH_H__
#define __MYFLASH_H__

uint32_t MyFLASH_ReadWord(uint32_t Address);
uint16_t MyFLASH_ReadHalfWord(uint32_t Address);
uint8_t MyFLASH_ReadByte(uint32_t Address);

void MyFLASH_EraseAllPages(void);
void MyFLASH_ErasePage(uint32_t PageAddress);

void MyFLASH_ProgramWord(uint32_t Address, uint32_t Data);
void MyFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);

// 針對APP區 (0x08004000到0x08008000)
void MyFLASH_EraseApp(void);
void MyFLASH_WriteApp(uint32_t dest_addr, uint8_t *data, uint32_t size);

#endif
