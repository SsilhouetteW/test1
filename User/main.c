#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "adc_temp.h"
#include <string.h>
#include <stdio.h>


void (*pNet_Test)(void);

int main()
{
	int temperature;
	u16 light;
	char temp_str[32];
	char light_str[16];
	u32 loop_counter = 0;
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART1_Init(115200);
	LED_Init();
	ADC_Temp_Init();
	ADC_Light_Init();
	WiFi_Config();
	
	printf("\r\n========================================\r\n");
	printf("   STM32 MQTT -> Phone Display Demo\r\n");
	printf("========================================\r\n");
	printf("\r\nInitializing ESP8266...\r\n");
    
	ESP8266_STA_TCP_Client_MQTT();
	
	printf("\r\n[OK] MQTT connected!\r\n");
	
	printf("\r\n----------------------------------------\r\n");
	printf("  STM32 -> Phone MQTT Demo\r\n");
	printf("----------------------------------------\r\n");
	printf("  Publish: phone/temperature\r\n");
	printf("  Broker: broker.emqx.io:1883\r\n");
	printf("----------------------------------------\r\n");
   
	while(1){
		temperature = Get_Temperture();
		light = Get_LightIntensity();
		
		sprintf(temp_str, "%d.%02d", temperature/100, temperature%100);
		sprintf(light_str, "%u", light);
		
		printf("\r\n[T%04lu] TEMP: %s C  |  LIGHT: %s\r\n", 
		       loop_counter, temp_str, light_str);
		
		// Publish temperature to phone topic
		if(ESP8266_Set_MQTT_Public("phone/temperature", temp_str))
		{
			printf("[MQTT] Published to phone successfully!\r\n");
		}
		
		LED1 = !LED1;
		loop_counter++;
		
		delay_ms(5000);
	};
}
