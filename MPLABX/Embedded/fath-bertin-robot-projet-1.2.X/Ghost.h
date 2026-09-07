/* 
 * File:   OrientationGhost.h
 * Author: E306-PC6
 *
 * Created on 2 septembre 2026, 16:11
 */

#ifndef GHOST_H
#define	GHOST_H


typedef struct GState {
    union {
        struct {
            float ThetaGhost;
            float ThetaWayPoint;
            float ThetaRestant;
            float ThetaArret;
            float IncrementTheta;
            float VTheta;
            float VThetamax;
            float AccTheta;
            float Tsampling;
            float FQEI;
            
            float GhostRecu;    //Theta ghost recu
            
                     
        };
    };
} GSTATE;

extern volatile GSTATE ghostState;
void SetupGhostState(GSTATE* gState, float theta, float vitesse, float accel, float vitesseMax, float waypoint);
void GhostComputation(GSTATE gState);

// 03/09
void AffichageGHOST(GSTATE gState);


#endif	/* GHOST_H */

