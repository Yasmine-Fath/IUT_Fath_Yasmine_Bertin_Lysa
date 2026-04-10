#define PI 3.141592653589793

#ifndef TOOLBOX_H
#define	TOOLBOX_H
//    float Abs(float value);
//    float Max(float value, float value2);
//    float Min(float value, float value2);
//    float LimitToInterval(float value, float lowLimit, float highLimit);
//    float RadianToDegree(float value);
//    float DegreeToRadian(float value);
    void getBytesFromFloat(unsigned char *p, int index, float f);
    float getFloatFromBytes(const unsigned char *p, int index);
    void getBytesFromInt32(unsigned char *p, int index, long in);
    void getBytesFromDouble(unsigned char *p, int index, double d);

#endif
