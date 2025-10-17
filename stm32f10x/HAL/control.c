#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "control.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"

char moter_com = 0x00;
int32_t mod;   //ģʽ
void motor_mod(void)
{
	switch(mod)
	{
		case 1:
			Analysis_wires();	break;				//����wires�е�����,�����Ƶ��
		case 2:
			Analysis_hcsro4();  break;                //�������� ,�����Ƶ��
	}
}


void Analysis_wires(void)   //Ѳ�߽������ƺ���
{
	if(wires & LINE_INIT)
	{
		if(wires & STRAIGHT_LINE)
		{
			printf("0\r\n");  
		}
		if(wires & SLIGHTLY_LEFT)
		{
			printf("-1\r\n");  
		}
		if(wires & SLIGHTLY_RIGHT)
		{
			printf("1\r\n");  
		}
		if(wires & TURN_LEFT)
		{
			printf("-2\r\n");  
		}
		if(wires & TURN_RIGHT)
		{
			printf("2\r\n");  
		}
	}else{
		//printf("���δ��ʼ��\r\n");
		return;
	}
	
}


void Analysis_hcsro4(void)                  //�������Ͻ���
{
	if(hcsr04_flag)
	{
		//wires &= ~LINE_INIT; 
		switch(hcsr04_flag)
		{
			case HCSR04_IS_LE:
				 printf("��ת\r\n");
				break;
			case HCSR04_IS_RI:
				printf("��ת\r\n");
				break;
			case HCSR04_IS:
				printf("�����ת\r\n");
				break;
			case HCSR04_IS_RI_LE:
				printf("ֹͣ\r\n");
				break;
		}

	}

}
