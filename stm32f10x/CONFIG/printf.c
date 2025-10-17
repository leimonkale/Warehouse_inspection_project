#include "printf.h"
#include "ALL_DEFINE.h"


unsigned char buf[128] = {0};
int flag = 0;

//调用stdio.h中的printf，重写fputc函数
int fputc(int ch, FILE *file)
{
	USART_SendData(USART2, ch);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	
	return ch;
}

void gpio_uart2_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = UART2_Pin_TX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIO_UART2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = UART2_Pin_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIO_UART2, &GPIO_InitStructure);
}

void printf_init(int baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	gpio_uart2_init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);
	
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void send_data(char *str)
{
	while(*str != '\0')
	{
		USART_SendData(USART2, *str);
		str++;
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	}
}

void USART2_IRQHandler(void)
{
	static int i = 0;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		buf[i++] = USART_ReceiveData(USART2);
		if(i >= 128 || buf[i - 1] == '\n')
		{
			buf[i-1] = '\0';
			i = 0;
			//printf("%s\r\n", buf);
			flag = 1;
		}
		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}
