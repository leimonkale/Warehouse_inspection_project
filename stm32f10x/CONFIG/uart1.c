#include "stm32f10x.h"
#include "uart1.h"
#include "stdlib.h"
#include "ALL_DEFINE.h"
#include <string.h>

char cmd;
int distance;

// ======== 串口1 引脚定义 ========
// PA9  -> USART1_TX  (复用推挽输出)
// PA10 -> USART1_RX  (浮空输入)

// ======== 全局变量 ========
#define UART1_BUF_SIZE 5
volatile char recv_buf[UART1_BUF_SIZE]; // 接收缓存
volatile uint8_t recv_index = 0;        // 当前接收位置
volatile uint8_t recv_flag = 0;         // 接收完成标志
volatile char last_cmd[UART1_BUF_SIZE]; // 最近接收到的完整命令字符串
//volatile char last_cmd = 0;              // 最近接收到的命令字符（'0'~'6'）
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

        // 记录缓存（以换行符作为结束标志）
        if (ch != '\n' && recv_index < UART1_BUF_SIZE - 1)
        {
            recv_buf[recv_index++] = ch;
        }
        else
        {
            recv_buf[recv_index] = '\0';
            recv_index = 0;
            recv_flag = 1;  // 一帧接收完成
            
            // 保存完整命令字符串
            strcpy((char *)last_cmd, (char *)recv_buf);
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// ================== 读取命令接口 ==================

// ================== 解析字符串命令 ==================
// 返回值: 0-解析失败, 1-解析成功
// cmd_char: 存储解析出的命令字符
// distance: 存储解析出的距离值
uint8_t uart1_parse_command(char *cmd_char, int *distance)
{
    if (!recv_flag) return 0;
    
    // 确保字符串至少有一个字符
    if (strlen((char *)last_cmd) < 1)
    {
        recv_flag = 0;
        return 0;
    }
    
    // 解析命令字符
    *cmd_char = last_cmd[0];
    //printf("last_cmd:%s\r\n",last_cmd);
    // 解析距离值（如果有）
    *distance = 0;
    if (strlen((char *)last_cmd) > 1)
    {
        *distance = atoi((char *)last_cmd + 1);
    }
    
    recv_flag = 0;
    return 1;
}

// ================== 获取并处理完整命令 ==================
void process_uart_command(void)
{
    if (uart1_parse_command(&cmd, &distance))
    {
        // 解析控制命令（例如方向）
        wires &= 0x80;
        switch (cmd)
        {
        case '0': wires |= STRAIGHT_LINE;  break;
        case '1': wires |= SLIGHTLY_RIGHT; break;
        case '2': wires |= SLIGHTLY_LEFT;  break;
        case '3': wires |= TURN_RIGHT;     break;
        case '4': wires |= TURN_LEFT;      break;
		case '5': wires |= SLIGHTLY_TT;    break;
        case '6': break;
        default: break;
        }
        
        // 这里可以根据distance值做进一步处理
        // 例如：根据距离调整电机速度等
    }
}
