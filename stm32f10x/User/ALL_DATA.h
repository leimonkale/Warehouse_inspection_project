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

typedef enum {
    HCSR04_IDLE = 0,      // 空闲
    HCSR04_TRIGGERED,     // 已触发，等待上升沿
    HCSR04_WAIT_ECHO,     // 等待下降沿
    HCSR04_DONE,          // 测量完成
    HCSR04_TIMEOUT        // 超时
} HCSR04_State;

extern HCSR04_HandleTypeDef hcsr04;   //超声波实例
extern float hcsr04_distance;         //距离
extern char hcsr04_flag;              //超声左前后标志
	
extern char engin_flag;               //舵机左前后标志
extern uint16_t angle;                //舵机角度


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
extern Motor_HandleTypeDef motor_R, motor_L;   //两个电机实体


typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float last_error;
} PID_Controller;


extern int humi,temp;    //温湿度
extern int adc_num;
#endif

