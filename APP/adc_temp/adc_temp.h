#ifndef _adc_temp_H
#define _adc_temp_H

#include "system.h"

void ADC_Temp_Init(void);
void ADC_Light_Init(void);
u16 Get_ADC_Temp_Value(u8 ch,u8 times);
int Get_Temperture(void);
u16 Get_LightIntensity(void);

#endif
