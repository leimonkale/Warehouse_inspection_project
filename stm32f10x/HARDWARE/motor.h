#ifndef __MOTOR_H
#define __MOTOR_H


#include "stm32f10x.h"

// 电机方向枚举
typedef enum {
    MOTOR_FORWARD = 0,  // 正转
    MOTOR_BACKWARD      // 反转
} Motor_DirectionTypeDef;

// 电机结构体定义
typedef struct {
    TIM_TypeDef* TIMx;          // 定时器
    uint16_t TIM_Channel;       // 定时器通道
	GPIO_TypeDef* PWM_GPIOx;    // PWM引脚端口
	uint16_t PWM_GPIO_Pin;      // PWM引脚
    GPIO_TypeDef* IN1_GPIOx;    // IN1引脚端口
    uint16_t IN1_GPIO_Pin;      // IN1引脚
    GPIO_TypeDef* IN2_GPIOx;    // IN2引脚端口
    uint16_t IN2_GPIO_Pin;      // IN2引脚
    uint8_t speed;              // 当前速度(0-100)
    Motor_DirectionTypeDef dir; // 当前方向
} Motor_HandleTypeDef;

// 函数声明
void Motor_Init(Motor_HandleTypeDef* motor);
void Motor_SetDirection(Motor_HandleTypeDef* motor, Motor_DirectionTypeDef dir);
void Motor_SetSpeed(Motor_HandleTypeDef* motor, uint8_t speed);
void Motor_Start(Motor_HandleTypeDef* motor);
void Motor_Stop(Motor_HandleTypeDef* motor);
void Motor_ToggleDirection(Motor_HandleTypeDef* motor);