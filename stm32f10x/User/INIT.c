
#include "ALL_DEFINE.h" 
#include "ALL_DATA.h"
#include "uart1.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"

volatile uint32_t SysTick_count; //系统时间计数
volatile uint8_t spl_flag; //系统时间计数


///////////////全部初始化//////////////////////////////////
void ALL_Init(void)
{
	
	printf_init(115200);    //串口2初始化
	uart_init(115200);      //串口1初始化
	wires |= LINE_INIT;     //电机初始化标志
	
	Hcsr04_Init();          //超声波结构体初始化
	HCSR04_Init(&hcsr04);   //超声波模块初始化
	
	gpio_engin_init();
	tim1_init();            //舵机pwm初始化
	
}










