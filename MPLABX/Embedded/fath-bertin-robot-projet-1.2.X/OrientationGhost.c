#include "OrientationGhost.h"
#include "QEI.h"
#include "Robot.h"
#include "math.h"
#include "Utilities.h"
#include "main.h"

void SetupGhostState(volatile GHOST_STATE* GHOST, double Theta, double vitesse, double accel, double vitessemax){
    
    GHOST->Theta = Theta;
    GHOST->vitesse = vitesse; 
    GHOST->accel = accel;
    GHOST->vitessemax = vitessemax;
}

void compteurGhost (){

    GhostState.ThetaRestant = ModuloByAngle(GhostState.ThetaGhost, GhostState.ThetaWayPoint) - GhostState.ThetaGhost;

    GhostState.ThetaArret = (GhostState.VTheta*GhostState.VTheta)/(2*GhostState.AccTheta);
    GhostState.incrementTheta = GhostState.VTheta*GhostState.Tsampling;
    
    
    if (GhostState.VTheta < 0) {
       GhostState.ThetaArret = - GhostState.ThetaArret;
    
    }
    
    if (((GhostState.ThetaArret >= 0 && GhostState.ThetaRestant > =0) || (GhostState.ThetaArret <= 0 && GhostState.ThetaRestant <= 0)) && |GhostState.ThetaRestant|>=|GhostState.ThetaArret|) { 
        // On accélère en rampe saturée
        if (GhostState.ThetaRestant > 0){
            // Si la destination est devant, on accélère en positif en saturant la vitesse à V?Max
            GhostState.VTheta = Min(GhostState.VTheta + GhostState.AccTheta/GhostState.FQEI, GhostState.VThetamax); 
        }
        
        else if(GhostState.ThetaRestant < 0) {
          //Si la destination est derrière, on accélère en négatif en saturant la vitesse à ?V?Max
            GhostState.VTheta = Max(GhostState.VTheta - GhostState.AccTheta/GhostState.FQEI, - GhostState.VThetamax);
        
        }
    }
    
    else {
        // On freine en rampe saturée
        if (GhostState.VTheta > 0) {
        //Si la vitesse positive est positive, on freine en positif en saturant la vitesse à 0
        GhostState.VTheta = Min(GhostState.VTheta - GhostState.AccTheta/GhostState.FQEI, 0); 
        }
        else if (GhostState.VTheta < 0) {
        //Si la vitesse est négative, on freine en négatif en saturant la vitesse à 0
        GhostState.VTheta = Max(GhostState.VTheta + GhostState.AccTheta/GhostState.FQEI, 0);
                
        }
        
        if (Abs(GhostState.ThetaRestant) < Abs(GhostState.incrementTheta)){
            GhostState.incrementTheta = GhostState.ThetaRestant;   
        }
    }
    
    // On intègre le déplacement
    GhostState.ThetaGhost = GhostState.ThetaGhost + GhostState.incrementTheta;
    
    // On gère les erreurs numériques d?arrondis
    if (GhostState.VTheta == 0 && |GhostState.ThetaRestant| < 0.01){
        GhostState.ThetaGhost = GhostState.ThetaWayPoint;
    }
    
}  



// 03/09
/// AFFICHAGE DU GHOST

void AffichageGHOST(){
    unsigned char payload[4];
     
    //*****************Theta Ghost ****************
    getBytesFromFloat(payload, 0, GhostState.ThetaGhost);
   
    UartEncodeAndSendMessage(0x0090, 4, payload );
    
}