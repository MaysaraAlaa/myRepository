/*
 * ADC_Interface.h
 *
 * Created: 2/25/2023 12:20:07 AM
 *  Author: MiSo
 */ 


#ifndef ADC_INTERFACE_H_
#define ADC_INTERFACE_H_

#include "Type_Definitions.h"
#include "BitMath.h"
#include "Interrupt.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif /* F_CPU */

#define F_ADC 125000UL

#define DF ((F_CPU)/(F_ADC))

#define ADC_Mode                    10

#define Manual_Trigger               0
#define Auto_Trigger                 1

#define Free_Running_mode            0
#define Analog_Comparator            1
#define External_Interrupt_Request0  2
#define TimerCounter0_Compare_Match  3
#define TimerCounter0_Overflow       4
#define TimerCounter1_Compare_MatchB 5
#define TimerCounter1_Overflow       6
#define TimerCounter1_Capture_Event  7

#define Select_PreScaler_Mask     0xF8
#define             Vref_Mask     0x3F
#define   Choose_Channel_Mask     0xE0

typedef enum {Internal_Vref, AVCC, Reserved, AREF} VREF_TYPE;
typedef enum {A0, A1, A2, A3, A4, A5, A6, A7} CHANNEL_TYPE;
	
// __________________ ADC Functions ___________________ //
/* 1 */ void ADC_Init(VREF_TYPE);
/* 2 */ void ADC_Start_Conversion(CHANNEL_TYPE, U8);
/* 3 */  U16 ADC_Read(void);
// ================= Helpful Functions ================ //
/* 1 */ double Map(double Value, double Imin, double Imax, double Omin, double Omax);
/* 2 */ double Contrast (double Value, double Min, double Max);

#endif /* ADC_INTERFACE_H_ */