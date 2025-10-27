#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "scheduler.h"
#include "delay.h"
#include "uart1.h"
#include "printf.h"
#include "control.h"
#include "hcsr04.h"
#include "engin.h"
#include "wifi.h"
#include "usart2.h"
//#include "usart.h"
#include "DHT.h"
#include "adc.h"

loop_t loop = {0}; 

u32 time[10],time_sum;
 
extern int16_t MpuOffset[8];

void Loop_check()
{
	loop.cnt_2ms++;
	loop.cnt_4ms++;
	loop.cnt_6ms++;
	loop.cnt_10ms++;
	loop.cnt_20ms++;
	loop.cnt_50ms++;
	loop.cnt_200ms++;
	loop.cnt_1000ms++;
	loop.cnt_2000ms++;
	if( loop.check_flag >= 1)
	{
		loop.err_flag ++;// 2ms 
	}
	else
	{
		loop.check_flag += 1;   //该标志位在循环后面清0
	}
	
}
void main_loop()
{
	if( loop.check_flag >= 1 )
	{
		
		if( loop.cnt_2ms >= 1 )
		{
			loop.cnt_2ms = 0;
			
			Duty_2ms();	 					//周期2ms的任务
		}
		if( loop.cnt_4ms >= 2 )
		{
			loop.cnt_4ms = 0;
			Duty_4ms();						//周期4ms的任务
		}
		if( loop.cnt_6ms >= 3 )
		{
			loop.cnt_6ms = 0;
			Duty_6ms();						//周期6ms的任务
		}
		if( loop.cnt_10ms >= 5 )
		{
			loop.cnt_10ms = 0;
			Duty_10ms();					//周期10ms的任务
		} 
		if( loop.cnt_20ms >= 10 )
		{
			loop.cnt_20ms = 0;
			Duty_20ms();					//周期20ms的任务
		}
		if( loop.cnt_50ms >= 25 )
		{
			loop.cnt_50ms = 0;
			Duty_50ms();					//周期50ms的任务
		}
		if(loop.cnt_200ms >= 100)           
		{
			loop.cnt_200ms = 0;
			Duty_200ms();                   //周期200ms的任务
		}
		if( loop.cnt_1000ms >= 500)
		{
			loop.cnt_1000ms = 0;
			Duty_1000ms();				//周期1s的任务
		}
		if(loop.cnt_2000ms >= 1000)
		{
			loop.cnt_2000ms = 0;
			Duty_2000ms();
		}
		loop.check_flag = 0;		//循环运行完毕标志
	}
}
/////////////////////////////////////////////////////////
void Duty_2ms()
{
	time[0] = GetSysTime_us();
								//读取传感器信息
								
	
	time[0] = GetSysTime_us() - time[0];
}
//////////////////////////////////////////////////////////
void Duty_4ms()
{
	time[1] = GetSysTime_us();
								//解析服务器命令
								//返回数据到服务器
	                         
	time[1] = GetSysTime_us() - time[1];
}
//////////////////////////////////////////////////////////
void Duty_6ms()
{
	time[2] = GetSysTime_us();
	
	  
	time[2] = GetSysTime_us() - time[2];
}
/////////////////////////////////////////////////////////
void Duty_10ms()
{
	time[3] = GetSysTime_us();
	//printf("10ms\r\n");
	
	
	time[3] = GetSysTime_us() - time[3];
	
}
/////////////////////////////////////////////////////////
void Duty_20ms()
{
	time[4] = GetSysTime_us();
	
	motor_mod();					//电机模式控制
	//servo_set_angle();              //舵机角度控制
	time[4] = GetSysTime_us() - time[4];
	//printf("time[20]:%d\r\n",time[4]);

}
//////////////////////////////////////////////////////////
void Duty_50ms()
{
	time[5] = GetSysTime_us();
	process_uart_command();
	HCSR04_GetFlag();                  //检测有无障碍
	if (wifi_receive_mqtt_msg(received_msg))
    {
            printf("处理消息: %s\r\n", received_msg);
			Find_Line();
     }
	//printf("distance:%d\r\n",distance);
	time[5] = GetSysTime_us() - time[5];
	//printf("time[50]:%d\r\n",time[5]);
}

/////////////////////////////////////////////////////////////
void Duty_200ms()
{
    time[6] = GetSysTime_us();
	
    //printf("HCSR04:%d\r\n",HCSR04_Measure(&hcsr04));
	//printf("hcsr04_distance = %.4f\r\n",hcsr04_distance);
	
    time[6] = GetSysTime_us() - time[6];
}

//////////////////////////end///////////////////////////////////////////
void Duty_1000ms()
{
	time[7] = GetSysTime_us();
	DHT_ReadValue();
	Get_ADCValue();
	//printf("adc:%d\r\n",adc_num);
	//printf("humi:%d temp:%d\r\n",humi,temp);
	
	time[7] = GetSysTime_us() - time[7];
	printf("time[1000]:%d\r\n",time[7]);
}
//////////////////////////end///////////////////////////////////////////
void Duty_2000ms()
{
	time[8] = GetSysTime_us();
	send_data_mqtt();
	//printf("adc:%d\r\n",adc_num);
	//printf("humi:%d temp:%d\r\n",humi,temp);
	
	time[8] = GetSysTime_us() - time[8];
	printf("time[2000]:%d\r\n",time[8]);
}
//////////////////////////end///////////////////////////////////////////
