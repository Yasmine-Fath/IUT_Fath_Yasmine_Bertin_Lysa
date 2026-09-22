
#include "Utilities.h"
#include "math.h"
#include <xc.h>


double Abs(double value)
{
    if (value>=0)
        return value;
    else return -value;      
}

double Max(double value, double value2)
{
    if(value>value2)
        return value;
    else return value2;
}

double Min(double value,double value2)
{
    if (value < value2)
        return value;
    else
        return value2;    
}

double LimitToInterval(double value, double min, double max)
{
    if(value < min)
        return min;
    else if(value > max)
        return max;
    else
        return value;
}

double Modulo2PIAngleRadian(double angleRadian) {
    double angleTemp = fmod(angleRadian - PI, 2 * PI) + PI;
    return fmod(angleTemp + PI, 2 * PI) - PI;
}

float getFloat(unsigned char *p, int index)
{
    float *result_ptr = (float*)(p + index);
    float result = *result_ptr;
    return result;
}

double getDouble(unsigned char *p, int index)
{
    double *result_ptr = (double*)(p + index);
    return *result_ptr;
}

void getBytesFromFloat(unsigned char *p, int index, float f)
{
    int i;
    unsigned char *f_ptr = (unsigned char*)&f;
    for (i = 0; i < 4; i++)
        p[index + i] = f_ptr[i];
}

void getBytesFromInt32(unsigned char *p, int index, long in)
{
    int i;
    unsigned char *f_ptr = (unsigned char*)&in;
    for (i = 0; i < 4; i++)
        p[index + i] = f_ptr[3-i];
}

void getBytesFromDouble(unsigned char *p, int index, double d)
{
    int i;
    unsigned char *f_ptr = (unsigned char*)&d;
    for (i = 0; i < 8; i++)
        p[index + i] = f_ptr[i];
}

double ModuloByAngle(double angleToCenterAround, double angle)
{
    double interAngle = Modulo2PIAngleRadian (angle - angleToCenterAround);
    if(interAngle > M_PI)
        interAngle -= M_2_PI;
    return interAngle + angleToCenterAround;
}

// ---------------------------------------------------------------------
// CalculDistanceProjection : calcule la distance signee entre le robot
// et la PROJECTION du point cible sur l'axe d'avance du robot.
//
// Parametres :
//   xRobot, yRobot   : position actuelle du robot (ou du ghost)
//   thetaRobot       : cap actuel du robot (en radians)
//   xCible, yCible   : position du point que l'on veut atteindre
//
// Retourne :
//   La distance signee, le long de l'axe du robot, jusqu'au point
//   projete de la cible sur cet axe.
//     > 0  => le projete est devant le robot
//     < 0  => le projete est derriere le robot
// ---------------------------------------------------------------------
double CalculDistanceProjection(double xRobot, double yRobot, double thetaRobot,
                                 double xCible, double yCible)
{
    // Vecteur directeur unitaire de l'axe d'avance du robot
    double ux = cos(thetaRobot);
    double uy = sin(thetaRobot);

    // Vecteur allant du robot vers la cible
    double vx = xCible - xRobot;
    double vy = yCible - yRobot;

    // Produit scalaire v.u : longueur (signee) de la projection de v sur u
    double distance = vx*ux + vy*uy;

    return distance;
}