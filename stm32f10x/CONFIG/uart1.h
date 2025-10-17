#ifndef __UART1_H__
#define __UART1_H__

#include <stdio.h>
#include <string.h>
#include "ALL_DEFINE.h"
#include "ALL_DATA.h"

extern char wires;   //储存巡线结构数据

void uart_init(int BaudRate);
void uart_send_byte(char data);
void uart_send_str(char *str);


unsigned char uart_recv_byte(void);
void uart_get_recvbuf(unsigned char *buf);

#endif
