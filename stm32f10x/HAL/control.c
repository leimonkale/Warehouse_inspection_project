#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "control.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"

char moter_com = 0x00;

void Analysis_wires(void)   //巡线解析控制函数
{
	if(wires & LINE_INIT)
	{
		if(wires & STRAIGHT_LINE)
		{
			printf("走直线\r\n");  
			wires &= ~(STRAIGHT_LINE);
			wires |= LINE_INIT;
		}
		if(wires & SLIGHTLY_LEFT)
		{
			printf("左偏\r\n");  
			wires &= ~(SLIGHTLY_LEFT);
			wires |= LINE_INIT;
		}
		if(wires & SLIGHTLY_RIGHT)
		{
			printf("右偏\r\n");  
			wires &= ~(SLIGHTLY_RIGHT);
			wires |= LINE_INIT;
		}
		if(wires & TURN_LEFT)
		{
			printf("左转\r\n");  
			wires &= ~(TURN_LEFT);
			wires |= LINE_INIT;
		}
		if(wires & TURN_RIGHT)
		{
			printf("右转\r\n");  
			wires &= ~(TURN_RIGHT);
			wires |= LINE_INIT;
		}
	}else{
		//printf("电机未初始化\r\n");
	}
}

void Analysis_hcsro4(void)                  //超声避障解析
{
	if(hcsr04_flag)
	{
		wires &= ~LINE_INIT; 
		switch(hcsr04_flag)
		{
			case HCSR04_IS_LE:
				 printf("右转\r\n");
				break;
			case HCSR04_IS_RI:
				printf("左转\r\n");
				break;
			case HCSR04_IS:
				printf("左或右转\r\n");
				break;
			case HCSR04_IS_RI_LE:
				printf("停止\r\n");
				break;
			default:
				wires |= LINE_INIT;
		}
		hcsr04_flag = 0x00;
	}

}
