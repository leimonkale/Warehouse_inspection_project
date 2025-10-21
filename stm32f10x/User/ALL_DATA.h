#ifndef _ALL_USER_DATA_H_
#define _ALL_USER_DATA_H_
#include "stm32f10x.h"

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       long long int64_t;

    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       long long uint64_t;

///////////////////////////////////////////////////////////
#define DWT                 ((DWT_Type *) DWT_BASE)
#define DWT_BASE            (0xE0001000UL)
#define DWT_CTRL_CYCCNTENA_Msk (1UL << 0)

#define NULL 0

extern uint32_t turn_l;
extern uint32_t turn_r;

extern volatile uint32_t SysTick_count;
extern volatile uint8_t spl_flag;
extern volatile uint32_t ST_CpuID;

extern char wires;   //����Ѳ�߽ṹ����
extern char moter_com; //�������

// ������ģ��ṹ�嶨��
typedef struct {
    GPIO_TypeDef* TRIG_GPIOx;   // ��������GPIO
    uint16_t TRIG_GPIO_Pin;     // ��������
    GPIO_TypeDef* ECHO_GPIOx;   // ��������GPIO
    uint16_t ECHO_GPIO_Pin;     // ��������
    TIM_TypeDef* TIMx;          // ���ڼ�ʱ�Ķ�ʱ��
    uint32_t timeout_us;        // ��ʱʱ��(΢��)
    float distance;             // ��ǰ����(����)
} HCSR04_HandleTypeDef;

typedef enum {
    HCSR04_IDLE = 0,      // ����
    HCSR04_TRIGGERED,     // �Ѵ������ȴ�������
    HCSR04_WAIT_ECHO,     // �ȴ��½���
    HCSR04_DONE,          // �������
    HCSR04_TIMEOUT        // ��ʱ
} HCSR04_State;

extern HCSR04_HandleTypeDef hcsr04;   //������ʵ��
extern float hcsr04_distance;         //����
extern char hcsr04_flag;              //������ǰ���־
	
extern char engin_flag;               //�����ǰ���־
extern uint16_t angle;                //����Ƕ�


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
extern Motor_HandleTypeDef motor_R, motor_L;   //�������ʵ��


typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float last_error;
} PID_Controller;


extern int humi,temp;    //��ʪ��
extern int adc_num;
#endif

