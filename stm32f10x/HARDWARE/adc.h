#ifndef __ADC_H__
#define __ADC_H__

#include <STM32F10x.h>
#include "printf.h"

extern int adc_num;

void ADC1_Init(void);
void Get_ADCValue(void);
	
#endif
