#include "Ghost.h"
#include "QEI.h"
#include "Robot.h"
#include "math.h"
#include "Utilities.h"
#include "main.h"
#include "UART_Protocol.h"
#include "timer.h"

GSTATE RobotGstate;


//void SetupGhostState(GSTATE* RobotGstate, float theta, float vitesse, float accel, float vitesseMax){

void SetupGhostState(float theta, float vitesse, float accel, float vitesseMax) {

    RobotGstate.ThetaGhost = theta;
    RobotGstate.VTheta = vitesse;
    RobotGstate.AccTheta = accel;
    RobotGstate.VThetamax = vitesseMax;
    RobotGstate.FQEI = 10;
    RobotGstate.Tsampling = 1.0f / RobotGstate.FQEI;
    RobotGstate.ThetaWayPoint = 0;
    // Position initiale du Ghost
    RobotGstate.xGhost = 0.0f;
    RobotGstate.yGhost = 0.0f;
    // Vitesse linéaire du Ghost
    RobotGstate.VGhost = 0.2f;

}

void GhostComputation() {

    RobotGstate.ThetaRestant = ModuloByAngle(RobotGstate.ThetaGhost, RobotGstate.ThetaWayPoint) - RobotGstate.ThetaGhost;

    RobotGstate.ThetaArret = (RobotGstate.VTheta * RobotGstate.VTheta) / (2 * RobotGstate.AccTheta);
    RobotGstate.IncrementTheta = RobotGstate.VTheta * RobotGstate.Tsampling;


    if (RobotGstate.VTheta < 0) {
        RobotGstate.ThetaArret = -RobotGstate.ThetaArret;

    }

    if (((RobotGstate.ThetaArret >= 0 && RobotGstate.ThetaRestant >= 0) || (RobotGstate.ThetaArret <= 0 && RobotGstate.ThetaRestant <= 0)) && Abs(RobotGstate.ThetaRestant) >= RobotGstate.ThetaArret) {
        // On accélère en rampe saturée
        if (RobotGstate.ThetaRestant > 0) {
            // Si la destination est devant, on accélère en positif en saturant la vitesse à V?Max
            RobotGstate.VTheta = Min(RobotGstate.VTheta + RobotGstate.AccTheta / RobotGstate.FQEI, RobotGstate.VThetamax);
        } else if (RobotGstate.ThetaRestant < 0) {
            //Si la destination est derrière, on accélère en négatif en saturant la vitesse à ?V?Max
            RobotGstate.VTheta = Max(RobotGstate.VTheta - RobotGstate.AccTheta / RobotGstate.FQEI, -RobotGstate.VThetamax);

        }
    } else {
        // On freine en rampe saturée
        if (RobotGstate.VTheta > 0) {
            //Si la vitesse positive est positive, on freine en positif en saturant la vitesse à 0
            RobotGstate.VTheta = Min(RobotGstate.VTheta - RobotGstate.AccTheta / RobotGstate.FQEI, 0);
        } else if (RobotGstate.VTheta < 0) {
            //Si la vitesse est négative, on freine en négatif en saturant la vitesse à 0
            RobotGstate.VTheta = Max(RobotGstate.VTheta + RobotGstate.AccTheta / RobotGstate.FQEI, 0);
        }

        if (Abs(RobotGstate.ThetaRestant) < Abs(RobotGstate.IncrementTheta)) {
            RobotGstate.IncrementTheta = RobotGstate.ThetaRestant;
        }
    }

    // On intègre le déplacement
    RobotGstate.ThetaGhost = RobotGstate.ThetaGhost + RobotGstate.IncrementTheta;

    // ============================================================
    // CALCUL DE LA POSITION DU GHOST
    // ============================================================

    RobotGstate.xGhost += RobotGstate.VGhost * cosf(RobotGstate.ThetaGhost) * RobotGstate.Tsampling;
    RobotGstate.yGhost += RobotGstate.VGhost * sinf(RobotGstate.ThetaGhost) * RobotGstate.Tsampling;

    // ============================================================
    // CALCUL DE LA DISTANCE DU WAYPOINT AU SEGMENT AB
    // ============================================================
    CalculDistanceGhostWaypoint();


    // On gère les erreurs numériques d?arrondis
    if (RobotGstate.VTheta == 0 && Abs(RobotGstate.ThetaRestant) < 0.01) {
        RobotGstate.ThetaGhost = RobotGstate.ThetaWayPoint;

    }

}



// 03/09
/// AFFICHAGE DU GHOST

//void AffichageGHOST(GSTATE RobotGstate){

void AffichageGHOST() {
    unsigned char payload[28];

    //*****************Theta Ghost ****************
    getBytesFromFloat(payload, 0, RobotGstate.xGhost);
    getBytesFromFloat(payload, 4, RobotGstate.yGhost);
    getBytesFromFloat(payload, 8, RobotGstate.ThetaGhost);
    getBytesFromFloat(payload, 12, RobotGstate.xWaypoint);
    getBytesFromFloat(payload, 16, RobotGstate.yWaypoint);
    getBytesFromFloat(payload, 20, RobotGstate.ThetaWayPoint);
    getBytesFromFloat(payload, 24, RobotGstate.DistanceGhostWaypoint);
    UartEncodeAndSendMessage(0x0090, 28, payload);

}

void CalculDistanceGhostWaypoint(void) {
    Point A, B, P;
    double distance = 0.0;

    A.x = RobotGstate.xGhost;
    A.y = RobotGstate.yGhost;

    P.x = RobotGstate.xWaypoint;
    P.y = RobotGstate.yWaypoint;

    // B = point situé à 1 m devant le Ghost
    B.x = A.x + cosf(RobotGstate.ThetaGhost);
    B.y = A.y + sinf(RobotGstate.ThetaGhost);

    /*
     * Détermination du cas
     */
    int cas = 0;

    if (A.x == P.x && A.y == P.y)
        cas = 1; // A = P
    else if (P.x == B.x && P.y == B.y)
        cas = 2; // P = B
    else
        cas = 3; // Cas général

    switch (cas) {
        case 1:
            // A = P
            distance = 0.0;
            break;

        case 2:
            // P = B
            distance = 0.0;
            break;

        case 3:
            // Cas général :
            // distance entre P et la droite AB
            distance = distancePointDroite(P, A, B);
            break;

        default:
            distance = 0.0;
            break;
    }

    RobotGstate.DistanceGhostWaypoint = distance;
}
