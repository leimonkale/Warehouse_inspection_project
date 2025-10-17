#include "stm32f10x.h"
#include "uart1.h"
#include "ALL_DEFINE.h"
#include <string.h>

// ======== 串口1 引脚定义 ========
// PA9  -> USART1_TX  (复用推挽输出)
// PA10 -> USART1_RX  (浮空输入)

// ======== 全局变量 ========
#define UART1_BUF_SIZE 64
volatile char recv_buf[UART1_BUF_SIZE]; // 接收缓存
volatile uint8_t recv_index = 0;        // 当前接收位置
volatile uint8_t recv_flag = 0;         // 接收完成标志
volatile char last_cmd = 0;              // 最近接收到的命令字符（'0'~'6'）
char wires = 0x00;              // 控制标志位

// ======== 函数定义 ========
void uart_init(int BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // TX(PA9) 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // RX(PA10) 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 串口参数配置
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 开启中断
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

// ================== 串口发送 ==================
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
// ================== 中断接收 ==================
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        char ch = USART_ReceiveData(USART1);

        // 保存最后一个命令字符（0~6）
        if (ch >= '0' && ch <= '6')
            last_cmd = ch;

        // 记录缓存（简单字符串模式，可扩展）
        if (ch != '\n' && recv_index < UART1_BUF_SIZE - 1)
        {
            recv_buf[recv_index++] = ch;
        }
        else
        {
            recv_buf[recv_index] = '\0';
            recv_index = 0;
            recv_flag = 1;  // 一帧接收完成
        }

        // 解析控制命令（例如方向）
        wires &= 0x80;
        switch (ch)
        {
        case '0': wires |= STRAIGHT_LINE;  break;
        case '1': wires |= SLIGHTLY_RIGHT; break;
        case '2': wires |= SLIGHTLY_LEFT;  break;
        case '3': wires |= TURN_RIGHT;     break;
        case '4': wires |= TURN_LEFT;      break;
        case '6': /* 停止或其他操作 */     break;
        default: break;
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// ================== 读取命令接口 ==================

// 获取最近一条命令（非阻塞）
char uart1_get_cmd(void)
{
    char cmd = last_cmd;
    last_cmd = 0; // 清空命令（防止重复）
    return cmd;
}

// 获取完整字符串命令（非阻塞）
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
