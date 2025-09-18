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

int N = 2296; // valeur ADC pour une distance de 30cm

int main(void) {

    //Initialisation oscillateur
    InitOscillator();
    InitIO();
    InitTimer23();
    InitTimer1();

//    // Configuration des input et output (IO)
//    LED_BLANCHE_1 = 1;
//    LED_BLEUE_1 = 1;
//    LED_ORANGE_1 = 1;
//    LED_ROUGE_1 = 1;
//    LED_VERTE_1 = 1;
//
//    LED_BLANCHE_2 = 1;
//    LED_BLEUE_2 = 1;
//    LED_ORANGE_2 = 1;
//    LED_ROUGE_2 = 1;
//    LED_VERTE_2 = 1;


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
            
//            if (ADCValue0 < N){
//                LED_BLEUE_1 = 1;
//            }
//            else{
//                LED_BLEUE_1 = 0;
//            }
//            if (ADCValue1 < N){
//                LED_ORANGE_1 = 1;
//            }
//            else{
//                LED_ORANGE_1 = 0;
//            }
//            if (ADCValue2 < N){
//                LED_ROUGE_1 = 1;
//            }
//            else{
//                LED_ROUGE_1 = 0;
//            }
            
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            
        }
     


    }

}