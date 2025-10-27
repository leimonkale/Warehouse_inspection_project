
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

volatile uint32_t SysTick_count; //ϵͳʱ�����
volatile uint8_t spl_flag; //ϵͳʱ�����


///////////////ȫ����ʼ��//////////////////////////////////
void ALL_Init(void)
{
	
	printf_init(115200);    //����2��ʼ��
	uart_init(115200);      //����1��ʼ��(k20����)

	//printf_uart_init(115200);
	
	
	wifi_GPIO_Init();        //esp8266
	USART2_Init(115200); //����ESP8266
	rst_wifi(); 	//����esp
	init_wifi();	//�������ʼ��esp
	
	
	ADC1_Init();
	
	mod = 3;
	
	Hcsr04_Init();          //�������ṹ���ʼ��
	HCSR04_Init(&hcsr04);   //������ģ���ʼ��
	
	gpio_engin_init();
	tim1_init();            //���pwm��ʼ��
	
	
	Motor1_Init();
	Motor2_Init();
	Motor_ctr_Init();
	
	wires |= LINE_INIT;     //�����ʼ����־
	printf("�����ʼ��\r\n");
}










