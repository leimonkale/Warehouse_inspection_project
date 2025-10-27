#include "stm32f10x.h"                  // Device header
#include "wifi.h"   	//头文件，路径在:stm32_-share-master\STM32程序\HARDWARE\ESP8266
#include "delay.h"		
//#include "usart.h"		//使用串口1，向电脑输出
#include "usart2.h"		//使用串口2，和esp通信   里面使用定时器，每10ms检测esp是否有数据给stm32.
#include <string.h>		//使用了strtr函数，加入头文件
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

//第一步、wifi模块上电先重启一下

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


//第二步、开始进行AT指令配置


//判断串口二收到的数据是不是前面定义的ack（期待的应答结果）
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


//放一个命令函数在这
//cmd：发送的AT指令
//ack：期待的回答
//time：等待时间(单位10ms)
//返回值：0、发送成功。 1、发送失败
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
			if(USART2_RX_STA&0X8000) //串口二接收到数据
			{
				//判断接受的数据是不是想要的
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

//第三步、按顺序发送AT指令
void init_wifi(void)
{
	//1 AT
	while(wifi_send_cmd("AT","OK",50))
	{
		printf("AT响应失败\r\n");
	}
	
	//2 将Wi-Fi模块设置为Station（STA）模式
	while(wifi_send_cmd("AT+CWMODE=1","OK",50))
	{
		printf("STA模式设置失败\r\n");
	}
	
	//3 连接WIFI的用户名和密码
//	while(wifi_send_cmd("AT+CWJAP=\"xiaomi\",\"123456789\"","OK",500))
	while(wifi_send_cmd("AT+CWJAP=\"mochen\",\"123456789a\"","OK",500))//改为自己的WiFi
	{
		printf("连接WIFI失败3\r\n");
	}
	
	//4 设置MQTT相关属性
	while(wifi_send_cmd("AT+MQTTUSERCFG=0,1,\"MQTTID\",\"username\",\"password\",0,0,\"\"","OK",500))
	{
		printf("连接WIFI失败4\r\n");
	}
  //5 连接MQTT的ip
	while(wifi_send_cmd("AT+MQTTCONN=0,\"broker.emqx.io\",1883,1","OK",500))//免费的mqtt服务器
	{
		printf("连接MQTT服务器失败\r\n");
	}	
	//6 订阅主题
	while(wifi_send_cmd("AT+MQTTSUB=0,\"mochen9227\",0","OK",50))//自己创建的主题
	{
		printf("订阅主题失败\r\n");
	}	

}

char buf2[50];

void send_data_mqtt(void)
{
	//DHT11_Read_Data(&temp,&humi);
	//printf("temp:%d%d,humi:%d%d\r\n",temp/10,temp%10,humi/10,humi%10);
	sprintf(buf2, "AT+MQTTPUB=0,\"mochen9227\",\"{temp:%d%d\\,humi:%d%d\\,adc_num:%d\\}\",0,0", 
            temp/10, temp%10, humi/10, humi%10,adc_num/10);
	//printf("buf2:%s\r\n",buf2);
	wifi_send_cmd((u8 *)buf2,"OK",100);//最重要的一行，给esp发送命令
	
}

char received_msg[100];
// 接收并解析 MQTT 订阅消息
// 参数: msg_buf -> 用于保存接收到的消息内容
// 返回值: 0 表示未收到；1 表示成功解析到一条消息
int wifi_receive_mqtt_msg(char *msg_buf)
{
    if (USART2_RX_STA & 0x8000)  // 串口2有接收完成标志
    {
        USART2_RX_BUF[USART2_RX_STA & 0x7FFF] = '\0'; // 添加字符串结束符
        char *start = strstr((const char*)USART2_RX_BUF, "+MQTTSUBRECV:");
        
        // 严格检查是否是订阅消息格式：+MQTTSUBRECV:0,"topic",length,message
        if (start)
        {
            // 检查消息格式是否符合订阅消息的格式
            char *first_comma = strchr(start, ',');
            if (first_comma)
            {
                char *second_comma = strchr(first_comma + 1, ',');
                if (second_comma)
                {
                    char *third_comma = strchr(second_comma + 1, ',');
                    if (third_comma)
                    {
                        // 提取最后一个逗号后的内容
                        strcpy(msg_buf, third_comma + 1);
                        
                        // 去除可能的换行符或回车符
                        char *end = msg_buf + strlen(msg_buf) - 1;
                        while(end >= msg_buf && (*end == '\n' || *end == '\r')) {
                            *end = '\0';
                            end--;
                        }
                        
						// 确保缓冲区有足够空间添加\n
                        size_t len = strlen(msg_buf);
                        if(len < 100 - 1) { // 假设MAX_MSG_BUF_SIZE是msg_buf的大小
                            msg_buf[len] = '\0';      
                        }
						
                        USART2_RX_STA = 0; // 清空接收状态
                        return 1;
                    }
                }
            }
        }
        USART2_RX_STA = 0; // 无有效消息则清空接收状态
    }

    return 0;
}
