#include "stm32f10x.h"
#include "uart1.h"
#include "ALL_DEFINE.h"
#include <string.h>

// ======== ����1 ���Ŷ��� ========
// PA9  -> USART1_TX  (�����������)
// PA10 -> USART1_RX  (��������)

// ======== ȫ�ֱ��� ========
#define UART1_BUF_SIZE 64
volatile char recv_buf[UART1_BUF_SIZE]; // ���ջ���
volatile uint8_t recv_index = 0;        // ��ǰ����λ��
volatile uint8_t recv_flag = 0;         // ������ɱ�־
volatile char last_cmd = 0;              // ������յ��������ַ���'0'~'6'��
char wires = 0x00;              // ���Ʊ�־λ

// ======== �������� ========
void uart_init(int BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // ����ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // TX(PA9) �����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RX(PA10) ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ���ڲ�������
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // �����ж�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

// ================== ���ڷ��� ==================
void uart_send_byte(char data)
{
    USART_SendData(USART1, data);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
/*
void uart_send_str(const char *str)
{
    while (*str)
    {
        uart_send_byte(*str++);
    }
}
*/
// ================== �жϽ��� ==================
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        char ch = USART_ReceiveData(USART1);

        // �������һ�������ַ���0~6��
        if (ch >= '0' && ch <= '6')
            last_cmd = ch;

        // ��¼���棨���ַ���ģʽ������չ��
        if (ch != '\n' && recv_index < UART1_BUF_SIZE - 1)
        {
            recv_buf[recv_index++] = ch;
        }
        else
        {
            recv_buf[recv_index] = '\0';
            recv_index = 0;
            recv_flag = 1;  // һ֡�������
        }

        // ��������������緽��
        wires &= 0x80;
        switch (ch)
        {
        case '0': wires |= STRAIGHT_LINE;  break;
        case '1': wires |= SLIGHTLY_RIGHT; break;
        case '2': wires |= SLIGHTLY_LEFT;  break;
        case '3': wires |= TURN_RIGHT;     break;
        case '4': wires |= TURN_LEFT;      break;
        case '6': /* ֹͣ���������� */     break;
        default: break;
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// ================== ��ȡ����ӿ� ==================

// ��ȡ���һ�������������
char uart1_get_cmd(void)
{
    char cmd = last_cmd;
    last_cmd = 0; // ��������ֹ�ظ���
    return cmd;
}

// ��ȡ�����ַ��������������
uint8_t uart1_get_string(char *buf)
{
    if (recv_flag)
    {
        strcpy(buf, (char *)recv_buf);
        recv_flag = 0;
        return 1;
    }
    return 0;
}
