#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED
#include "types.h"

typedef struct pid_struct_t
{
	
/** Proportional error value **/
	float Kp;
/** Derivative error value **/	
	float Kd;
/** Integral error value **/	
	float Ki;
/** Stores the previous error **/
	float previousError;
/** Store the total error value, sum of all current error **/
	float totalError;
/** The current error **/	
	float currentError;
/** The corection needed after the following formula (Kp * currentError) + (Kd * (currentError - previousError)) + (Ki * totalError)**/
	float correction;
	
}pid_struct_t;

s8 pid_getCorrection(pid_struct_t s_pid_example);

/**	Gets the correction needed using a PID regulator
	@param[in]	currentError: This variable stores the current error
	@return		correction: This is the correction needed for the system to be happy
**/


void pid_init(pid_struct_t s_pid_example);
/** Initialize the values of the regulator
**/

#endif /* PID_H_INCLUDED */

