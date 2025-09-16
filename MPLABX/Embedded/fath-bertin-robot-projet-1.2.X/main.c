#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "Robot.h"
#include "ADC.h"

//5/09

unsigned int ADCValue0, ADCValue1, ADCValue2;

int main(void) {

    //Initialisation oscillateur
    InitOscillator();
    InitIO();
    InitTimer23();
    InitTimer1();

    // Configuration des input et output (IO)
    LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;


    //PWM
//    InitPWM();
    //PWMSetSpeed(20.0, MOTEUR_DROIT);
    InitADC1();
    unsigned int * result = ADCGetResult();
    // Boucle Principale
    while (1) {
        if(ADCIsConversionFinished() == 1){
            ADCValue0 = result[0]; 
            ADCValue1 = result[1]; 
            ADCValue2 = result[2]; 
            
        }
     


    }

}