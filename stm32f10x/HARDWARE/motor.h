#ifndef __MOTOR_H
#define __MOTOR_H


#include "stm32f10x.h"

// �������ö��
typedef enum {
    MOTOR_FORWARD = 0,  // ��ת
    MOTOR_BACKWARD      // ��ת
} Motor_DirectionTypeDef;

// ����ṹ�嶨��
typedef struct {
    TIM_TypeDef* TIMx;          // ��ʱ��
    uint16_t TIM_Channel;       // ��ʱ��ͨ��
	GPIO_TypeDef* PWM_GPIOx;    // PWM���Ŷ˿�
	uint16_t PWM_GPIO_Pin;      // PWM����
    GPIO_TypeDef* IN1_GPIOx;    // IN1���Ŷ˿�
    uint16_t IN1_GPIO_Pin;      // IN1����
    GPIO_TypeDef* IN2_GPIOx;    // IN2���Ŷ˿�
    uint16_t IN2_GPIO_Pin;      // IN2����
    uint8_t speed;              // ��ǰ�ٶ�(0-100)
    Motor_DirectionTypeDef dir; // ��ǰ����
} Motor_HandleTypeDef;

// ��������
void Motor_Init(Motor_HandleTypeDef* motor);
void Motor_SetDirection(Motor_HandleTypeDef* motor, Motor_DirectionTypeDef dir);
void Motor_SetSpeed(Motor_HandleTypeDef* motor, uint8_t speed);
void Motor_Start(Motor_HandleTypeDef* motor);
void Motor_Stop(Motor_HandleTypeDef* motor);
void Motor_ToggleDirection(Motor_HandleTypeDef* motor);