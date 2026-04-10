#ifndef ROBOT_H
#define ROBOT_H

#include "asservissement.h"

typedef struct robotStateBITS {
    union {
        struct {
            unsigned char taskEnCours;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;
            float distanceTelemetreDroitDroit;
            float distanceTelemetreDroit;
            float distanceTelemetreCentre;
            float distanceTelemetreGauche;
            float distanceTelemetreGaucheGauche;
            unsigned char Mode;
            double vitesseDroitFromOdometry;
            double vitesseGaucheFromOdometry;
            double vitesseLineaireFromOdometry;
            double vitesseAngulaireFromOdometry;
            double xPosFromOdometry;
            double yPosFromOdometry;
            double xPosFromOdometry_1;
            double yPosFromOdometry_1;
            double angleRadianFromOdometry;
            double angleRadianFromOdometry_1;
            
            // Asservissement polaire
            PidCorrector PidX;      // PID pour vitesse linéaire (VL)
            PidCorrector PidTheta;  // PID pour vitesse angulaire (VA)            
        };
    };
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;



#endif /* ROBOT_H */
