#ifndef __ALL_DEFINE_H__
#define __ALL_DEFINE_H__
#include "stm32f10x.h"
#include "ALL_DATA.h"

#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED  1
/***************Ѳ�߷���ֵ����******************/
#undef LINE_INIT
#define LINE_INIT      (1<<7)        //��ʼ��(������־)
#undef STRAIGHT_LINE 
#define STRAIGHT_LINE  (1<<6)        //ֱ��

#undef SLIGHTLY_LEFT 
#define SLIGHTLY_LEFT  (1<<5)        //��ƫ
#undef SLIGHTLY_RIGHT 
#define SLIGHTLY_RIGHT (1<<4)        //��ƫ

#undef TURN_LEFT 
#define TURN_LEFT      (1<<2)        //��ת
#undef TURN_RIGHT 
#define TURN_RIGHT     (1<<1)        //��ת

#undef ACCEPT          
#define ACCEPT         (1<<0)        //�ж��յ��źű�־λ

/*********************************************/

/***************������־λ ******************/
#undef  HCSR04_IS
#define HCSR04_IS      (1<<1)       //��ǰ��
#undef  HCSR04_LE      
#define HCSR04_LE      (1<<2)       //��
#undef  HCSR04_RI      
#define HCSR04_RI      (1<<0)       //�ҷ�

#define HCSR04_IS_LE    0x6   // ��ǰ�� + ��
#define HCSR04_IS_RI    0x3   // ��ǰ�� + �ҷ�
#define HCSR04_IS_RI_LE 0x7  // ��ǰ�� + �ҷ� + ��

/*********************************************/

/***************�����־λ ******************/
#undef  ENGIN_IS
#define ENGIN_IS      (1<<1)       //��ǰ��
#undef  ENGIN_LE      
#define ENGIN_LE      (1<<2)       //��
#undef  ENGIN_RI      
#define ENGIN_RI      (1<<0)       //�ҷ�

/*********************************************/

/***************UART1 GPIO����******************/ //�Ӿ�ʶ�𴮿�
#define RCC_UART1		RCC_APB2Periph_GPIOA
#define GPIO_UART1		GPIOA
#define UART1_Pin_TX	GPIO_Pin_9
#define UART1_Pin_RX	GPIO_Pin_10
/*********************************************/

/***************UART2 GPIO����******************/  //printf����
#define RCC_UART2		RCC_APB2Periph_GPIOA
#define GPIO_UART2		GPIOA
#define UART2_Pin_TX	GPIO_Pin_2
#define UART2_Pin_RX	GPIO_Pin_3
/*********************************************/

/***************������ GPIO����******************/ 
#define RCC_HCSR04		RCC_APB2Periph_GPIOA
#define GPIO_HCSR04		GPIOA
#define HCSR04_ECHO  	GPIO_Pin_14
#define HCSR04_TRIG	    GPIO_Pin_15
/*********************************************/

/***************��� GPIO����******************/  
#define RCC_engin		RCC_APB2Periph_GPIOA
#define GPIO_engin		GPIOA
#define engin_pwm   	GPIO_Pin_8
/*********************************************/

extern volatile uint32_t SysTick_count;   //ϵͳ�δ�ʱ�Ӽ���

#endif

