#include "pid.h"

float Kp = 30;
float Kd = 0;
float Ki = 0;
float previousError = 0;
float totalError = 0;

float getKp()
{
	return Kp;
}
float getKd()
{
	return Kd;
}
float getKi()
{
	return Ki;
}

void setKp(float Kp)
{
	Kp = Kp;
}
void setKd(float Kd)
{
	Kd = Kd;
}
void setKi(float Ki)
{
	Ki = Ki;
}

signed char PID_GetCorrection(float currentError)
{
	float correction = 0;
	  
	totalError += currentError;
	correction = (Kp * currentError) + (Kd * (currentError - previousError)) + (Ki * totalError);
	previousError = currentError;

	if(correction > 100)
		correction = 100;
	if(correction < -100)
		correction = -100;
	  
	return correction;
}
