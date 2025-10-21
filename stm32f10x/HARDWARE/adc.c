#include "adc.h"
#include "ALL_DATA.h"
#include "ALL_DEFINE.h"


int adc_num = 0;

void ADC_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_GPIOInit();
	
	//adc初始化
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	//设置转换顺序和采样时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9, 1, ADC_SampleTime_55Cycles5);
	
	//重置校准
	ADC_ResetCalibration(ADC1);
	//printf("--------%d---------\r\n", __LINE__);
	//while(ADC_GetResetCalibrationStatus(ADC1)==SET);
	//printf("--------%d---------\r\n", __LINE__);
	//开始校准
	ADC_StartCalibration(ADC1);
	//printf("--------%d---------\r\n", __LINE__);
	while(ADC_GetCalibrationStatus(ADC1) == SET);
	//printf("--------%d---------\r\n", __LINE__);
	ADC_Cmd(ADC1, ENABLE);
	
	//开启软件转换
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void Get_ADCValue(void)
{
	float value = 0;
	//判断是否转换完成
	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET)
	{
		
		value = ADC_GetConversionValue(ADC1);
		value = value * 3300 / 4096;
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	}
	
	adc_num = value;
}
