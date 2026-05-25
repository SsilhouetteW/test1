#include "adc_temp.h"
#include "SysTick.h"

/*******************************************************************************
* �� �� ��         : ADC_Temp_Init
* ��������		   : ADC_Temp��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ADC_Temp_Init(void)
{	
	ADC_InitTypeDef       ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //��Ƶ����6ʱ��Ϊ72M/6=12MHz
	
	ADC_TempSensorVrefintCmd(ENABLE);//��ADC�ڲ��¶ȴ�����
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ����������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//1��ת���ڹ��������� Ҳ����ֻת����������1 
	ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����
	
	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
	
	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC������ת����������
}

/*******************************************************************************
* �� �� ��         : Get_ADC_Temp_Value
* ��������		   : ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 	
* ��    ��         : ch:ͨ�����
					 times:��ȡ����
* ��    ��         : ͨ��ch��times��ת�����ƽ��ֵ
*******************************************************************************/
u16 Get_ADC_Temp_Value(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��
	for(t=0;t<times;t++)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ��ָ����ADC1������ת����������	
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
		temp_val+=ADC_GetConversionValue(ADC1);
		delay_ms(5);
	}
	return temp_val/times;
} 

/*******************************************************************************
* �� �� ��         : Get_Temperture
* ��������		   : ��ȡ�¶�ֵ 	
* ��    ��         : ��
* ��    ��         : �¶�ֵ(������100��,��λ:��)
*******************************************************************************/
int Get_Temperture(void)
{
	u32 adc_value;
	int temp;
 	double temperture;
	adc_value=Get_ADC_Temp_Value(ADC_Channel_16,10);	//��ȡͨ��16�ڲ��¶ȴ�����ͨ��,10��ȡƽ��
	temperture=(float)adc_value*(3.3/4096);		//��ѹֵ
	temperture=(1.43-temperture)/0.0043+25; //ת��Ϊ�¶�ֵ 
	temp=temperture*100;					//����100��.
	return temp;
}

/*******************************************************************************
* �� �� ��         : ADC_Light_Init
* ��������		   : ADC��ͨ��ʼ������(��ȡ��Ч����)
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void ADC_Light_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

/*******************************************************************************
* �� �� ��         : Get_LightIntensity
* ��������		   : ��ȡ��Ч����ֵ 	
* ��    ��         : ��
* ��    ��         : ��Ч����ֵ(0-4095,ԽСԽ��)
*******************************************************************************/
u16 Get_LightIntensity(void)
{
	u32 adc_value;
	adc_value = Get_ADC_Temp_Value(ADC_Channel_1, 10);
	return (u16)adc_value;
}
