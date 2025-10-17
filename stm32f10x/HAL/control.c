#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "control.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"

char moter_com = 0x00;

void Analysis_wires(void)   //Ѳ�߽������ƺ���
{
	if(wires & LINE_INIT)
	{
		if(wires & STRAIGHT_LINE)
		{
			printf("��ֱ��\r\n");  
			wires &= ~(STRAIGHT_LINE);
			wires |= LINE_INIT;
		}
		if(wires & SLIGHTLY_LEFT)
		{
			printf("��ƫ\r\n");  
			wires &= ~(SLIGHTLY_LEFT);
			wires |= LINE_INIT;
		}
		if(wires & SLIGHTLY_RIGHT)
		{
			printf("��ƫ\r\n");  
			wires &= ~(SLIGHTLY_RIGHT);
			wires |= LINE_INIT;
		}
		if(wires & TURN_LEFT)
		{
			printf("��ת\r\n");  
			wires &= ~(TURN_LEFT);
			wires |= LINE_INIT;
		}
		if(wires & TURN_RIGHT)
		{
			printf("��ת\r\n");  
			wires &= ~(TURN_RIGHT);
			wires |= LINE_INIT;
		}
	}else{
		//printf("���δ��ʼ��\r\n");
	}
}

void Analysis_hcsro4(void)                  //�������Ͻ���
{
	if(hcsr04_flag)
	{
		wires &= ~LINE_INIT; 
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
			default:
				wires |= LINE_INIT;
		}
		hcsr04_flag = 0x00;
	}

}
