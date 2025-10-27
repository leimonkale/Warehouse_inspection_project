
#include "ALL_DEFINE.h" 
#include "ALL_DATA.h"
#include "delay.h"
#include "uart1.h"
#include "usart2.h"
//#include "printf.h"
#include "hcsr04.h"
#include "engin.h"
#include "control.h"
#include "motor.h"
#include "wifi.h"
#include "usart.h"
#include "DHT.h"
#include "adc.h"

volatile uint32_t SysTick_count; //系统时间计数
volatile uint8_t spl_flag; //系统时间计数


///////////////全部初始化//////////////////////////////////
void ALL_Init(void)
{
	
	printf_init(115200);    //串口2初始化
	uart_init(115200);      //串口1初始化(k20数据)

	//printf_uart_init(115200);
	
	
	wifi_GPIO_Init();        //esp8266
	USART2_Init(115200); //连接ESP8266
	rst_wifi(); 	//重启esp
	init_wifi();	//重启后初始化esp
	
	
	ADC1_Init();
	
	mod = 3;
	
	Hcsr04_Init();          //超声波结构体初始化
	HCSR04_Init(&hcsr04);   //超声波模块初始化
	
	gpio_engin_init();
	tim1_init();            //舵机pwm初始化
	
	
	Motor1_Init();
	Motor2_Init();
	Motor_ctr_Init();
	
	wires |= LINE_INIT;     //电机初始化标志
	printf("电机初始化\r\n");
}










