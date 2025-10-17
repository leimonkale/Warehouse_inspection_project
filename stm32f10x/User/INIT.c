
#include "ALL_DEFINE.h" 
#include "ALL_DATA.h"
#include "uart1.h"
#include "printf.h"
#include "hcsr04.h"
#include "engin.h"

volatile uint32_t SysTick_count; //ϵͳʱ�����
volatile uint8_t spl_flag; //ϵͳʱ�����


///////////////ȫ����ʼ��//////////////////////////////////
void ALL_Init(void)
{
	
	printf_init(115200);    //����2��ʼ��
	uart_init(115200);      //����1��ʼ��
	wires |= LINE_INIT;     //�����ʼ����־
	
	Hcsr04_Init();          //�������ṹ���ʼ��
	HCSR04_Init(&hcsr04);   //������ģ���ʼ��
	
	gpio_engin_init();
	tim1_init();            //���pwm��ʼ��
	
}










