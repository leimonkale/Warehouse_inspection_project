#include "ALL_DATA.h"
#include "ALL_DEFINE.h"
#include "printf.h"
#include "stm32f10x.h"

char engin_flag = 0x00;
uint16_t angle = 90;
/**
 * @brief  ��ʼ�����GPIO����(PA8 - TIM1_CH1)
 * @param  ��
 * @retval ��
 */
void gpio_engin_init(void){
    GPIO_InitTypeDef init;
    // ʹ��GPIOA��AFIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // PA8 ����Ϊ�������������TIM1_CH1��
    init.GPIO_Pin = engin_pwm;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &init);
}

/**
 * @brief  ��ʼ�������ʱ��(TIM1_CH1)
 * @param  ��
 * @retval ��
 */
void tim1_init(void){
    TIM_TimeBaseInitTypeDef BaseInit;
    TIM_OCInitTypeDef OcInit;

    // ʹ��TIM1ʱ�ӣ�APB2���ߣ�
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // ʱ�����ã�20ms���ڣ�50Hz��
    BaseInit.TIM_Prescaler = 71;        // 72MHz/(71+1)=1MHz��1��s������
    BaseInit.TIM_Period = 19999;        // 20000��s = 20ms
    BaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
    BaseInit.TIM_CounterMode = TIM_CounterMode_Up;
    BaseInit.TIM_RepetitionCounter = 0;  // �߼���ʱ������
    TIM_TimeBaseInit(TIM1, &BaseInit);

    // PWM���ã�CH1��- ע�⣺�ṹ����û��TIM_Channel��Ա
    OcInit.TIM_OCMode = TIM_OCMode_PWM1;
    OcInit.TIM_OutputState = TIM_OutputState_Enable;
    OcInit.TIM_OutputNState = TIM_OutputNState_Disable;  // ���û���ͨ��
    OcInit.TIM_Pulse = 1500;            // ��ʼ��λ��1.5ms��
    OcInit.TIM_OCPolarity = TIM_OCPolarity_High;
    OcInit.TIM_OCNPolarity = TIM_OCNPolarity_High;      // ����ͨ�����ԣ�δʹ�ã�
    OcInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
    OcInit.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC1Init(TIM1, &OcInit);         // ͨ��������ָ��ͨ��1

    // ʹ��Ԥװ��
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // �ؼ����߼���ʱ������ʹ�������
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    // ������ʱ��
    TIM_Cmd(TIM1, ENABLE);
}

// ����Ƕȿ��ƺ���
void servo_set_angle(void){
	
	static int flag = 1;
	if(flag)
	{
		if(angle < 180)
			angle+= 3;
		else
			flag = !flag;;
	}else{
		if(angle > 0)
			angle-= 3;
		else
			flag = !flag;
	}
	
    // �Ƕ�ת���壺0��->500��s��180��->2500��s
    uint16_t pulse = 500 + (angle * 2000 / 180);
    TIM_SetCompare1(TIM1, pulse);
	
	
	if(angle == 89)
	{
		engin_flag &= 0x00;
		engin_flag |= ENGIN_IS;
	}
	if(angle == 0)
	{
		engin_flag &= 0x00;
		engin_flag |= ENGIN_RI;
	}
	if(angle == 179)
	{
		engin_flag &= 0x00;
		engin_flag |= ENGIN_LE;
	}
}
