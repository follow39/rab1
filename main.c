#include "main.h"


//PB5   - overload detector
//PC3   - output 1
//PC6   - output 2
//PD3   - microphone
//PD5   - main frequency potentiometer
//PD6   - interrupt frequency potentiometer



int main( void )
{
  hsi_init();
  option_bytes_init();
  set_interrupt_priority();
  
  memory_read();
  calculating();
  memory_write();
  
  
  asm("sim");
  gpio_init();
  timer_init();
  adc_init();
  asm("rim");
  
  
  
  while(1)
  {
    asm("nop");
  }
}

void calculating()
{
//  period_main = MIN_MAIN_PERIOD + ((uint32_t)(MAX_MAIN_PERIOD - MIN_MAIN_PERIOD)*adc_main)/ADC_BIT_CAPACITY;
  if(mode == 4)
  {
    period_main = MAX_MAIN_PERIOD - MIN_MAIN_PERIOD;
    period_main = period_main*adc_main;
    period_main = period_main >> 10;
    period_main = period_main + MIN_MAIN_PERIOD;
  }
  
//  period_interrupt = MIN_INTERRUPT_PERIOD + ((MAX_INTERRUPT_PERIOD - MIN_INTERRUPT_PERIOD)*adc_interrupt)/ADC_BIT_CAPACITY;
  if(mode == 1)
  {
    period_interrupt = MAX_INTERRUPT_PERIOD - MIN_INTERRUPT_PERIOD;
    period_interrupt = period_interrupt*adc_interrupt;
    period_interrupt = period_interrupt >> 10;
    period_interrupt = period_interrupt + MIN_INTERRUPT_PERIOD;
  }
  
  period_main_half = period_main/2;
  period_main_start = period_main_half/2;
  period_main_half = period_main_start*2;
  period_main = period_main_half*2;
  
  return;
}

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

void buffer_push(uint16_t number)
{
  for(int i = 0; i < (BUFFER_SIZE - 1); i++)
  {
    buffer[i] = buffer[i+1];
  }
  buffer[BUFFER_SIZE - 1] = number;
  
  return;
}

void calc_average(void)
{
  average_value = 0;
  for(int i = 0; i < BUFFER_SIZE; i++)
  {
    average_value = average_value + buffer[i];
  }
  average_value = average_value/BUFFER_SIZE;
  
  return;
}

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void TIM2_OVR_UIF_handler(void)
{  
  buf = period_main - period_main_start;
  buf1 = TIM1_CNTRH;
  buf1 = buf1 << 8;
  buf1 = buf1 + TIM1_CNTRL;
  while((buf1 < (buf - 0x0F)) || (buf1 > (buf - 0x05)))
  {
    buf1 = TIM1_CNTRH;
    buf1 = buf1 << 8;
    buf1 = buf1 + TIM1_CNTRL;
  }
  
  
  TIM1_CR1_bit.CEN = 0;
  TIM1_BKR_bit.MOE = 0;
  TIM2_CR1_bit.CEN = 0;
  TIM4_CR1_bit.CEN = 1;
  
    
  calculating();
  
  
//  TIM1_CNTRH = 0;
//  TIM1_CNTRL = 0;
  TIM1_CNTRH = period_main_start >> 8;
  TIM1_CNTRL = period_main_start & 0xFF;
  
  TIM1_CCR1H = period_main_half >> 8;
  TIM1_CCR1L = period_main_half & 0xFF;
  
  TIM1_ARRH = period_main >> 8;
  TIM1_ARRL = period_main & 0xFF;
  
  TIM2_ARRH = period_interrupt >> 8;
  TIM2_ARRL = period_interrupt & 0xFF;
  
    
  TIM2_SR1_bit.UIF = 0;
}

#pragma vector = TIM4_OVR_UIF_vector
__interrupt void TIM4_OVR_UIF_handler(void)
{
  TIM1_CR1_bit.CEN = 1;
  TIM1_BKR_bit.MOE = 1;
  TIM2_CR1_bit.CEN = 1;
  TIM4_CR1_bit.CEN = 0;
  
    if(mode != 4)
    {
      if(mode_change == 1)
      {
        memory_write();
        mode_change = 0;
      }
    }
  
  TIM4_SR_bit.UIF = 0;
}

#pragma vector = ADC1_EOC_vector
__interrupt void ADC1_EOC_handler(void)
{
  if(adc_current_channel == 5)
  {
    adc_border = 0;
    adc_border = ADC_DRL;
    adc_border = adc_border + (ADC_DRH << 8);
    
    if(adc_border < MODE_BORDER_FIRST)
    {
      mode = 1;
    }
    if(adc_border > MODE_BORDER_FIRST && adc_border < MODE_BORDER_SECOND)
    {
      mode = 2;
    }
    if(adc_border > MODE_BORDER_SECOND && adc_border < MODE_BORDER_THIRD)
    {
      mode = 3;
      adc_threshold = adc_border - MODE_BORDER_SECOND;
      adc_threshold = adc_threshold*1024;
      adc_threshold = adc_threshold/(MODE_BORDER_THIRD - MODE_BORDER_SECOND);
    }
    if(adc_border > MODE_BORDER_THIRD)
    {
      mode = 4;
      if(adc_border > MODE_BORDER_SAVE)
      {
        mode_change = 1;
      }
    }
  }
  if(adc_current_channel == 5)  
  {
    switch(mode)
    {
    case 1:
      adc_interrupt = 0;
      adc_interrupt = ADC_DRL;
      adc_interrupt = adc_interrupt + (ADC_DRH << 8);
      break;
    case 2:
      
      break;
    case 3:      
      buf = 0;
      buf = ADC_DRL;
      buf = buf + (ADC_DRH << 8);
      if(buf > adc_threshold)
      {
        buffer_push(buf);
      }
      break;
    case 4:
      adc_main = 0;
      adc_main = ADC_DRL;
      adc_main = adc_main + (ADC_DRH << 8);
      break;
    }
  }
  
  ADC_CSR_bit.CH = adc_current_channel;
  ADC_CR1_bit.ADON = 1;
  
  ADC_CSR_bit.EOC = 0;
  return;
}










