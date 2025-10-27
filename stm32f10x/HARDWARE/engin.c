#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "printf.h"
#include "stm32f10x.h"

char engin_flag = 0x00;
uint16_t angle = 90;
/**
 * @brief  初始化舵机GPIO引脚(PA8 - TIM1_CH1)
 * @param  无
 * @retval 无
 */
void gpio_engin_init(void){
    GPIO_InitTypeDef init;
    // 使能GPIOA和AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // PA8 配置为复用推挽输出（TIM1_CH1）
    init.GPIO_Pin = engin_pwm;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &init);
}

/**
 * @brief  初始化舵机定时器(TIM1_CH1)
 * @param  无
 * @retval 无
 */
void tim1_init(void){
    TIM_TimeBaseInitTypeDef BaseInit;
    TIM_OCInitTypeDef OcInit;

    // 使能TIM1时钟（APB2总线）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // 时基配置：20ms周期（50Hz）
    BaseInit.TIM_Prescaler = 71;        // 72MHz/(71+1)=1MHz（1μs计数）
    BaseInit.TIM_Period = 19999;        // 20000μs = 20ms
    BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
    BaseInit.TIM_CounterMode = TIM_CounterMode_Up;
    BaseInit.TIM_RepetitionCounter = 0;  // 高级定时器特有
    TIM_TimeBaseInit(TIM1, &BaseInit);

    // PWM配置（CH1）- 注意：结构体中没有TIM_Channel成员
    OcInit.TIM_OCMode = TIM_OCMode_PWM1;
    OcInit.TIM_OutputState = TIM_OutputState_Enable;
    OcInit.TIM_OutputNState = TIM_OutputNState_Disable;  // 禁用互补通道
    OcInit.TIM_Pulse = 1500;            // 初始中位（1.5ms）
    OcInit.TIM_OCPolarity = TIM_OCPolarity_High;
    OcInit.TIM_OCNPolarity = TIM_OCNPolarity_High;      // 互补通道极性（未使用）
    OcInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
    OcInit.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &OcInit);         // 通过函数名指定通道1

    // 使能预装载
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // 关键：高级定时器必须使能主输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    // 启动定时器
    TIM_Cmd(TIM1, ENABLE);
}

// 舵机角度控制函数
void servo_set_angle(void)
{
    static uint8_t flag = 1;

    // 平滑角度变化
    if(flag)
    {
        angle += 3;
        if(angle >= 177)
        {
            angle = 180;
            flag = 0;
        }
    }
    else
    {
        angle -= 3;
        if(angle <= 3)
        {
            angle = 0;
            flag = 1;
        }
    }
	//printf("angle: %d\r\n",angle);
    // 精确计算 PWM 脉宽 (0°→500us, 180°→2500us)
    //uint16_t pulse = 500 + (uint16_t)((float)angle * 2000.0f / 180.0f);
    //TIM_SetCompare1(TIM1, pulse);

    // 状态标志更新
    if(angle < 60)       engin_flag = ENGIN_RI;
    else if(angle < 120) engin_flag = ENGIN_IS;
    else                 engin_flag = ENGIN_LE;
}
