/* 
 * File:   ADC.h
 * Author: E306-PC6
 *
 * Created on 16 septembre 2025, 09:49
 */

#ifndef ADC_H
#define	ADC_H

void InitADC1(void);
void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void);
void ADC1StartConversionSequence(void);
unsigned int * ADCGetResult(void);
unsigned char ADCIsConversionFinished(void);
void ADCClearConversionFinishedFlag(void);
             
#endif	/* ADC_H */

