#include "pid.h"
#include "math.h"

void pid_init(pid_struct_t s_pid){
	s_pid.currentError = 0;
	s_pid.previousError = 0;
	s_pid.totalError = 0;
}

f24 pid_getCorrection(pid_struct_t s_pid)
{
	f24 correction = 0;

/** Calculates the total error **/	
	s_pid.totalError += s_pid.currentError;
	
/** Calculates the correction after the following formula (Kp * currentError) + (Kd * (currentError - previousError)) + (Ki * totalError) **/		
	correction = (s_pid.Kp * s_pid.currentError)
				+ (s_pid.Kd * (s_pid.currentError - s_pid.previousError))
				+ (s_pid.Ki * s_pid.totalError);
/** The previous error takes the value of the current one after the calculation of correction **/			
	s_pid.previousError = s_pid.currentError;

	if(correction > s_pid.maximumCorrection)
		correction = s_pid.maximumCorrection;
	if(correction < s_pid.minimumCorrection)
		correction = s_pid.minimumCorrection;

/** Returns the correction value**/	
	return correction;
}

