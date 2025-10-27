#include "printf.h"
#include "ALL_DEFINE.h"

unsigned char buf[128] = {0};
int flag = 0;

// ��дfputc��������printf
int fputc(int ch, FILE *file)
{
    USART_SendData(USART3, ch);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    
    return ch;
}

void gpio_uart3_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // ����PB10ΪUSART3_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ����PB11ΪUSART3_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void printf_init(int baud)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    gpio_uart3_init();
    
    // ʹ��USART3ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);
    
    // ����USART3�ж�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(USART3, ENABLE);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void send_data(char *str)
{
    while(*str != '\0')
    {
        USART_SendData(USART3, *str);
        str++;
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

void USART3_IRQHandler(void)
{
    static int i = 0;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        buf[i++] = USART_ReceiveData(USART3);
        if(i >= 128 || buf[i - 1] == '\n')
        {
            buf[i-1] = '\0';
            i = 0;
            flag = 1;
        }
        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
