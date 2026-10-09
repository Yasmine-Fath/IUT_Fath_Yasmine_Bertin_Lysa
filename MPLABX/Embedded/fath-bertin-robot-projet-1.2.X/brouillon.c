
#include "Ghost.h" 
#include "QEI.h" 
#include "Robot.h" 
#include "math.h" 
#include "Utilities.h" 
#include "main.h" 
#include "UART_Protocol.h" 
#include "timer.h" 

// [AJOUT]
#define SEUIL_ARRIVEE   0.002f    // 2 mm
#define PI_F            3.14159265f
 
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
    RobotGstate.VGhost = 0.0f;      // [MODIF 1/2] 0.2f -> 0.0f : l'avance est maintenant geree par DeplacementLineaireGhost()

    // [AJOUT] Initialisation de la machine a etats et du deplacement lineaire
    RobotGstate.state = GHOST_IDLE;
    RobotGstate.VLin = 0.0f;
    RobotGstate.VLinMax = 0.5f;     // m/s
    RobotGstate.AccLin = 0.5f;      // m/s2
    RobotGstate.DistanceRestante = 0.0f;
    RobotGstate.xWaypoint = 0.0f;
    RobotGstate.yWaypoint = 0.0f;
 
} 
 
// [MODIF 2/2] ancien "void GhostComputation()" renomme. Le corps est inchange.
void GhostComputationRotation() { 
 
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
    } 
        /* 
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
    } 
        /* 
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
 
    } 
    if (RobotGstate.ThetaRestant != 0) { 
 
        RobotGstate.ThetaWayPoint = atan2f(RobotGstate.yWaypoint - RobotGstate.yGhost, RobotGstate.xWaypoint - RobotGstate.xGhost); 
 
    } 
 
 
 
} 


// ============================================================
// [AJOUT] NOUVEAU WAYPOINT : a appeler a la reception de la trame 0x0091
// ============================================================
void SetWaypoint(float x, float y) {
    RobotGstate.xWaypoint = x;
    RobotGstate.yWaypoint = y;

    // Direction du waypoint calculee tout de suite (evite d'utiliser l'ancienne valeur)
    RobotGstate.ThetaWayPoint = atan2f(y - RobotGstate.yGhost, x - RobotGstate.xGhost);

    RobotGstate.VLin = 0;
    RobotGstate.state = GHOST_ROTATION;
}


// ============================================================
// [AJOUT] Q1 : distance signee ghost -> PROJETE du waypoint sur l'axe du ghost
//        = |d| * cos(angle)
// ============================================================
float DistanceProjeteeSurAxe(void) {
    float dx = RobotGstate.xWaypoint - RobotGstate.xGhost;
    float dy = RobotGstate.yWaypoint - RobotGstate.yGhost;
    return dx * cosf(RobotGstate.ThetaGhost) + dy * sinf(RobotGstate.ThetaGhost);
}


// ============================================================
// [AJOUT] Q2 : angle entre l'axe de deplacement et la direction du waypoint
//        1 si l'angle est dans [-90 ; 90] (waypoint devant), 0 sinon (derriere)
// ============================================================
int WaypointDevant(void) {
    float dx = RobotGstate.xWaypoint - RobotGstate.xGhost;
    float dy = RobotGstate.yWaypoint - RobotGstate.yGhost;

    // Angle ramene dans [-pi ; pi]
    float angle = ModuloByAngle(0, atan2f(dy, dx) - RobotGstate.ThetaGhost);
    RobotGstate.AngleAxeWaypoint = angle;

    return (angle >= -PI_F / 2 && angle <= PI_F / 2);
}


// ============================================================
// [AJOUT] Q3 : deplacement longitudinal (meme structure que la rotation)
// ============================================================
void DeplacementLineaireGhost(void) {

    float c = cosf(RobotGstate.ThetaGhost);
    float s = sinf(RobotGstate.ThetaGhost);

    // Q2 : devant ou derriere
    int devant = WaypointDevant();

    // Q1 : DistanceRestante joue le role de ThetaRestant
    float dist = Abs(DistanceProjeteeSurAxe());
    RobotGstate.DistanceRestante = devant ? dist : -dist;

    // Equivalent de ThetaArret (signe selon le sens de la vitesse)
    RobotGstate.DistanceArret = (RobotGstate.VLin * Abs(RobotGstate.VLin)) / (2 * RobotGstate.AccLin);

    // Equivalent de IncrementTheta
    RobotGstate.IncrementLin = RobotGstate.VLin * RobotGstate.Tsampling;

    if (((RobotGstate.DistanceArret >= 0 && RobotGstate.DistanceRestante >= 0) ||
         (RobotGstate.DistanceArret <= 0 && RobotGstate.DistanceRestante <= 0)) &&
        Abs(RobotGstate.DistanceRestante) >= Abs(RobotGstate.DistanceArret)) {

        // On accelere en rampe saturee
        if (RobotGstate.DistanceRestante > 0) {
            // Waypoint devant : on accelere en positif, sature a VLinMax
            RobotGstate.VLin = Min(RobotGstate.VLin + RobotGstate.AccLin / RobotGstate.FQEI, RobotGstate.VLinMax);
        } else if (RobotGstate.DistanceRestante < 0) {
            // Waypoint derriere : on accelere en negatif, sature a -VLinMax
            RobotGstate.VLin = Max(RobotGstate.VLin - RobotGstate.AccLin / RobotGstate.FQEI, -RobotGstate.VLinMax);
        }
    } else {
        // On freine en rampe saturee
        if (RobotGstate.VLin > 0) {
            RobotGstate.VLin = Max(RobotGstate.VLin - RobotGstate.AccLin / RobotGstate.FQEI, 0);
        } else if (RobotGstate.VLin < 0) {
            RobotGstate.VLin = Min(RobotGstate.VLin + RobotGstate.AccLin / RobotGstate.FQEI, 0);
        }
    }

    // On ne depasse jamais le projete
    if (Abs(RobotGstate.DistanceRestante) < Abs(RobotGstate.IncrementLin)) {
        RobotGstate.IncrementLin = RobotGstate.DistanceRestante;
    }

    // On integre le deplacement le long de l'axe du ghost
    RobotGstate.xGhost += RobotGstate.IncrementLin * c;
    RobotGstate.yGhost += RobotGstate.IncrementLin * s;

    // Distance affichee dans l'interface
    RobotGstate.DistanceGhostWaypoint = Abs(RobotGstate.DistanceRestante);

    // Projete atteint et ghost arrete -> retour en Idle
    if (RobotGstate.VLin == 0 && Abs(RobotGstate.DistanceRestante) < SEUIL_ARRIVEE) {
        RobotGstate.state = GHOST_IDLE;
    }
}


// ============================================================
// [AJOUT] MACHINE A ETATS : c'est elle que le Timer appelle (meme nom qu'avant)
// ============================================================
void GhostComputation() {

    switch (RobotGstate.state) {

        case GHOST_IDLE:
            RobotGstate.VTheta = 0;
            RobotGstate.VLin = 0;
            break;

        case GHOST_ROTATION:
            GhostComputationRotation();     // ton code de rotation

            // Fin de la rotation : on passe a l'avance
            if (RobotGstate.VTheta == 0 && Abs(RobotGstate.ThetaRestant) < 0.01f) {
                RobotGstate.VLin = 0;
                RobotGstate.state = GHOST_DEPLACEMENT_LINEAIRE;
            }
            break;

        case GHOST_DEPLACEMENT_LINEAIRE:
            DeplacementLineaireGhost();
            break;
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
