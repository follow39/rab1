#include "init.h"



void hsi_init(void)
{
  CLK_CKDIVR = 0;
  while(!CLK_ICKR_bit.HSIRDY){}
  
  return;
}

void set_interrupt_priority()
{  
  ITC_SPR4_bit.VECT13SPR = 3;//TIM2 update/overflow
//  ITC_SPR6_bit.VECT22SPR = 1;//ADC1 end of conversion
  ITC_SPR6_bit.VECT23SPR = 3;//TIM4 update/overflow
  
  return;
}

void gpio_init()
{
  PB_DDR_bit.DDR5 = 0;
  
  PC_DDR_bit.DDR3 = 1;
  PC_DDR_bit.DDR6 = 1;
  PC_CR1_bit.C13 = 1;
  PC_CR1_bit.C16 = 1;
  PC_CR2_bit.C23 = 1;
  PC_CR2_bit.C26 = 1;
  
  PD_DDR_bit.DDR5 = 0;
  PD_DDR_bit.DDR6 = 0;
  
  return;
}

void option_bytes_init(void)
{
  if(FLASH_IAPSR_bit.DUL == 0)
  {
    FLASH_DUKR = FIRST_DATA_AREA_KEY;
    FLASH_DUKR = SECOND_DATA_AREA_KEY;
  }
  FLASH_CR2_bit.OPT = 1;
  FLASH_NCR2_bit.NOPT = 0;
  if(*((uint8_t*)(ADDRESS_OPT2)) != 0x91)
    *((uint8_t*)(ADDRESS_OPT2)) = 0x91;
  if(*((uint8_t*)(ADDRESS_NOPT2)) != 0x6E)
    *((uint8_t*)(ADDRESS_NOPT2)) = 0x6E;
  
  return;
}

void adc_init()
{
  ADC_CSR_bit.CH = adc_current_channel;
  ADC_CSR_bit.EOCIE = 1;
  ADC_CR2_bit.ALIGN = 1;
  
  ADC_CR1_bit.ADON = 1;
  ADC_CR1_bit.ADON = 1;
  
  return;
}

void timer_init()
{
  //TIM1  
  //PWM
  TIM1_CCR1H = period_main_half >> 8;
  TIM1_CCR1L = period_main_half & 0xFF;
  TIM1_CCMR1_bit.OC1M = 6;
  TIM1_CCMR1_bit.OC1FE = 1;
  
  //complementary output
  TIM1_CCER1_bit.CC1E = 1;
  TIM1_CCER1_bit.CC1NE = 1;
  TIM1_BKR_bit.BKP = 0;
  TIM1_BKR_bit.BKE = 1;
  TIM1_BKR_bit.OSSR = 1;
  TIM1_BKR_bit.OSSI = 1;
  TIM1_BKR_bit.AOE = 1;
  TIM1_DTR = 0x03;
    
  //initial initialization
  TIM1_CNTRH = period_main_start >> 8;
  TIM1_CNTRL = period_main_start & 0xFF;
  TIM1_ARRH = period_main >> 8;
  TIM1_ARRL = period_main & 0xFF;
  TIM1_PSCRH = 0;
  TIM1_PSCRL = 0;
  TIM1_CR1_bit.ARPE = 1;
  
  
  //tim2
  TIM2_PSCR = 8;
  TIM2_IER_bit.UIE = 1;
  TIM2_ARRH = period_interrupt >> 8;
  TIM2_ARRL = period_interrupt & 0xFF;
  
  
  //tim4
  TIM4_IER_bit.UIE = 1;
  TIM4_PSCR = 6;
  
  
  //start
  TIM1_CR1_bit.CEN = 1;
  TIM1_BKR_bit.MOE = 1;
  TIM2_CR1_bit.CEN = 1;
  
  return;
}