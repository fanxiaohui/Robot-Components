#include "pid.h"



s8 pid_getCorrection(pid_struct_t s_pid_example)
{
	s_pid_example.correction = 0;

/** Calculates the total error **/	
	s_pid_example.totalError += s_pid_example.currentError;
	
/** Calculates the correction after the following formula (Kp * currentError) + (Kd * (currentError - previousError)) + (Ki * totalError) **/		
	s_pid_example.correction = (s_pid_example.Kp * s_pid_example.currentError) 
	+ (s_pid_example.Kd * (s_pid_example.currentError - s_pid_example.previousError)) 
	+ (s_pid_example.Ki * s_pid_example.totalError);
/** The previous error takes the value of the current one after the calculation of correction **/			
	s_pid_example.previousError = s_pid_example.currentError;


	if(s_pid_example.correction > 100)
	s_pid_example.correction = 100;
	if(s_pid_example.correction < -100)
	s_pid_example.correction = -100;


/** Returns the correction value**/	
	return s_pid_example.correction;
}

