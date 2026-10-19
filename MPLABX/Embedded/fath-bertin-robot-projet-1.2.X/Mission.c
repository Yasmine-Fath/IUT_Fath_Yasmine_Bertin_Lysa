#include <math.h>
#include <xc.h>
#include "Mission.h"
#include "IO.h"
#include "Ghost.h"
#include "Robot.h"
#include "Utilities.h"
#include "asservissement.h"

// ---------- Paramètres terrain (en mètres) ----------
#define X_DEPART   1.0f     // position de départ du robot sur le terrain
#define Y_DEPART   0.0f
#define Y_CARRE    1.5f     // moitié du terrain (3 m / 2)

#define NB_WAYPOINTS 3
static const float wpTerrain[NB_WAYPOINTS][2] = {
    {X_DEPART, 1.0f}, // 1 m devant
    {X_DEPART + 0.5f, 1.0f}, // tourne à droite, 0,5 m
    {X_DEPART + 0.5f, 2.0f} // tourne à gauche, 1 m, puis arrêt
};

// ---------- Tolérances ----------
#define TOL_GHOST_M   0.003f   // le ghost est arrivé au waypoint
#define TOL_ROBOT_M   0.05f    // le robot réel est assez proche du waypoint
#define TIMEOUT_TICKS (250 * 20)  // 20 s max par waypoint (250 Hz)

typedef enum {
    MISSION_IDLE,
    MISSION_EN_COURS,
    MISSION_FINIE
} MissionState;

static volatile MissionState etatMission = MISSION_IDLE;
static int indexWp = 0;
static unsigned int ticksWp = 0;

// Pose de départ du robot en repère odométrie, relevée à l'appui
static float x_0, y_0, cos_0, sin_0;



// ---------------------------------------------------------------
// Bouton : anti-rebond (5 échantillons à 250 Hz = 20 ms) + front
// ---------------------------------------------------------------

static unsigned char BoutonFrontAppui(void) {
    static unsigned char compteur = 0;
    static unsigned char dejaVu = 0;

    if (BOUTON_CARRE == BOUTON_NIVEAU_APPUI) {
        if (compteur < 5) compteur++;
        if (compteur == 5 && !dejaVu) {
            dejaVu = 1;
            return 1;
        }
    } else {
        compteur = 0;
        dejaVu = 0;
    }
    return 0;
}

// ---------------------------------------------------------------
// Envoie le waypoint i au ghost (conversion terrain -> odométrie)
// ---------------------------------------------------------------

static void MissionEnvoyerWaypoint(int i) {
    // Repère local du robot au départ : avant = +y terrain, gauche = -x terrain
    float avant = wpTerrain[i][1] - Y_DEPART;
    float gauche = X_DEPART - wpTerrain[i][0];

    RobotGstate.xWaypoint = x_0 + avant * cos_0 - gauche * sin_0;
    RobotGstate.yWaypoint = y_0 + avant * sin_0 + gauche * cos_0;
    RobotGstate.ThetaWayPoint = atan2f(RobotGstate.yWaypoint - RobotGstate.yGhost,
            RobotGstate.xWaypoint - RobotGstate.xGhost);
    ticksWp = 0;
}

static void MissionDemarrer(void) {
    // Pose de départ = pose odométrique actuelle
    x_0 = (float) robotState.xPosFromOdometry;
    y_0 = (float) robotState.yPosFromOdometry;
    float th_0 = (float) robotState.angleRadianFromOdometry;
    cos_0 = cosf(th_0);
    sin_0 = sinf(th_0);

    // Le ghost repart exactement de la pose du robot, à l'arrêt
    RobotGstate.xGhost = x_0;
    RobotGstate.yGhost = y_0;
    RobotGstate.ThetaGhost = th_0;
    RobotGstate.VTheta = 0;
    RobotGstate.xWaypoint = x_0;
    RobotGstate.yWaypoint = y_0;
    RobotGstate.ThetaWayPoint = th_0;

    // Évite un coup de dérivée au démarrage
    robotState.PidX.erreurIntegrale = 0;
    robotState.PidX.epsilon_1 = 0;
    robotState.PidTheta.erreurIntegrale = 0;
    robotState.PidTheta.epsilon_1 = 0;

    indexWp = 0;
    MissionEnvoyerWaypoint(indexWp);
    etatMission = MISSION_EN_COURS;
}

static void MissionArreter(void) {
    // Le ghost s'arrête sur place, le PD maintient le robot dessus
    RobotGstate.xWaypoint = RobotGstate.xGhost;
    RobotGstate.yWaypoint = RobotGstate.yGhost;
    RobotGstate.ThetaWayPoint = RobotGstate.ThetaGhost;
    etatMission = MISSION_FINIE;
}

unsigned char MissionEstActive(void) {
    return (etatMission != MISSION_EN_COURS);
}

// ---------------------------------------------------------------
// À appeler à 250 Hz, AVANT GhostComputation()
// ---------------------------------------------------------------

void MissionUpdate(void) {
    unsigned char appui = BoutonFrontAppui();
    if (appui && etatMission != MISSION_EN_COURS) { // relance à chaque appui, mais pas en cours de route
        MissionDemarrer();
        return;
    }
    switch (etatMission) {
        case MISSION_IDLE:
            break;

        case MISSION_EN_COURS:
        {
            float dxg = RobotGstate.xWaypoint - RobotGstate.xGhost;
            float dyg = RobotGstate.yWaypoint - RobotGstate.yGhost;
            float dxr = RobotGstate.xWaypoint - (float) robotState.xPosFromOdometry;
            float dyr = RobotGstate.yWaypoint - (float) robotState.yPosFromOdometry;

            unsigned char ghostArrive = (dxg * dxg + dyg * dyg) < (TOL_GHOST_M * TOL_GHOST_M);
            unsigned char robotArrive = (dxr * dxr + dyr * dyr) < (TOL_ROBOT_M * TOL_ROBOT_M);

            if (ghostArrive && robotArrive) {
                indexWp++;
                if (indexWp >= NB_WAYPOINTS)
                    MissionArreter(); // le ghost reste sur le dernier point
                else
                    MissionEnvoyerWaypoint(indexWp);
            } else if (++ticksWp > TIMEOUT_TICKS) {
                MissionArreter(); // sécurité : robot bloqué
            }
            break;
        }

        case MISSION_FINIE:
            // Pour relancer : reset de la carte (le robot n'est plus au point de départ)
            // Attente d'un nouvel appui sur le bouton
            break;
    }
}
