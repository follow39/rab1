#include <iostm8s003f3.h>
#include <stdint.h>


#define FIRST_DATA_AREA_KEY 0xAE
#define SECOND_DATA_AREA_KEY 0x56
#define ADDRESS_OPT2 0x4803
#define ADDRESS_NOPT2 0x4804
#define ADDRESS_MAIN_PERIOD 0x4000
#define ADDRESS_INTERRUPT_PERIOD 0x4002
#define MIN_MAIN_PERIOD 260
#define MAX_MAIN_PERIOD 800
#define MIN_INTERRUPT_PERIOD 350
#define MAX_INTERRUPT_PERIOD 800000
#define ADC_BIT_CAPACITY 8
#define MODE_BORDER_FIRST 1024*0.1
#define MODE_BORDER_SECOND 1024*0.45
#define MODE_BORDER_THIRD 1024*0.8
#define MODE_BORDER_SAVE 1024*0.95
#define AVERAGE_DIVIDER 10



void option_bytes_init(void);
void set_interrupt_priority(void);
void hsi_init(void);
void gpio_init(void);
void adc_init(void);
void timer_init(void);
void memory_read(void);
void memory_write(void);
void calculating(void);
void buffer_push(uint16_t);
void calc_average(void);
__interrupt void TIM2_OVR_UIF_handler(void);
__interrupt void TIM4_OVR_UIF_handler(void);
__interrupt void ADC1_EOC_handler(void);


uint32_t period_main = MIN_MAIN_PERIOD;
uint16_t period_main_half = 0;
uint16_t period_main_start = 0;
uint32_t period_interrupt = MIN_INTERRUPT_PERIOD;
uint16_t adc_main = 0;
uint16_t adc_interrupt = 0;
uint16_t adc_border = 0;
uint16_t adc_threshold = 0;
uint16_t buf = 0;
uint16_t buf1 = 0;
uint32_t average_value = 0;
int adc_current_channel = 5;
int adc_counter = 0;
int mode = 0;
int mode_change = 0;
char* adress = 0;






