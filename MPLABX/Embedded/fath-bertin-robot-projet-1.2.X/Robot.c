#include "Robot.h"
#include "asservissement.h"
volatile ROBOT_STATE_BITS robotState;

// Asservissement polaire
volatile PidCorrector PidX;      // PID pour vitesse linéaire (VL)
volatile PidCorrector PidTheta;  // PID pour vitesse angulaire (VA)





