
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

double distancePointDroite(Point P, Point A, Point B) {
    double num = fabs((B.x - A.x) * (A.y - P.y) - (A.x - P.x) * (B.y - A.y));
    double den = sqrt((B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y));
    
    // Vérification pour éviter une division par zéro si A et B sont confondus
    if (den == 0.0) {
        return sqrt((P.x - A.x) * (P.x - A.x) + (P.y - A.y) * (P.y - A.y));
    }
    
    return num / den;
}