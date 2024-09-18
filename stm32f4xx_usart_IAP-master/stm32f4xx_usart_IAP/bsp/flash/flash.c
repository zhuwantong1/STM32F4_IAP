#include "flash.h"
#include "delay.h"
/******************************************************************************
* File       : flash.h
* Function   : FLASH C/C++ File
* Description: 1.FLASH ReadWord 2.FLASH WriteWord 3.FLASH Erase        
* Version    : V1.00
* Author     : LC
* Date       : 
* History    :  
* No.					|Date						|Author					|Description
   1					|2020-10-27					|LC					    |V1.0
******************************************************************************/
u32 FLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr;
}

uint16_t GET_FLASH_SECTOR(u32 addr)
{
	if(addr > STM32_FLASH_BASE && addr < ADDR_FLASH_SECTOR1) return FLASH_Sector_0;
	else if(addr < ADDR_FLASH_SECTOR2) return FLASH_Sector_1;
	else if(addr < ADDR_FLASH_SECTOR3) return FLASH_Sector_2;
	else if(addr < ADDR_FLASH_SECTOR4) return FLASH_Sector_3;
	else if(addr < ADDR_FLASH_SECTOR5) return FLASH_Sector_4;
	else if(addr < ADDR_FLASH_SECTOR6) return FLASH_Sector_5;
	else if(addr < ADDR_FLASH_SECTOR7) return FLASH_Sector_6;
	else if(addr < ADDR_FLASH_SECTOR8) return FLASH_Sector_7;
	else if(addr < ADDR_FLASH_SECTOR9) return FLASH_Sector_8;
	else if(addr < ADDR_FLASH_SECTOR10) return FLASH_Sector_9;
	else if(addr < ADDR_FLASH_SECTOR11) return FLASH_Sector_10;
	else if(addr < (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)) return FLASH_Sector_11;
	else return ERROR;
}

u8 FLASH_WriteWord(u32 addr,u32 *pbuffer,u32 DataNum)
{
	FLASH_Status status = FLASH_COMPLETE;
	u32 SECTOR_Addr;
	u32 ADDR_END;
	
	SECTOR_Addr = addr;
	ADDR_END = addr + DataNum * 4;
	if(addr < STM32_FLASH_BASE || addr%4) return ERROR;
	
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	
	if(SECTOR_Addr < 0X1FFF0000)
	{
		while(SECTOR_Addr < ADDR_END)
		{
			if(FLASH_ReadWord(SECTOR_Addr) != 0xFFFFFFFF)
			{
				status = FLASH_EraseSector(GET_FLASH_SECTOR(SECTOR_Addr),VoltageRange_3);
				break;
			}
			else
			{
				SECTOR_Addr += 4;
			}
		}
	}
	
	if(status == FLASH_COMPLETE)
	{
		while(addr < ADDR_END)
		{
			if(FLASH_ProgramWord(addr,*pbuffer) != FLASH_COMPLETE)
			{
				return ERROR;
			}
			addr += 4;
			pbuffer ++;
		}
	}
	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
	return COMPLETE;
}

u8 EraseParaInFlash(u32 addr)
{
	FLASH_Status status = FLASH_COMPLETE;
	uint16_t SECTOR_NUM;
	u32 SECTOR_END_Addr;
	u32 SECTOR_SIZE;
	u32 SECTOR_Addr;
	
	if(addr < STM32_FLASH_BASE || addr%4) return ERROR;
	SECTOR_NUM = GET_FLASH_SECTOR(addr);
	switch(SECTOR_NUM)
	{
		case FLASH_Sector_0:
			SECTOR_SIZE = 1024 * 16;
			SECTOR_Addr = ADDR_FLASH_SECTOR0;
			break;
		case FLASH_Sector_1:
			SECTOR_SIZE = 1024 * 16;
			SECTOR_Addr = ADDR_FLASH_SECTOR1;
			break;
		case FLASH_Sector_2:
			SECTOR_SIZE = 1024 * 16;
			SECTOR_Addr = ADDR_FLASH_SECTOR2;
			break;
		case FLASH_Sector_3:
			SECTOR_SIZE = 1024 * 16;
			SECTOR_Addr = ADDR_FLASH_SECTOR3;
			break;
		case FLASH_Sector_4:
			SECTOR_SIZE = 1024 * 64;
			SECTOR_Addr = ADDR_FLASH_SECTOR4;
			break;
		case FLASH_Sector_5:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR5;
			break;
		case FLASH_Sector_6:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR6;
			break;
		case FLASH_Sector_7:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR7;
			break;
		case FLASH_Sector_8:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR8;
			break;
		case FLASH_Sector_9:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR9;
			break;
		case FLASH_Sector_10:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR10;
			break;
		case FLASH_Sector_11:
			SECTOR_SIZE = 1024 * 128;
			SECTOR_Addr = ADDR_FLASH_SECTOR11;
			break;
		default:
			break;
	}
	SECTOR_END_Addr = SECTOR_Addr + SECTOR_SIZE;
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	
	while(SECTOR_Addr < SECTOR_END_Addr)
	{
		if(FLASH_ReadWord(SECTOR_Addr) != 0xFFFFFFFF)
		{
			status = FLASH_EraseSector(SECTOR_NUM,VoltageRange_3);
			break;
		}
		else
		{
			SECTOR_Addr += 4;
		}
	}

	FLASH_DataCacheCmd(ENABLE);
	FLASH_Lock();
	return status;
}

//擦除代码存放扇区，APP代码我这里存放的是SECTOR1-SECTOR4
u8 STMFLASH_Erase()
{
	FLASH_Status status = FLASH_COMPLETE;
	FLASH_Unlock();
	FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
	status = FLASH_EraseSector(ADDR_FLASH_SECTOR1,VoltageRange_3);
	if(status != FLASH_COMPLETE) return 0;
	status = FLASH_EraseSector(ADDR_FLASH_SECTOR2,VoltageRange_3);
	if(status != FLASH_COMPLETE) return 0;
	status = FLASH_EraseSector(ADDR_FLASH_SECTOR3,VoltageRange_3);
	if(status != FLASH_COMPLETE) return 0;
	status = FLASH_EraseSector(ADDR_FLASH_SECTOR4,VoltageRange_3);
	if(status != FLASH_COMPLETE) return 0;
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
	return 1;
}
