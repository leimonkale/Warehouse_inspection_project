#ifndef __DHT_H__
#define __DHT_H__

#include <STM32F10x.h>
#include "delay.h"

extern int humi,temp;
int DHT_Start(void);
int DHT_ReadValue(void);

#endif
