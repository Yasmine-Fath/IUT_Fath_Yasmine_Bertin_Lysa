#include "Ghost.h"
#include "QEI.h"
#include "Robot.h"
#include "math.h"
#include "Utilities.h"
#include "main.h"
#include "UART_Protocol.h"
#include "timer.h"


#define SEUIL_ARRIVEE 0.002f   // 2 mm
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
    RobotGstate.VGhost = 0.0f;
    RobotGstate.state = GHOST_ROTATION;
    RobotGstate.VLin = 0.0f;
    RobotGstate.VLinMax = 0.5f; // m/s
    RobotGstate.AccLin = 0.5f; // m/s²
    RobotGstate.DistanceRestante = 0.0f;

}

void GhostComputation() {

    if (RobotGstate.state == GHOST_IDLE) {
        RobotGstate.VTheta = 0;
        RobotGstate.VLin = 0;
        return;
    }

    if (RobotGstate.state == GHOST_DEPLACEMENT_LINEAIRE) {
        GhostComputationLin();
        return;
    }

    if (RobotGstate.state == GHOST_ROTATION) {
        GhostComputationRot();
        return;
    }
}
//    09/10 matin

void GhostComputationLin(void) {

    float c = cosf(RobotGstate.ThetaGhost);
    float s = sinf(RobotGstate.ThetaGhost);

    // Devant ou derrière + distance restante signée (joue le rôle de ThetaRestant)
    int devant = WaypointDevant();
    float dist = Abs(DistanceProjeteeSurAxe());
    RobotGstate.DistanceRestante = devant ? dist : -dist;

    // Distance d'arrêt (joue le rôle de ThetaArret) : signe selon le sens de la vitesse
    RobotGstate.DistanceArret = (RobotGstate.VLin * Abs(RobotGstate.VLin)) / (2 * RobotGstate.AccLin);

    // Incrément de ce cycle (joue le rôle de IncrementTheta)
    RobotGstate.IncrementLin = RobotGstate.VLin * RobotGstate.Tsampling;

    if (((RobotGstate.DistanceArret >= 0 && RobotGstate.DistanceRestante >= 0) ||
            (RobotGstate.DistanceArret <= 0 && RobotGstate.DistanceRestante <= 0)) &&
            Abs(RobotGstate.DistanceRestante) >= Abs(RobotGstate.DistanceArret)) {

        // On accélère en rampe saturée
        if (RobotGstate.DistanceRestante > 0) {
            // Waypoint devant : on accélère en positif, saturé à VLinMax
            RobotGstate.VLin = Min(RobotGstate.VLin + RobotGstate.AccLin / RobotGstate.FQEI, RobotGstate.VLinMax);
        } else if (RobotGstate.DistanceRestante < 0) {
            // Waypoint derrière : on accélère en négatif, saturé à -VLinMax
            RobotGstate.VLin = Max(RobotGstate.VLin - RobotGstate.AccLin / RobotGstate.FQEI, -RobotGstate.VLinMax);
        }
    } else {
        // On freine en rampe saturée
        if (RobotGstate.VLin > 0) {
            RobotGstate.VLin = Max(RobotGstate.VLin - RobotGstate.AccLin / RobotGstate.FQEI, 0);
        } else if (RobotGstate.VLin < 0) {
            RobotGstate.VLin = Min(RobotGstate.VLin + RobotGstate.AccLin / RobotGstate.FQEI, 0);
        }
    }

    // On ne dépasse jamais le projeté
    if (Abs(RobotGstate.DistanceRestante) < Abs(RobotGstate.IncrementLin)) {
        RobotGstate.IncrementLin = RobotGstate.DistanceRestante;
    }

    // On intègre le déplacement le long de l'axe du ghost
    RobotGstate.xGhost += RobotGstate.IncrementLin * c;
    RobotGstate.yGhost += RobotGstate.IncrementLin * s;

    // Valeur affichée dans l'interface
    RobotGstate.DistanceGhostWaypoint = Abs(RobotGstate.DistanceRestante);

    // Projeté atteint et ghost arrêté -> retour en Idle
    if (RobotGstate.VLin == 0 && Abs(RobotGstate.DistanceRestante) < SEUIL_ARRIVEE) {
        RobotGstate.state = GHOST_IDLE;
    }

    return; // on ne passe pas dans le code de rotation

}

void GhostComputationRot(void) {
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
    // CALCUL DE LA DISTANCE REELLE DU GHOST AU WAYPOINT
    // ============================================================

    float dx = RobotGstate.xWaypoint - RobotGstate.xGhost;
    float dy = RobotGstate.yWaypoint - RobotGstate.yGhost;
    float distanceAvantDeplacement = sqrtf(dx * dx + dy * dy);

    // ============================================================
    // DISTANCE PARCOURUE PENDANT CE CYCLE
    // ============================================================

    float distanceDeplacement = RobotGstate.VGhost * RobotGstate.Tsampling;

    // ============================================================
    // CALCUL DE LA POSITION DU GHOST
    // ============================================================

    /*
     * Si le Ghost est déjà arrivé au waypoint,
     * on le place exactement sur le waypoint.
     *
     * Cela évite que la position continue à évoluer.
     */

    if (distanceAvantDeplacement <= 0.001f) {

        RobotGstate.xGhost = RobotGstate.xWaypoint;
        RobotGstate.yGhost = RobotGstate.yWaypoint;
        RobotGstate.DistanceGhostWaypoint = 0.0f;
    }/*
         * Si le Ghost va atteindre ou dépasser le waypoint
         * pendant ce cycle, on le place directement sur le waypoint.
         *
         * Exemple :
         *
         * Distance restante = 0.01 m
         * Déplacement prévu = 0.02 m
         *
         * Au lieu de faire :
         *
         * 0.00 -> 0.02
         *
         * on fait :
         *
         * 0.00 -> 0.01
         *
         * et le Ghost s'arrête exactement sur le waypoint.
         */

    else if (distanceAvantDeplacement <= distanceDeplacement) {
        RobotGstate.xGhost = RobotGstate.xWaypoint;
        RobotGstate.yGhost = RobotGstate.yWaypoint;
        RobotGstate.DistanceGhostWaypoint = 0.0f;
    }/*
         * Sinon, le Ghost continue normalement son déplacement.
         */

    else {
        RobotGstate.xGhost += RobotGstate.VGhost * cosf(RobotGstate.ThetaGhost) * RobotGstate.Tsampling;
        RobotGstate.yGhost += RobotGstate.VGhost * sinf(RobotGstate.ThetaGhost) * RobotGstate.Tsampling;
    }
    // ============================================================
    // CALCUL DE LA DISTANCE DU WAYPOINT AU SEGMENT AB
    // ============================================================
    CalculDistanceGhostWaypoint();


    // On gère les erreurs numériques d?arrondis
    if (RobotGstate.VTheta == 0 && Abs(RobotGstate.ThetaRestant) < 0.01) {
        RobotGstate.ThetaGhost = RobotGstate.ThetaWayPoint;

        RobotGstate.VLin = 0;
        RobotGstate.state = GHOST_DEPLACEMENT_LINEAIRE;

    }
    if (RobotGstate.ThetaRestant != 0) {

        RobotGstate.ThetaWayPoint = atan2f(RobotGstate.yWaypoint - RobotGstate.yGhost, RobotGstate.xWaypoint - RobotGstate.xGhost);

    }
}

// Retourne 1 si le waypoint est DEVANT le ghost (angle dans [-90° ; 90°]), 0 s'il est DERRIERE

int WaypointDevant(void) {
    float dx = RobotGstate.xWaypoint - RobotGstate.xGhost;
    float dy = RobotGstate.yWaypoint - RobotGstate.yGhost;

    // Direction du waypoint vue depuis le ghost
    float angleWaypoint = atan2f(dy, dx);

    // Angle entre l'axe de déplacement (ThetaGhost) et cette direction, ramené dans [-pi ; pi]
    RobotGstate.AngleAxeWaypoint = ModuloByAngle(0, angleWaypoint - RobotGstate.ThetaGhost);

    // Dans [-90° ; 90°] -> devant, sinon derrière
    if (RobotGstate.AngleAxeWaypoint >= -M_PI / 2 && RobotGstate.AngleAxeWaypoint <= M_PI / 2)
        return 1;
    else
        return 0;
}

// Distance signée entre le ghost et le projeté du waypoint sur l'axe du ghost

float DistanceProjeteeSurAxe(void) {
    float dx = RobotGstate.xWaypoint - RobotGstate.xGhost;
    float dy = RobotGstate.yWaypoint - RobotGstate.yGhost;
    return dx * cosf(RobotGstate.ThetaGhost) + dy * sinf(RobotGstate.ThetaGhost);
}

void SetWaypoint(float x, float y) {
    RobotGstate.xWaypoint = x;
    RobotGstate.yWaypoint = y;
    RobotGstate.ThetaWayPoint = atan2f(y - RobotGstate.yGhost, x - RobotGstate.xGhost);
    RobotGstate.VTheta = 0;
    RobotGstate.VLin = 0;
    RobotGstate.state = GHOST_ROTATION;
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


