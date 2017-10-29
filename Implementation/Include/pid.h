#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED

float getKp();
float getKd();
float getKi();

void setKp(float Kp);
void setKd(float Kd);
void setKi(float Ki);
signed char PID_GetCorrection(float currentError);

#endif