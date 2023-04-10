/*
 * ADC_Private.h
 *
 * Created: 2/25/2023 12:19:35 AM
 *  Author: MiSo
 */ 


#ifndef ADC_PRIVATE_H_
#define ADC_PRIVATE_H_

#include "Type_Definitions.h"

#define ADCSRA (*((volatile U8 *) 0x26))
#define ADMUX  (*((volatile U8 *) 0x27))
									  
#define ADCL   (*((volatile U8 *) 0x24))
#define ADCH   (*((volatile U8 *) 0x25))

#ifndef __ASSEMBLER__
#define ADC   (*((volatile U16 *) 0x24))   // Both Registers together
#endif
#define ADCW  (*((volatile U16 *) 0x24))

#define SFIOR  (*((volatile U8 *) 0x50))

/* ADCSRA */
#define ADEN    7
#define ADSC    6
#define ADATE   5
#define ADIF    4
#define ADIE    3
#define ADPS2   2
#define ADPS1   1
#define ADPS0   0

/* ADMUX */
#define REFS1   7
#define REFS0   6
#define ADLAR   5
#define MUX4    4
#define MUX3    3
#define MUX2    2
#define MUX1    1
#define MUX0    0


#endif /* ADC_PRIVATE_H_ */