#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "scheduler.h"
#include "delay.h"
#include "uart1.h"
#include "printf.h"
#include "control.h"
#include "hcsr04.h"
#include "engin.h"

loop_t loop = {0}; 
u32 time[10],time_sum;
 
extern int16_t MpuOffset[6];

void Loop_check()
{
	loop.cnt_2ms++;
	loop.cnt_4ms++;
	loop.cnt_6ms++;
	loop.cnt_10ms++;
	loop.cnt_20ms++;
	loop.cnt_50ms++;
	loop.cnt_1000ms++;

	if( loop.check_flag >= 1)
	{
		loop.err_flag ++;// 2ms 
	}
	else
	{
		loop.check_flag += 1;   //�ñ�־λ��ѭ��������0
	}
	
}
void main_loop()
{
	if( loop.check_flag >= 1 )
	{
		
		if( loop.cnt_2ms >= 1 )
		{
			loop.cnt_2ms = 0;
			
			Duty_2ms();	 					//����2ms������
		}
		if( loop.cnt_4ms >= 2 )
		{
			loop.cnt_4ms = 0;
			Duty_4ms();						//����4ms������
		}
		if( loop.cnt_6ms >= 3 )
		{
			loop.cnt_6ms = 0;
			Duty_6ms();						//����6ms������
		}
		if( loop.cnt_10ms >= 5 )
		{
			loop.cnt_10ms = 0;
			Duty_10ms();					//����10ms������
		} 
		if( loop.cnt_20ms >= 10 )
		{
			loop.cnt_20ms = 0;
			Duty_20ms();					//����20ms������
		}
		if( loop.cnt_50ms >= 25 )
		{
			loop.cnt_50ms = 0;
			Duty_50ms();					//����50ms������
		}
		if( loop.cnt_1000ms >= 500)
		{
			loop.cnt_1000ms = 0;
			Duty_1000ms();				//����1s������
		}
		loop.check_flag = 0;		//ѭ��������ϱ�־
	}
}
/////////////////////////////////////////////////////////
void Duty_2ms()
{
	time[0] = GetSysTime_us();
								//��ȡ��������Ϣ
								//�������
	//printf("2ms\r\n");
	time[0] = GetSysTime_us() - time[0];
}
//////////////////////////////////////////////////////////
void Duty_4ms()
{
	time[1] = GetSysTime_us();
								//��������������
								//�������ݵ�������
	                           
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
	
	
	time[3] = GetSysTime_us() - time[3];
}
/////////////////////////////////////////////////////////
void Duty_20ms()
{
	time[4] = GetSysTime_us();
	Analysis_wires();					//�����Ӿ�ʶ��,�������wires��
									   //���ģʽ����
	servo_set_angle();                 //����Ƕȿ���
	HCSR04_GetFlag();                  //��������ϰ�
	time[4] = GetSysTime_us() - time[4];
}
//////////////////////////////////////////////////////////
void Duty_50ms()
{
	time[5] = GetSysTime_us();
	HCSR04_Measure(&hcsr04);             //���������,�������hcsr04_distance��
	
	time[5] = GetSysTime_us() - time[5];
}
/////////////////////////////////////////////////////////////
void Duty_1000ms()
{
	time[6] = GetSysTime_us();
	Analysis_hcsro4();                  //��������
	
	
	time[6] = GetSysTime_us() - time[6];
}

//////////////////////////end///////////////////////////////////////////
