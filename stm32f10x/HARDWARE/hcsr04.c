#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "hcsr04.h"
#include "printf.h"

#include "stm32f10x.h"
#include <stddef.h>

HCSR04_HandleTypeDef hcsr04;
float hcsr04_distance = 0;
char hcsr04_flag = 0x00;
/**
 * @brief  初始化超声波模块结构体
 */
void Hcsr04_Init(void)
{
	hcsr04.TIMx = TIM2;
	hcsr04.ECHO_GPIOx = GPIOB;
	hcsr04.ECHO_GPIO_Pin = HCSR04_ECHO;
	hcsr04.TRIG_GPIOx = GPIOB;
	hcsr04.TRIG_GPIO_Pin = HCSR04_TRIG;
	
	HCSR04_Init(&hcsr04);
}

/**
 * @brief  初始化超声波模块
 * @param  hcsr04: 超声波模块结构体指针
 * @retval 无
 */
void HCSR04_Init(HCSR04_HandleTypeDef* hcsr04) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    ///使能GPIO时钟
    if (hcsr04->TRIG_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (hcsr04->TRIG_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (hcsr04->TRIG_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    if (hcsr04->ECHO_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (hcsr04->ECHO_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (hcsr04->ECHO_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 使能定时器时钟
    if (hcsr04->TIMx == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    else if (hcsr04->TIMx == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (hcsr04->TIMx == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (hcsr04->TIMx == TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置TRIG引脚(推挽输出)
    GPIO_InitStructure.GPIO_Pin = hcsr04->TRIG_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(hcsr04->TRIG_GPIOx, &GPIO_InitStructure);
    
    // 配置ECHO引脚(上拉输入)
    GPIO_InitStructure.GPIO_Pin = hcsr04->ECHO_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(hcsr04->ECHO_GPIOx, &GPIO_InitStructure);
    
    // 初始化TRIG为低电平
    GPIO_ResetBits(hcsr04->TRIG_GPIOx, hcsr04->TRIG_GPIO_Pin);
    
    // 配置定时器(1us计数)
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;  // 最大计数65535us
    TIM_TimeBaseStructure.TIM_Prescaler = 71;   // 72MHz/(71+1)=1MHz, 1us计数一次
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(hcsr04->TIMx, &TIM_TimeBaseStructure);
    
    // 启动定时器
    TIM_Cmd(hcsr04->TIMx, ENABLE);
    
    // 初始化参数
    hcsr04->timeout_us = 30000;  // 30ms超时(对应最大距离约5米)
    hcsr04->distance = 0.0f;
}

/**
 * @brief  测量距离
 * @param  hcsr04: 超声波模块结构体指针
 * @retval 0:成功 1:超时
 */
uint8_t HCSR04_Measure(HCSR04_HandleTypeDef* hcsr04) {
    uint32_t start_time, end_time, duration;
	uint32_t i;
    
    // 发送10us触发脉冲
    GPIO_SetBits(hcsr04->TRIG_GPIOx, hcsr04->TRIG_GPIO_Pin);
    for (i = 0; i < 720; i++) __NOP();  // 约10us延时
    GPIO_ResetBits(hcsr04->TRIG_GPIOx, hcsr04->TRIG_GPIO_Pin);
    
    // 等待ECHO上升沿
    start_time = 0;
    while (GPIO_ReadInputDataBit(hcsr04->ECHO_GPIOx, hcsr04->ECHO_GPIO_Pin) == 0) {
        if (start_time++ > hcsr04->timeout_us) {
            hcsr04->distance = -1.0f;  // 超时标志
            return 1;
        }
        while (hcsr04->TIMx->CNT < 1);  // 等待1us
        hcsr04->TIMx->CNT = 0;
    }
    
    // 记录上升沿时间
    start_time = hcsr04->TIMx->CNT;
    
    // 等待ECHO下降沿
    while (GPIO_ReadInputDataBit(hcsr04->ECHO_GPIOx, hcsr04->ECHO_GPIO_Pin) == 1) {
        if (hcsr04->TIMx->CNT - start_time > hcsr04->timeout_us) {
            hcsr04->distance = -1.0f;  // 超时标志
            return 1;
        }
    }
    
    // 计算持续时间(us)
    end_time = hcsr04->TIMx->CNT;
    duration = end_time - start_time;
    
    // 计算距离(声速343.2m/s, 距离=时间*声速/2)
    hcsr04->distance = (duration * 0.03432f) / 2.0f;
    hcsr04_distance = hcsr04->distance;
    return 0;
}

/**
 * @brief  获取当前距离
 * @param  hcsr04: 超声波模块结构体指针
 * @retval 距离值(厘米), 负数表示超时
 */
void HCSR04_GetDistance(HCSR04_HandleTypeDef* hcsr04) {
   hcsr04_distance = hcsr04->distance;
}

void HCSR04_GetFlag(void)   //超声标志位更新
{
	if(hcsr04_distance <= 10 && hcsr04_distance>= 0)
	{
		switch(engin_flag)
		{
			case ENGIN_IS:
				hcsr04_flag	|= HCSR04_IS;
				break;
			case ENGIN_RI:
				hcsr04_flag	|= HCSR04_RI;
				break;
			case ENGIN_LE:
				hcsr04_flag	|= HCSR04_LE;
				break;
		}
	}
}
