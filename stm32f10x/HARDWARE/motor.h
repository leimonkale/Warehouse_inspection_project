#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stm32f10x.h"

extern Motor_HandleTypeDef motor_R, motor_L;

// º¯ÊýÉùÃ÷
void Motor_Init(Motor_HandleTypeDef* motor);
void Motor_SetDirection(Motor_HandleTypeDef* motor, Motor_DirectionTypeDef dir);
void Motor_SetSpeed(Motor_HandleTypeDef* motor, uint8_t speed);
void Motor_Start(Motor_HandleTypeDef* motor);
void Motor_Stop(Motor_HandleTypeDef* motor);
void Motor_ToggleDirection(Motor_HandleTypeDef* motor);
void Motor_SetSpeedWithDirection(Motor_HandleTypeDef* motor, int8_t speed);

void Motor1_Init(void);
void Motor2_Init(void);

void Motor_ctr_Init(void) ;
void Motor_ctr_LINE(void) ;
void Motor_ctr_LEFT(void) ;
void Motor_ctr_RIGHT(void);
void Motor_ctr_TURNLEFT(void);
void Motor_ctr_TURNRIGHT(void);


#endif
