#include "flash.h"
#include <stm32f0xx.h>


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Private define ------------------------------------------------------------*/
#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */
#define FLASH_USER_START_ADDR   ((uint32_t)0x08006000)   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ((uint32_t)0x08007000)   /* End @ of user Flash area */
#define DATA_32                 ((uint32_t)0x12345678)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t EraseCounter = 0x00, Address = 0x00;
uint32_t Data = 0x3210ABCD;
uint32_t NbrOfPage = 0x00;
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus MemoryProgramStatus = PASSED;


int program_flash(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */ 

  /* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_Unlock();
    
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

  /* Define the number of page to be erased */
  NbrOfPage = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;

  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    if (FLASH_ErasePage(FLASH_USER_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter))!= FLASH_COMPLETE)
    {
     /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
  }
  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  Address = FLASH_USER_START_ADDR;

  while (Address < FLASH_USER_END_ADDR)
  {
    if (FLASH_ProgramWord(Address, DATA_32) == FLASH_COMPLETE)
    {
      Address = Address + 4;
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
         User can add here some code to deal with this error */
      while (1)
      {
      }
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock(); 


  /* Check if the programmed data is OK 
      MemoryProgramStatus = 0: data programmed correctly
      MemoryProgramStatus != 0: number of words not programmed correctly ******/
  Address = FLASH_USER_START_ADDR;
  MemoryProgramStatus = PASSED;
  
  while (Address < FLASH_USER_END_ADDR)
  {
    Data = *(__IO uint32_t *)Address;

    if (Data != DATA_32)
    {
      MemoryProgramStatus = FAILED;  
    }

    Address = Address + 4;
  }
	
	return 0;
}

#define PAGE_SIZE FLASH_PAGE_SIZE


static u32 ApplicationAddress = 0x8000000 + (32*1024);      //APP程序首地址
static u32 ApplicationSize		= 1024*96;          //程序预留空间

static u8 buffer[2];
static u8 buffer_index = 0;
static u32 buffer_addr = 0;

static u32 FLASH_PagesMask(u32 Size)
{
    u32 pagenumber = 0x0;
    u32 size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

#include <stdio.h>

u32 FLASH_WriteBank(u8 *pData, u32 addr, u16 size)
{
    u16 *pDataTemp = (u16 *)pData;
    u32 temp = addr;
//    FLASH_Status FLASHStatus = FLASH_COMPLETE;
//    u32 NbrOfPage = 0;
//
//    NbrOfPage = FLASH_PagesMask(addr + size - ApplicationAddress);
//    for (; (m_EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); m_EraseCounter++)
//    {
//        FLASHStatus = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * m_EraseCounter));
//    }

    for (; temp < (addr + size); pDataTemp++, temp += 2)
    {
				//写数据数据
        FLASH_ProgramHalfWord(temp, *pDataTemp);
        if (*pDataTemp != *(u16 *)temp)
        {
            return 0;
        }
    }

    return 1;
}
//#include <stdio.h>
u32 FLASH_AppendOneByte(u8 Data)
{
	buffer[buffer_index++] = Data;
	
	//printf("FLASH %08X %d \r\n",buffer_addr,sizeof(buffer));
	
	if (buffer_index >= sizeof(buffer))
	{
		FLASH_WriteBank(buffer,buffer_addr,sizeof(buffer));
		buffer_addr += sizeof(buffer);
		buffer_index = 0;
	}else
	{
		//printf("IGNORE\r\n");
	}
	
	return 0;
	//
}

u32 FLASH_AppendBuffer(u8 *Data , u32 size)
{
	u32 i=0;
	for(i=0;i<size;i++)
	{
		FLASH_AppendOneByte(Data[i]);
	}
	
	return 0;
	
	//
}

void FLASH_AppendEnd(void)
{
	if (buffer_index > 0)
	{
		FLASH_WriteBank(buffer,buffer_addr,sizeof(buffer));
		buffer_addr += sizeof(buffer);
	}
	//
}
/*******************************************************************************
* Function Name :void FLASH_ProgramDone(void)
* Description   :烧写启动
* Input         :
* Output        :
* Other         :
* Date          :2013.02.24
*******************************************************************************/
void FLASH_ProgramStart(u32 addr , u32 size)
{
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    u32 NbrOfPage = 0;
    u32 EraseCounter = 0;

    FLASH_Unlock();
//    m_EraseCounter = 0;
	
		ApplicationAddress = addr;
		ApplicationSize = size;
	
		buffer_index = 0;
		buffer_addr = ApplicationAddress;

    NbrOfPage = FLASH_PagesMask(ApplicationSize);
    for (; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(ApplicationAddress + (PAGE_SIZE * EraseCounter));
    }
}


/*******************************************************************************
* Function Name :void FLASH_ProgramDone(void)
* Description   :烧写结束
* Input         :
* Output        :
* Other         :
* Date          :2013.02.24
*******************************************************************************/
void FLASH_ProgramDone(void)
{
    FLASH_Lock();
}

