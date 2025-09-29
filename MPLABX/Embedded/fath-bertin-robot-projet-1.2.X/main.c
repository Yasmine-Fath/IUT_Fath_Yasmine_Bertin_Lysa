#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "Robot.h"
#include "ADC.h"
#include "main.h"

//5/09

unsigned int ADCValue0, ADCValue1, ADCValue2, ADCValue3, ADCValue4;

int N = 0x667; // valeur ADC pour une distance de 20cm
//pour 40cm : 0x02E6

int main(void) {

    //Initialisation oscillateur
    InitOscillator();
    InitIO();
    InitTimer23();
    InitTimer1();
    InitTimer4();
    InitPWM();
    InitADC1();

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


    // Boucle Principale
    while (1) {
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();

            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGaucheGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreDroitDroit = 34 / volts - 5;


            if (robotState.distanceTelemetreGaucheGauche > 30) {
                LED_BLANCHE_1 = 0;
            } else {
                LED_BLANCHE_1 = 1;
            }
            if (robotState.distanceTelemetreGauche > 30) {
                LED_BLEUE_1 = 0;
            } else {
                LED_BLEUE_1 = 1;
            }
            if (robotState.distanceTelemetreCentre > 30) {
                LED_ORANGE_1 = 0;
            } else {
                LED_ORANGE_1 = 1;
            }
            if (robotState.distanceTelemetreDroit > 30) {
                LED_ROUGE_1 = 0;
            } else {
                LED_ROUGE_1 = 1;
            }
            if (robotState.distanceTelemetreDroitDroit > 30) {
                LED_VERTE_1 = 0;
            } else {
                LED_VERTE_1 = 1;
            }
        }
    }
}

unsigned char stateRobot;

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000) {
                stateRobot = STATE_AVANCE;
            }
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(30, MOTEUR_DROIT);
            PWMSetSpeedConsigne(30, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;

        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}


unsigned char nextStateRobot = 0;
unsigned int flagrobot;

void SetNextRobotStateInAutomaticMode() {
//    unsigned char positionObstacle = PAS_D_OBSTACLE;
//    //ÈDtermination de la position des obstacles en fonction des ÈÈËtlmtres
//    if (robotState.distanceTelemetreDroit < 30 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche > 30) { //Obstacle ‡droite
//        positionObstacle = OBSTACLE_A_DROITE;
//    } else if (robotState.distanceTelemetreDroit > 30 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche < 30) { //Obstacle ‡gauche
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    } else if (robotState.distanceTelemetreCentre < 20) { //Obstacle en face
//        positionObstacle = OBSTACLE_EN_FACE;
//    } else if (robotState.distanceTelemetreDroit > 30 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGauche > 30) { //pas d?obstacle
//        positionObstacle = PAS_D_OBSTACLE;
//    }   
//    else if (robotState.distanceTelemetreDroitDroit > 30 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreGaucheGauche < 15) { //Obstacle ‡gauche
//        positionObstacle = OBSTACLE_A_GAUCHE_GAUCHE;
//    } 
//    else if (robotState.distanceTelemetreGaucheGauche > 30 &&
//            robotState.distanceTelemetreCentre > 20 &&
//            robotState.distanceTelemetreDroitDroit < 15) { //Obstacle ‡gauche
//        positionObstacle = OBSTACLE_A_DROITE_DROITE;
//        
//    } 
    
      
            
//   
//    //ÈDtermination de lÈ?tat ‡venir du robot
//    if (positionObstacle == PAS_D_OBSTACLE) {
//        nextStateRobot = STATE_AVANCE;
//    } else if (positionObstacle == OBSTACLE_A_DROITE) {
//        nextStateRobot = STATE_TOURNE_GAUCHE;
//    } else if (positionObstacle == OBSTACLE_A_GAUCHE) {
//        nextStateRobot = STATE_TOURNE_DROITE;
//    } else if (positionObstacle == OBSTACLE_EN_FACE) {
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
//    }
//    else if (positionObstacle == OBSTACLE_A_GAUCHE_GAUCHE) {
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
//    }
//    else if (positionObstacle == OBSTACLE_A_DROITE_DROITE) {
//        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
//    }
    
    flagrobot = 0b00000;
    if(robotState.distanceTelemetreGaucheGauche < 25){
        flagrobot |= 0b00001;  
    }
    
    if(robotState.distanceTelemetreGauche< 30){
        flagrobot |= 0b00010 ;
        //flagrobot != 1<<3;  
    }
    if(robotState.distanceTelemetreCentre < 35){
        flagrobot |= 0b00100 ;
       // flagrobot != 1<<2;  
    }
    if(robotState.distanceTelemetreDroit < 30){
        flagrobot |= 0b01000 ;
        //flagrobot != 1<<1;  
    }
    if(robotState.distanceTelemetreDroitDroit < 25){
        flagrobot |= 0b10000;  
    }
    
    
    switch (flagrobot) {
        case 0b00000:
            nextStateRobot = STATE_AVANCE;
            break;
            
        case 0b00001:
            nextStateRobot = STATE_AVANCE;
            break;
            
        case 0b00010:
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
            
        case 0b00011:
            nextStateRobot = STATE_TOURNE_GAUCHE;
            break;
            
        case 0b00100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b00101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b00110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b00111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01000:
            nextStateRobot = STATE_TOURNE_DROITE;
            break;
            
        case 0b01001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b01101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b01111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b10000:
            nextStateRobot = STATE_AVANCE;
            break;
            
        case 0b10001:
            nextStateRobot = STATE_AVANCE;
            break;
            
        case 0b10010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b10011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b10100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b10101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b10110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b10111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b11000:
            nextStateRobot = STATE_TOURNE_DROITE;
            break;
            
        case 0b11001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b11010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b11011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
        case 0b11100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b11101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b11110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
            
        case 0b11111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
            
            
        
    }
    //Si l?on n?est pas dans la transition de lÈ?tape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
    }
}

