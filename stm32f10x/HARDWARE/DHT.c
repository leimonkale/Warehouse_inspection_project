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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // ��������
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
	DHT_OutPut();	//��������Ϊ���ģʽ
	DHT_Out(0);		//���һ��0
	delay_ms(20);	//��ʱ����18ms
	DHT_Out(1);		//���һ��1
	delay_us(30);	//��ʱ20-40us
	
	DHT_InPut();	//����Ϊ����ģʽ
	if(DHT_In() == 0)
	{
		//����80us�ĵ͵�ƽ
		while(DHT_In() == 0);
		//����80us�ĸߵ�ƽ
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
	//������ʼ�ź�
	if(DHT_Start() == 1)
	{
		for(int i = 0; i < 5; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				//���ռ���ź�
				while(DHT_In() == 0);
				if(DHT_In() == 1)
				{
					delay_us(30);	//��������0������1
					if(DHT_In() == 1)
					{
						//����1
						data[i] |= 1<<(7-j);
						while(DHT_In() == 1);	//�ȴ�ʣ��ĸߵ�ƽ����
					}
				}
				else
					
					return -2;
			}
		}
		//printf("Received: %02X %02X %02X %02X %02X\n", 
         // data[0], data[1], data[2], data[3], data[4]);
		//У��
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
		
		return -1;	//��ʼ�źŴ���
	 
}

