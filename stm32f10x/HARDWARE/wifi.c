#include "stm32f10x.h"                  // Device header
#include "wifi.h"   	//ͷ�ļ���·����:stm32_-share-master\STM32����\HARDWARE\ESP8266
#include "delay.h"		
#include "usart.h"		//ʹ�ô���1����������
#include "usart2.h"		//ʹ�ô���2����espͨ��   ����ʹ�ö�ʱ����ÿ10ms���esp�Ƿ������ݸ�stm32.
#include <string.h>		//ʹ����strtr����������ͷ�ļ�
#include <stdio.h>
#include "ALL_DATA.h"
#include "ALL_DEFINE.h"

/*
			ESP01s       STM32
			 3V3----------3.3V
			 GND----------GND
			 RX-----------PA2
			 TX-----------PA3
			 RST----------PA4
*/

//��һ����wifiģ���ϵ�������һ��

void wifi_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);            		 
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void rst_wifi(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	delay_ms(1000);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
}


//�ڶ�������ʼ����ATָ������


//�жϴ��ڶ��յ��������ǲ���ǰ�涨���ack���ڴ���Ӧ������
u8* wifi_check_cmd(u8 *str)
{
	char *strx = 0;
	if(USART2_RX_STA&0X8000)
	{
		USART2_RX_BUF[USART2_RX_STA&0X7FFF] = 0;
		strx = strstr((const char*)USART2_RX_BUF,(const char*)str);
	}
	return (u8*)strx;
}


//��һ�����������
//cmd�����͵�ATָ��
//ack���ڴ��Ļش�
//time���ȴ�ʱ��(��λ10ms)
//����ֵ��0�����ͳɹ��� 1������ʧ��
u8 wifi_send_cmd(u8 *cmd,u8 *ack,u16 time)
{
	u8 res = 0;
	USART2_RX_STA = 0;
	u2_printf("%s\r\n",cmd);
	if(time)
	{
		while(--time)
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000) //���ڶ����յ�����
			{
				//�жϽ��ܵ������ǲ�����Ҫ��
				if(wifi_check_cmd(ack))
				{
					break;
				}
				USART2_RX_STA = 0;
			}
		}
		if(time == 0) res = 1;
	}
	return res;
	
}

//����������˳����ATָ��
void init_wifi(void)
{
	//1 AT
	while(wifi_send_cmd("AT","OK",50))
	{
		printf("AT��Ӧʧ��\r\n");
	}
	
	//2 ��Wi-Fiģ������ΪStation��STA��ģʽ
	while(wifi_send_cmd("AT+CWMODE=1","OK",50))
	{
		printf("STAģʽ����ʧ��\r\n");
	}
	
	//3 ����WIFI���û���������
//	while(wifi_send_cmd("AT+CWJAP=\"xiaomi\",\"123456789\"","OK",500))
	while(wifi_send_cmd("AT+CWJAP=\"mochen\",\"123456789a\"","OK",500))//��Ϊ�Լ���WiFi
	{
		printf("����WIFIʧ��3\r\n");
	}
	
	//4 ����MQTT�������
	while(wifi_send_cmd("AT+MQTTUSERCFG=0,1,\"MQTTID\",\"username\",\"password\",0,0,\"\"","OK",500))
	{
		printf("����WIFIʧ��4\r\n");
	}
  //5 ����MQTT��ip
	while(wifi_send_cmd("AT+MQTTCONN=0,\"broker.emqx.io\",1883,1","OK",500))//��ѵ�mqtt������
	{
		printf("����MQTT������ʧ��\r\n");
	}	
	//6 ��������
	while(wifi_send_cmd("AT+MQTTSUB=0,\"mochen9227\",0","OK",50))//�Լ�����������
	{
		printf("��������ʧ��\r\n");
	}	
	/*
		
	*/
	
}

char buf2[50];

void send_data_mqtt(void)
{
	//DHT11_Read_Data(&temp,&humi);
	//printf("temp:%d%d,humi:%d%d\r\n",temp/10,temp%10,humi/10,humi%10);
	sprintf(buf2, "AT+MQTTPUB=0,\"mochen9227\",\"{temp:%d%d\\,humi:%d%d\\,adc_num:%d\\}\",0,0", 
            temp/10, temp%10, humi/10, humi%10,adc_num/10);
	printf("buf2:%s\r\n",buf2);
	wifi_send_cmd((u8 *)buf2,"OK",100);//����Ҫ��һ�У���esp��������
	
}

