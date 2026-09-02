/* 
 * File:   OrientationGhost.h
 * Author: E306-PC6
 *
 * Created on 2 septembre 2026, 16:11
 */

#ifndef ORIENTATIONGHOST_H
#define	ORIENTATIONGHOST_H


typedef struct GhostState {
    union {
        struct {
            double Theta;
            double vitesse;
            double accel;
            double vitessemax;
            
            float ThetaGhost;
            float ThetaWayPoint;
            float ThetaRestant;
            float ThetaArret;
            float incrementTheta;
            float VTheta;
            float VThetamax;
            float AccTheta;
            float Tsampling;
            float FQEI = 250.0;
            
                     
        };
    };
} GHOST_STATE;
extern volatile GHOST_STATE ghostState;

void SetupGhostState(volatile GHOST_STATE* GHOST, double Theta, double vitesse, double accel, double vitessemax);

#endif	/* ORIENTATIONGHOST_H */

