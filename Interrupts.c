#include "interrupts.h"



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
  
    if(mode != 2)
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
    adc_current_channel = 6;
    
    if(adc_border < MODE_BORDER_FIRST)
    {
      mode = 0;
    }
    if(adc_border > MODE_BORDER_FIRST && adc_border < MODE_BORDER_SECOND)
    {
      mode = 1;
    }
    if(adc_border > MODE_BORDER_SECOND)
    {
      mode = 2;
      if(adc_border > 1024*0.9)
      {
        mode_change = 1;
      }
    }      
  }
  else
  {
    if(mode == 2)
    {
      adc_main = 0;
      adc_main = ADC_DRL;
      adc_main = adc_main + (ADC_DRH << 8); 
      adc_current_channel = 5;
    }
    else
    {      
      adc_interrupt = 0;
      adc_interrupt = ADC_DRL;
      adc_interrupt = adc_interrupt + (ADC_DRH << 8);
      adc_current_channel = 5;      
    }
  }
  
  ADC_CSR_bit.CH = adc_current_channel;
  ADC_CR1_bit.ADON = 1;
  
  ADC_CSR_bit.EOC = 0;
  return;
}