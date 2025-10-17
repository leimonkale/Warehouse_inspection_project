#ifndef __ALL_DEFINE_H__
#define __ALL_DEFINE_H__
#include "stm32f10x.h"
#include "ALL_DATA.h"

#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED  1
/***************巡线返回值处理******************/
#undef LINE_INIT
#define LINE_INIT      (1<<7)        //初始化(启动标志)
#undef STRAIGHT_LINE 
#define STRAIGHT_LINE  (1<<6)        //直线

#undef SLIGHTLY_LEFT 
#define SLIGHTLY_LEFT  (1<<5)        //左偏
#undef SLIGHTLY_RIGHT 
#define SLIGHTLY_RIGHT (1<<4)        //右偏

#undef TURN_LEFT 
#define TURN_LEFT      (1<<2)        //左转
#undef TURN_RIGHT 
#define TURN_RIGHT     (1<<1)        //右转

#undef ACCEPT          
#define ACCEPT         (1<<0)        //中断收到信号标志位

/*********************************************/

/***************超声标志位 ******************/
#undef  HCSR04_IS
#define HCSR04_IS      (1<<1)       //正前方
#undef  HCSR04_LE      
#define HCSR04_LE      (1<<2)       //左方
#undef  HCSR04_RI      
#define HCSR04_RI      (1<<0)       //右方

#define HCSR04_IS_LE    0x6   // 正前方 + 左方
#define HCSR04_IS_RI    0x3   // 正前方 + 右方
#define HCSR04_IS_RI_LE 0x7  // 正前方 + 右方 + 左方

/*********************************************/

/***************舵机标志位 ******************/
#undef  ENGIN_IS
#define ENGIN_IS      (1<<1)       //正前方
#undef  ENGIN_LE      
#define ENGIN_LE      (1<<2)       //左方
#undef  ENGIN_RI      
#define ENGIN_RI      (1<<0)       //右方

/*********************************************/

/***************UART1 GPIO定义******************/ //视觉识别串口
#define RCC_UART1		RCC_APB2Periph_GPIOA
#define GPIO_UART1		GPIOA
#define UART1_Pin_TX	GPIO_Pin_9
#define UART1_Pin_RX	GPIO_Pin_10
/*********************************************/

/***************UART2 GPIO定义******************/  //printf串口
#define RCC_UART2		RCC_APB2Periph_GPIOA
#define GPIO_UART2		GPIOA
#define UART2_Pin_TX	GPIO_Pin_2
#define UART2_Pin_RX	GPIO_Pin_3
/*********************************************/

/***************超声波 GPIO定义******************/ 
#define RCC_HCSR04		RCC_APB2Periph_GPIOA
#define GPIO_HCSR04		GPIOA
#define HCSR04_ECHO  	GPIO_Pin_14
#define HCSR04_TRIG	    GPIO_Pin_15
/*********************************************/

/***************舵机 GPIO定义******************/  
#define RCC_engin		RCC_APB2Periph_GPIOA
#define GPIO_engin		GPIOA
#define engin_pwm   	GPIO_Pin_8
/*********************************************/

extern volatile uint32_t SysTick_count;   //系统滴答时钟计数

#endif

