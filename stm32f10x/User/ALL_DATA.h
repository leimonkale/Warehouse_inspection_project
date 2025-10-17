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

extern char wires;   //储存巡线结构数据
extern char moter_com; //电机命令

// 超声波模块结构体定义
typedef struct {
    GPIO_TypeDef* TRIG_GPIOx;   // 触发引脚GPIO
    uint16_t TRIG_GPIO_Pin;     // 触发引脚
    GPIO_TypeDef* ECHO_GPIOx;   // 回响引脚GPIO
    uint16_t ECHO_GPIO_Pin;     // 回响引脚
    TIM_TypeDef* TIMx;          // 用于计时的定时器
    uint32_t timeout_us;        // 超时时间(微秒)
    float distance;             // 当前距离(厘米)
} HCSR04_HandleTypeDef;
extern HCSR04_HandleTypeDef hcsr04;   //超声波实例
extern float hcsr04_distance;         //距离
extern char hcsr04_flag;              //超声左前后标志
	
extern char engin_flag;               //舵机左前后标志
extern uint16_t angle;                //舵机角度

#endif

