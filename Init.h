#ifndef IOSTM8S003F3_H
#define IOSTM8S003F3_H
#include <iostm8s003f3.h>
#endif

#ifndef STDINT_H
#define STDINT_H
#include <stdint.h>
#endif

#ifndef MAIN_H
#define MAIN_H
#include "main.h"
#endif


void option_bytes_init(void);
void set_interrupt_priority(void);
void hsi_init(void);
void gpio_init(void);
void adc_init(void);
void timer_init(void);