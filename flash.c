#include "flash.h"


void memory_read()
{
  period_main = (*((uint8_t*)(ADDRESS_MAIN_PERIOD)) << 8) | (*((uint8_t*)(ADDRESS_MAIN_PERIOD + 1))) & 0xFFFF;
  
  return;
}

void memory_write()
{  
  if(FLASH_IAPSR_bit.DUL == 0)
  {
    FLASH_DUKR = FIRST_DATA_AREA_KEY;
    FLASH_DUKR = SECOND_DATA_AREA_KEY;
  }
  
  FLASH_CR2_bit.WPRG = 0;
  FLASH_NCR2_bit.NWPRG = 1;
  
  adress = (char*)ADDRESS_MAIN_PERIOD;  
  *adress++ = (char)(period_main >> 8); 
  *adress++ = (char)(period_main & 0xFF);  
  
  return;
}
