#include "stm32f10x.h"                  // Device header
#include "uart1.h"
#include "ALL_DEFINE.h"

//PA9-USART1_TX 复用推挽输出     PA10-USART1_RX 浮空输入
/**
 * @brief  初始化串口
 * @param  波特率
 * @retval 无
 */
void uart_init(int BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//1.RCC   GPIOA  UART 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	//2.GPIO  PA9复用推挽输出   PA10浮空输入
	GPIO_InitStructure.GPIO_Pin = UART1_Pin_TX; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = UART1_Pin_RX; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//3.uart  波特率 数据位 校验位 停止位 ... 
	USART_InitStructure.USART_BaudRate = BaudRate;  //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据长度 8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位 1位
	USART_InitStructure.USART_Parity = USART_Parity_No;  //奇偶校验位 不用
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控 无
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //模式
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 0/1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //0~7
	NVIC_Init(&NVIC_InitStructure);
	
	
	//4.使能
	USART_Cmd(USART1, ENABLE);
}


/**
 * @brief  串口发送字符
 * @param  单个字符
 * @retval 无
 */
void uart_send_byte(char data)
{
	USART_SendData(USART1, data);
	//等待发送完成
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	
}

/**
 * @brief  串口发送字符串
 * @param  字符串
 * @retval 无
 */
void uart_send_str(char *str)
{
	while(*str != '\0')
	{
		uart_send_byte(*str);
		str++;
	}
}

unsigned char uart_recv_byte(void)
{
	u16 RxData; 
	
	//等待数据来临 阻塞等待
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);	
	
	RxData = USART_ReceiveData(USART1);
	return RxData;
}

unsigned char recv_buf[64] = {0};
//unsigned char uart1_buf[128];
int32_t wires_start = 0;
char wires = 0x00;

void USART1_IRQHandler(void)
{
	u16 RxData; 
	static int i = 0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		RxData = USART_ReceiveData(USART1);
		USART_SendData(USART1, RxData);
		
		 switch(RxData)
        {
            case '0':  
                wires |= STRAIGHT_LINE;
                break;
            case '1':
                wires |= SLIGHTLY_RIGHT;
                break;
            case '2':
				wires |= SLIGHTLY_LEFT;
                break;
            case '3':
                wires |= TURN_RIGHT;
                break;
            case '4':
                wires |= TURN_LEFT;
                break;
            case '6':
                
                break;
		}
		
		if(RxData != '\n')
		{
			recv_buf[i++] = RxData;
		}
		else
		{
			recv_buf[i++] = '\0';
			i = 0;
			wires_start = 1;
		}
		
		/*if(!strcmp((char *)recv_buf,"1")){
			flag2 = 1;
		
		}
		else if(!strcmp((char *)recv_buf,"2")){
			flag3 = 1;
		
		}
		else if(!strcmp((char *)recv_buf,"3")){
			flag4 = 1;
		
		}
		else if(!strcmp((char *)recv_buf,"4")){
			flag5 = 1;
		
		}
		else if(!strcmp((char *)recv_buf,"5")){
			flag6 = 1;
		
		}
		else if(!strcmp((char *)recv_buf,"6")){
			flag7 = 1;
		
		}*/

//		if(RxData == 0x21)//判断包头
//		{
//			flag = 1;
//		}

		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void uart_get_recvbuf(unsigned char *buf)
{
	while(wires_start == 0);
	strcpy((char *)buf, (char *)recv_buf);
	wires_start = 0;	
}



