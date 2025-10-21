#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "printf.h"
#include "stm32f10x.h"

Motor_HandleTypeDef motor_R, motor_L;

/**
 * @brief  ��ʼ���������
 * @param  motor: ����ṹ��ָ��
 * @retval ��
 */
void Motor_Init(Motor_HandleTypeDef* motor) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    
    // ʹ��GPIOʱ��
    if (motor->IN1_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (motor->IN1_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (motor->IN1_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    if (motor->IN2_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (motor->IN2_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (motor->IN2_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	if (motor->PWM_GPIOx == GPIOA) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (motor->PWM_GPIOx == GPIOB) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (motor->PWM_GPIOx == GPIOC) RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // ʹ�ܶ�ʱ��ʱ��
    if (motor->TIMx == TIM1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    else if (motor->TIMx == TIM2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (motor->TIMx == TIM3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else if (motor->TIMx == TIM4) RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // ���÷����������
    GPIO_InitStructure.GPIO_Pin = motor->IN1_GPIO_Pin | motor->IN2_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor->IN1_GPIOx, &GPIO_InitStructure);  // IN1��IN2��ͬһ��GPIO��
	
	GPIO_InitStructure.GPIO_Pin = motor->PWM_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // ���������������ʱ�����ƣ�
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(motor->PWM_GPIOx, &GPIO_InitStructure);
    
    // Ĭ��ֹͣ״̬
    GPIO_ResetBits(motor->IN1_GPIOx, motor->IN1_GPIO_Pin);
    GPIO_ResetBits(motor->IN2_GPIOx, motor->IN2_GPIO_Pin);
    
    // ���ö�ʱ������PWM���
    // ����Ϊ10kHz PWM���ʺϴ����ֱ�����
    TIM_TimeBaseStructure.TIM_Period = 999;  // ���� = (999 + 1) / 72MHz * 1000 = ~13.89us��Ƶ��Լ72kHz
    TIM_TimeBaseStructure.TIM_Prescaler = 71;  // ��Ƶ��72MHz/(71+1)=1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(motor->TIMx, &TIM_TimeBaseStructure);
    
    // ����PWM���ͨ��
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;  // ��ʼռ�ձ�0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    // ����ͨ������
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
    
    // ʹ�ܶ�ʱ��Ԥ���ؼĴ���
    TIM_ARRPreloadConfig(motor->TIMx, ENABLE);
    
    // ������ʱ��
    TIM_Cmd(motor->TIMx, ENABLE);
    
    // �߼���ʱ������Ҫʹ�������
    if (motor->TIMx == TIM1) {
        TIM_CtrlPWMOutputs(motor->TIMx, ENABLE);
    }
    
    // ��ʼ���������
    motor->speed = 0;
    motor->dir = MOTOR_FORWARD;
}

/**
 * @brief  ���õ������
 * @param  motor: ����ṹ��ָ��
 * @param  dir: ����(MOTOR_FORWARD��MOTOR_BACKWARD)
 * @retval ��
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
 * @brief  ���õ���ٶ�
 * @param  motor: ����ṹ��ָ��
 * @param  speed: �ٶ�ֵ(0-100)
 * @retval ��
 */
void Motor_SetSpeed(Motor_HandleTypeDef* motor, uint8_t speed) {
    uint16_t compareValue;
    
    // �����ٶȷ�Χ
    if (speed > 100) speed = 100;
    motor->speed = speed;
    
    // ����Ƚ�ֵ (0-999��Ӧ0-100%)
    compareValue = (uint16_t)((speed / 100.0f) * 999);
    
    // ���ñȽ�ֵ���ı�PWMռ�ձ�
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
 * @brief  �������(���ֵ�ǰ�ٶȺͷ���)
 * @param  motor: ����ṹ��ָ��
 * @retval ��
 */
void Motor_Start(Motor_HandleTypeDef* motor) {
    Motor_SetDirection(motor, motor->dir);
    Motor_SetSpeed(motor, motor->speed);
}

/**
 * @brief  ֹͣ���
 * @param  motor: ����ṹ��ָ��
 * @retval ��
 */
void Motor_Stop(Motor_HandleTypeDef* motor) {
    GPIO_ResetBits(motor->IN1_GPIOx, motor->IN1_GPIO_Pin);
    GPIO_ResetBits(motor->IN2_GPIOx, motor->IN2_GPIO_Pin);
    //Motor_SetSpeed(motor, 0);
}

/**
 * @brief  �л��������
 * @param  motor: ����ṹ��ָ��
 * @retval ��
 */
void Motor_ToggleDirection(Motor_HandleTypeDef* motor) {
    if (motor->dir == MOTOR_FORWARD) {
        Motor_SetDirection(motor, MOTOR_BACKWARD);
    } else {
        Motor_SetDirection(motor, MOTOR_FORWARD);
    }
}

/**
 * @brief  ���õ���ٶȣ���������ƣ�
 * @param  motor: ����ṹ��ָ��
 * @param  speed: �ٶ�ֵ(-100��100����ֵ��ʾ��ת)
 * @retval ��
 */
void Motor_SetSpeedWithDirection(Motor_HandleTypeDef* motor, int8_t speed) {
    // �����ٶȷ�Χ
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;
    
    // ����ֹͣ״̬
    if (speed == 0) {
        Motor_Stop(motor);
        motor->speed = 0;
        return;
    }
    
    // ���÷���;���ֵ�ٶ�
    if (speed > 0) {
        Motor_SetDirection(motor, MOTOR_FORWARD);
        motor->speed = (uint8_t)speed;
    } else {
        Motor_SetDirection(motor, MOTOR_BACKWARD);
        motor->speed = (uint8_t)(-speed); // ȡ����ֵ
    }
    
    // ����Ƚ�ֵ (0-999��Ӧ0-100%)
    uint16_t compareValue = (uint16_t)((motor->speed / 100.0f) * 999);
    
    // ���ñȽ�ֵ���ı�PWMռ�ձ�
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
 * @brief  ��ʼ�����1�ṹ��
 */
void Motor1_Init(void) {
    motor_R.TIMx = TIM3;
    motor_R.TIM_Channel = TIM_Channel_1;
	motor_R.PWM_GPIOx = GPIOA;
	motor_R.PWM_GPIO_Pin = GPIO_Pin_6;
    motor_R.IN1_GPIOx = GPIOB;
    motor_R.IN1_GPIO_Pin = GPIO_Pin_6;
    motor_R.IN2_GPIOx = GPIOB;
    motor_R.IN2_GPIO_Pin = GPIO_Pin_7;
	motor_R.dir = MOTOR_FORWARD;
	motor_R.speed = MOTOR_R;
    Motor_Init(&motor_R);
}

/**
 * @brief  ��ʼ�����2�ṹ��
 * @param  motor: ����ṹ��ָ��
 * @retval ��
 */
void Motor2_Init(void) {
    motor_L.TIMx = TIM3;
    motor_L.TIM_Channel = TIM_Channel_2;
	motor_L.PWM_GPIOx = GPIOA;
	motor_L.PWM_GPIO_Pin = GPIO_Pin_7;
    motor_L.IN1_GPIOx = GPIOB;
    motor_L.IN1_GPIO_Pin = GPIO_Pin_8;
    motor_L.IN2_GPIOx = GPIOB;
    motor_L.IN2_GPIO_Pin = GPIO_Pin_9;
	motor_L.dir = MOTOR_FORWARD;
	motor_L.speed = MOTOR_L;
    Motor_Init(&motor_L);
}

void Motor_ctr_Init(void)            //���������ʼ��
{
	motor_R.dir = MOTOR_FORWARD;
	motor_L.dir = MOTOR_FORWARD;
}

void Motor_ctr_LINE(void)             //�����ֱ��
{
	Motor_SetSpeedWithDirection(&motor_R,MOTOR_R);       //����
	Motor_SetSpeedWithDirection(&motor_L,MOTOR_L);       //����
	Motor_Start(&motor_R);
	Motor_Start(&motor_L);
}

void Motor_ctr_LEFT(void)             //�����ƫ
{
	Motor_SetSpeedWithDirection(&motor_R,MOTOR_R);
	Motor_SetSpeedWithDirection(&motor_L,MOTOR_L-20);
	Motor_Start(&motor_R);
	Motor_Start(&motor_L);
}

void Motor_ctr_RIGHT(void)            //�����ƫ
{
	Motor_SetSpeedWithDirection(&motor_R,MOTOR_R-20);
	Motor_SetSpeedWithDirection(&motor_L,MOTOR_L);
	Motor_Start(&motor_R);
	Motor_Start(&motor_L);
}

// �޸�ת����ƺ���
void Motor_ctr_TURNLEFT(void) {
    // ����ʽת�򣬱���һ���ٶȲ�
    Motor_SetSpeedWithDirection(&motor_R, MOTOR_R);
    Motor_SetSpeedWithDirection(&motor_L, MOTOR_L * 0.1); // ���ּ��ٵ�30%
    Motor_Start(&motor_R);
    Motor_Start(&motor_L);
}

void Motor_ctr_TURNRIGHT(void) {
    Motor_SetSpeedWithDirection(&motor_R, MOTOR_R * 0.1); // ���ּ��ٵ�30%
    Motor_SetSpeedWithDirection(&motor_L, MOTOR_L);
    Motor_Start(&motor_R);
    Motor_Start(&motor_L);
}

