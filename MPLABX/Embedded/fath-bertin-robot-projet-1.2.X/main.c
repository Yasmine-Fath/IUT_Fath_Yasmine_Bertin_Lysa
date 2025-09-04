#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"

//3/09
int main (void){
    
    //Initialisation oscillateur
    InitOscillator();
    
    // Configuration des input et output (IO)
    InitIO();
    LED_BLANCHE_1 = 1;  
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 0;
    LED_VERTE_1 = 0;
    
    // Boucle Principale
    while(1)
    {
    }
    
}