/*
 * ADC_Implement.c
 *
 * Created: 2/25/2023 12:21:47 AM
 *  Author: MiSo
 */ 

#include "ADC_Private.h"
#include "ADC_Interface.h"

U16 ADC_Global = 0;
/*************************************************************************/
// Fun 1 : ADC Initialization :
/*************************************************************************/
void ADC_Init(VREF_TYPE Vref)
{
	// Set Prescaler
	U8 ADPS = 0;
	switch (DF)
	{
		case 2 :
		ADPS = 0;
		break;
		
		case 4 :
		ADPS = 2;
		break;
		
		case 8 :
		ADPS = 3;
		break;
		
		case 16 :
		ADPS = 4;
		break;
		
		case 32 :
		ADPS = 5;
		break;
		
		case 64 :
		ADPS = 6;
		break;
		
		case 128 :
		default  :
		ADPS = 7;	
	}
	
	ADCSRA = (ADCSRA & Select_PreScaler_Mask) | ADPS;    // Set ADPS Value (Prescaler)
	
	ADMUX = (ADMUX & Vref_Mask) | (Vref << 6);      // Set Vref
	
//	SET_BIT(ADMUX, ADLAR);				// Left adjust ADC result
	
	SET_BIT(ADCSRA, ADIE);				// ADC local Interrupt Enable
	
	SET_BIT(ADCSRA, ADEN);            	// ADC Enable
}

/*************************************************************************/
// Fun 2 : ADC Start Conversion :
/*************************************************************************/
void ADC_Start_Conversion(CHANNEL_TYPE Channel, U8 Trigger_Type)
{
	ADMUX = (ADMUX & Choose_Channel_Mask) | Channel;  // Choose Channel
	
	SET_BIT(ADCSRA, ADSC);             // Start Conversion
	
	// Auto Trigger Enable 4 1 Channel
	if (Trigger_Type == Auto_Trigger)
		SET_BIT(ADCSRA, ADATE);        // Auto Trigger
	else
		CLR_BIT(ADCSRA, ADATE);        // Manual Trigger
}

/*************************************************************************/
// Fun 3 : ADC Read :
/*************************************************************************/
U16 ADC_Read(void)
{
	switch (ADC_Mode)
	{
		case 8 :
		switch ((GET_BIT(ADCSRA, ADLAR)))
		{
			case 1 :
			return ADCH;
			
			case 0 :
			default:
			return (ADC >> 2);
		}
		break;
		
		case 10 :
		default :
		switch ((GET_BIT(ADCSRA, ADLAR)))
		{
			case 1 :
			return (ADC >> 6);
			
			case 0 :
			default:
			return ADC;
		}
	} 
	/*
	
	if (GET_BIT(ADMUX, ADLAR) == 1)
	{
		switch (ADC_Mode)
		{
			case 10 :
			return ADC >> 6; // ADC right-adjusted again.
			
			case 8 : // !! 255 Values !! The rest are neglected !
			default: // Reads 0, 4, 8, 12, 16, ...
			return ADCH;
		}
	}
	else
	{
		switch (ADC_Mode)
		{
			case 8 :
			return ADC >> 2;
			
			case 10 :
			default :
			return ADC;
		}
	}*/
}

double Map (double Value, double Imin, double Imax, double Omin, double Omax)
{
	return ( ( (Value - Imin) * (Omax - Omin) / (Imax - Imin) ) + Omin );
}

double Contrast (double Value, double Min, double Max)
{
	if (Value < Min)
	return Min;
	
	else if (Value > Max)
	return Max;
	
	else
	return Value;
}

/***************************** ISR Functions *****************************/

/*************************************************************************/
// ISR : Interrupt Service Routine for ADC Conversion Complete
/*************************************************************************/
ISR(ADC_Vect)
{
	ADC_Global = ADC_Read();
}