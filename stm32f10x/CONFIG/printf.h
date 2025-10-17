#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stm32f10x.h>
#include <stdio.h>

void printf_init(int baud);

void send_data(char *str);

#endif
