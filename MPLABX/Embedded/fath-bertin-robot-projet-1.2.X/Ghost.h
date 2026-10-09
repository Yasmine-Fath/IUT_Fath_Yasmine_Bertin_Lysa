/* 
 * File:   OrientationGhost.h
 * Author: E306-PC6
 *
 * Created on 2 septembre 2026, 16:11
 */

#ifndef GHOST_H
#define	GHOST_H

typedef enum {
    GHOST_IDLE = 0,
    GHOST_ROTATION,
    GHOST_DEPLACEMENT_LINEAIRE
} GhostMode;

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

            float GhostRecu; //Theta ghost recu

            //Position du Ghost et WayPoint
            float xGhost;
            float yGhost;
            float xWaypoint;
            float yWaypoint;
            float DistanceGhostWaypoint;
            float AngleAxeWaypoint;


            //Vitesse linéaire du Ghost
            float VGhost;

            GhostMode state;
            float VLin; // vitesse linéaire du ghost (signée)
            float VLinMax;
            float AccLin;
            float DistanceRestante; // équivalent de ThetaRestant
            float DistanceArret; // équivalent de ThetaArret
            float IncrementLin; // équivalent de IncrementTheta
        };
    };
} GSTATE;

extern GSTATE RobotGstate;
void SetupGhostState(float theta, float vitesse, float accel, float vitesseMax);
void GhostComputation();
void CalculDistanceGhostWaypoint(void);
int WaypointDevant(void);
void SetWaypoint(float x, float y);
void GhostComputationLin(void);
float DistanceProjeteeSurAxe(void);
void GhostComputationRot(void);

// 03/09
//void AffichageGHOST(GSTATE gState);
void AffichageGHOST();



#endif	/* GHOST_H */

