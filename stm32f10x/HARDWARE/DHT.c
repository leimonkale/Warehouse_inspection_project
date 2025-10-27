#include "DHT.h"
#include "delay.h"
#include "printf.h"
#include "ALL_DATA.h"
#include "ALL_DEFINE.h"

int temp, humi;
void DHT_OutPut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}

void DHT_InPut(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void DHT_Out(int x)
{
	if(x == 1)
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
	else
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
}

int DHT_In(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
}

int DHT_Start(void)
{
	int i = 0;
again:
	DHT_OutPut();	//引脚配置为输出模式
	DHT_Out(0);		//输出一个0
	delay_ms(20);	//延时大于18ms
	DHT_Out(1);		//输出一个1
	delay_us(30);	//延时20-40us
	
	DHT_InPut();	//配置为输入模式
	if(DHT_In() == 0)
	{
		//接收80us的低电平
		while(DHT_In() == 0);
		//接收80us的高电平
		while(DHT_In() == 1);
		
		return 1;
	}
	else
	{
		if(i++ >= 10)
			return -1;
		goto again;
	}
	
}

int DHT_ReadValue()
{
	
	unsigned char data[5] = {0};
	//发送起始信号
	if(DHT_Start() == 1)
	{
		for(int i = 0; i < 5; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				//接收间隔信号
				while(DHT_In() == 0);
				if(DHT_In() == 1)
				{
					delay_us(30);	//区分数据0和数据1
					if(DHT_In() == 1)
					{
						//数据1
						data[i] |= 1<<(7-j);
						while(DHT_In() == 1);	//等待剩余的高电平结束
					}
				}
				else
					
					return -2;
			}
		}
		//printf("Received: %02X %02X %02X %02X %02X\n", 
         // data[0], data[1], data[2], data[3], data[4]);
		//校验
		unsigned char check = data[0] + data[1] + data[2] + data[3];
		if(check == data[4])
		{
			temp = data[2];
			humi = data[0];
			
			return 0;
		}
		else
			
			return -3;
	}
	else
		
		return -1;	//起始信号错误
	 
}

