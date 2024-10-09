/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/22
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "debug.h"

#include <math.h>

#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a > b ? b : a)

/* Global define */

void ADC1_IRQHandler() __attribute__((interrupt("WCH-Interrupt-fast")));

/* Global Variable */

/*********************************************************************
 * @fn      DRIVE_INIT
 *
 * @brief   Initializes Stepper Motors
 *
 * Stepper 1: PC0 - PC3
 * Stepper 2: PC4, PC6, PC7, PD7
 *
 * @return  none
 */
void DRIVE_INIT(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;

    // Stepper 1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Stepper 2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Initialize Stepper 2
//    for (int i = 0; i < 4; i++) {
//        GPIO_InitStructure.GPIO_Pin = (i == 0 || i == 3 ? GPIO_Pin_4 : GPIO_Pin_5) + i;
//        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
//        GPIO_Init(i < 3 ? GPIOC : GPIOD, &GPIO_InitStructure);
//    }
}

// Set Stepper Pins
void DRIVE_SET(const int a[4], const int b[4]) {
	GPIO_WriteBit(GPIOC, GPIO_Pin_0, a[1] ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_1, a[0] ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_2, a[2] ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_3, a[3] ? Bit_SET : Bit_RESET);

	GPIO_WriteBit(GPIOC, GPIO_Pin_4, b[0] ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_6, b[1] ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOC, GPIO_Pin_7, b[3] ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(GPIOD, GPIO_Pin_5, b[2] ? Bit_SET : Bit_RESET);
}

void ADCConfig(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 3;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC1, 3);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_241Cycles);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_241Cycles);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_241Cycles);
    ADC_ExternalTrigInjectedConvCmd(ADC1, DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_Calibration_Vol(ADC1, ADC_CALVOL_50PERCENT);
    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);

    while (ADC_GetResetCalibrationStatus(ADC1))
        ;

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;

}

int adcFlag = 0;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    // Init Motors
    DRIVE_INIT();

    // Init Sensors
    ADCConfig();

	float val1 = 0, val2 = 0, val3 = 0;
    while(1)
    {
        ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);

    	if(adcFlag == 1) {
    	    val1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3);
    	    val2 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
    	    val3 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
    	    adcFlag = 0;
    	}

    	int left, right;

    	if (MIN(val1, val3) < val2 * 1.2) {
    		if (val1 < val3)
    			left = 1, right = -1;
    		else
    			left = -1, right = 1;
    	} else {
    		left = 1, right = 1;
    	}

    	const int n[4] = {0, 0, 0, 0}, a[4] = {1, 0, 0, 1}, b[4] = {1, 1, 0, 0}, c[4] = {0, 1, 1, 0}, d[4] = {0, 0, 1, 1};

    	Delay_Ms(4);
		DRIVE_SET(left < 0 ? d : (left > 0 ? a : n),
				  right < 0 ? d : (right > 0 ? a : n));

		Delay_Ms(4);
		DRIVE_SET(left < 0 ? c : (left > 0 ? b : n),
				 right < 0 ? c : (right > 0 ? b : n));

		Delay_Ms(4);
		DRIVE_SET(left < 0 ? b : (left > 0 ? c : n),
				 right < 0 ? b : (right > 0 ? c : n));

		Delay_Ms(4);
		DRIVE_SET(left < 0 ? a : (left > 0 ? d : n),
				  right < 0 ? a : (right > 0 ? d : n));
    }
}


void ADC1_IRQHandler()
{

    if (ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET)
    {
        adcFlag = 1;

        ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);

    }

}
