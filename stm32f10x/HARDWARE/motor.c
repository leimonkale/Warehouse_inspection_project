#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "printf.h"
#include "stm32f10x.h"



/**
 * @brief  初始化电机控制
 * @param  motor: 电机结构体指针
 * @retval 无
 */
void Motor_Init(Motor_HandleTypeDef* motor) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // 使能GPIO时钟
    if (motor->IN1_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (motor->IN1_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (motor->IN1_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    if (motor->IN2_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (motor->IN2_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (motor->IN2_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	if (motor->PWM_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (motor->PWM_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (motor->PWM_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 使能定时器时钟
    if (motor->TIMx == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    else if (motor->TIMx == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (motor->TIMx == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (motor->TIMx == TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置方向控制引脚
    GPIO_InitStructure.GPIO_Pin = motor->IN1_GPIO_Pin | motor->IN2_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor->IN1_GPIOx, &GPIO_InitStructure);  // IN1和IN2在同一个GPIO组
	
	GPIO_InitStructure.GPIO_Pin = motor->PWM_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出（定时器控制）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor->PWM_GPIOx, &GPIO_InitStructure);
    
    // 默认停止状态
    GPIO_ResetBits(motor->IN1_GPIOx, motor->IN1_GPIO_Pin);
    GPIO_ResetBits(motor->IN2_GPIOx, motor->IN2_GPIO_Pin);
    
    // 配置定时器用于PWM输出
    // 配置为10kHz PWM，适合大多数直流电机
    TIM_TimeBaseStructure.TIM_Period = 999;  // 周期 = (999 + 1) / 72MHz * 1000 = ~13.89us，频率约72kHz
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  // 分频，72MHz/(71+1)=1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(motor->TIMx, &TIM_TimeBaseStructure);
    
    // 配置PWM输出通道
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始占空比0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // 根据通道配置
    if (motor->TIM_Channel == TIM_Channel_1) {
        TIM_OC1Init(motor->TIMx, &TIM_OCInitStructure);
        TIM_OC1PreloadConfig(motor->TIMx, TIM_OCPreload_Enable);
    } else if (motor->TIM_Channel == TIM_Channel_2) {
        TIM_OC2Init(motor->TIMx, &TIM_OCInitStructure);
        TIM_OC2PreloadConfig(motor->TIMx, TIM_OCPreload_Enable);
    } else if (motor->TIM_Channel == TIM_Channel_3) {
        TIM_OC3Init(motor->TIMx, &TIM_OCInitStructure);
        TIM_OC3PreloadConfig(motor->TIMx, TIM_OCPreload_Enable);
    } else if (motor->TIM_Channel == TIM_Channel_4) {
        TIM_OC4Init(motor->TIMx, &TIM_OCInitStructure);
        TIM_OC4PreloadConfig(motor->TIMx, TIM_OCPreload_Enable);
    }
    
    // 使能定时器预加载寄存器
    TIM_ARRPreloadConfig(motor->TIMx, ENABLE);
    
    // 启动定时器
    TIM_Cmd(motor->TIMx, ENABLE);
    
    // 高级定时器，需要使能主输出
    if (motor->TIMx == TIM1) {
        TIM_CtrlPWMOutputs(motor->TIMx, ENABLE);
    }
    
    // 初始化电机参数
    motor->speed = 0;
    motor->dir = MOTOR_FORWARD;
}

/**
 * @brief  设置电机方向
 * @param  motor: 电机结构体指针
 * @param  dir: 方向(MOTOR_FORWARD或MOTOR_BACKWARD)
 * @retval 无
 */
void Motor_SetDirection(Motor_HandleTypeDef* motor, Motor_DirectionTypeDef dir) {
    motor->dir = dir;
    
    if (dir == MOTOR_FORWARD) {
        GPIO_SetBits(motor->IN1_GPIOx, motor->IN1_GPIO_Pin);
        GPIO_ResetBits(motor->IN2_GPIOx, motor->IN2_GPIO_Pin);
    } else {
        GPIO_ResetBits(motor->IN1_GPIOx, motor->IN1_GPIO_Pin);
        GPIO_SetBits(motor->IN2_GPIOx, motor->IN2_GPIO_Pin);
    }
}

/**
 * @brief  设置电机速度
 * @param  motor: 电机结构体指针
 * @param  speed: 速度值(0-100)
 * @retval 无
 */
void Motor_SetSpeed(Motor_HandleTypeDef* motor, uint8_t speed) {
    uint16_t compareValue;
    
    // 限制速度范围
    if (speed > 100) speed = 100;
    motor->speed = speed;
    
    // 计算比较值 (0-999对应0-100%)
    compareValue = (uint16_t)((speed / 100.0f) * 999);
    
    // 设置比较值，改变PWM占空比
    switch (motor->TIM_Channel) {
        case TIM_Channel_1:
            motor->TIMx->CCR1 = compareValue;
            break;
        case TIM_Channel_2:
            motor->TIMx->CCR2 = compareValue;
            break;
        case TIM_Channel_3:
            motor->TIMx->CCR3 = compareValue;
            break;
        case TIM_Channel_4:
            motor->TIMx->CCR4 = compareValue;
            break;
        default:
            break;
    }
}

/**
 * @brief  启动电机(保持当前速度和方向)
 * @param  motor: 电机结构体指针
 * @retval 无
 */
void Motor_Start(Motor_HandleTypeDef* motor) {
    Motor_SetDirection(motor, motor->dir);
    Motor_SetSpeed(motor, motor->speed);
}

/**
 * @brief  停止电机
 * @param  motor: 电机结构体指针
 * @retval 无
 */
void Motor_Stop(Motor_HandleTypeDef* motor) {
    GPIO_ResetBits(motor->IN1_GPIOx, motor->IN1_GPIO_Pin);
    GPIO_ResetBits(motor->IN2_GPIOx, motor->IN2_GPIO_Pin);
    //Motor_SetSpeed(motor, 0);
}

/**
 * @brief  切换电机方向
 * @param  motor: 电机结构体指针
 * @retval 无
 */
void Motor_ToggleDirection(Motor_HandleTypeDef* motor) {
    if (motor->dir == MOTOR_FORWARD) {
        Motor_SetDirection(motor, MOTOR_BACKWARD);
    } else {
        Motor_SetDirection(motor, MOTOR_FORWARD);
    }
}