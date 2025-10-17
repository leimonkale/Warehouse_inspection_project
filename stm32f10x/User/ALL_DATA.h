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


#define NULL 0
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
extern HCSR04_HandleTypeDef hcsr04;   //������ʵ��
extern float hcsr04_distance;         //����
extern char hcsr04_flag;              //������ǰ���־
	
extern char engin_flag;               //�����ǰ���־
extern uint16_t angle;                //����Ƕ�

#endif

