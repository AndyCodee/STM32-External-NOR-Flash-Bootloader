#include "stm32f10x.h"                  // Device header
#include "MySPI.h"
#include "W25Q64_Ins.h"

void W25Q64_Init(void)
{
	MySPI_Init();
	
}

void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_JEDEC_ID);	// 這邊傳我要交換的register address
	// 設定0x9F後，slave會開始轉換3個byte資料。可以查datasheet
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);	// slave收到後就知道之後要交給slave自己交換內容。因此這邊master的0xFF只是隨便給一個值進行交換，實際上無意義。給0x00也可以。反正重點是交換回來的內容是我要的。
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);	// device ID 高八位
	*DID <<= 8;
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);	// device ID 低八位
	MySPI_Stop();
}

void W25Q64_WriteEnable(void)
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_Stop();
}

void W25Q64_WaitBusy(void)	// 寫操作後，要等待內部buzy結束
{
	uint32_t Timeout;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	Timeout = 100000;
	while ((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01)
	{
		Timeout--;
		if (Timeout == 0)
		{
			break;
		}
	}
	MySPI_Stop();
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count) 
{
	uint16_t i;
	
	W25Q64_WriteEnable();
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	
	// address 是24bit。並且由高位先送。因此先右移16位，讓高八位貼後。只傳貼後的8bit資料
	MySPI_SwapByte(Address >> 16);	//	0x123456 >> 16 = 0x000012
	MySPI_SwapByte(Address >> 8);	//	0x123456 >> 8 = 0x001234		
	MySPI_SwapByte(Address);		//	0x123456 >> 0 = 0x123456
	
	// 地址發完，開始發送要寫入的data
	for (i = 0; i < Count; i++)
	{
		MySPI_SwapByte(DataArray[i]);
	}
	
	MySPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WriteEnable();
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	
	MySPI_SwapByte(Address >> 16);	
	MySPI_SwapByte(Address >> 8);	
	MySPI_SwapByte(Address);		
	
	MySPI_Stop();
	
	W25Q64_WaitBusy();
}

void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	
	MySPI_SwapByte(Address >> 16);	
	MySPI_SwapByte(Address >> 8);		
	MySPI_SwapByte(Address);		
	
	for (i = 0; i < Count; i++)
	{
		DataArray[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	}
	
	MySPI_Stop();
}

