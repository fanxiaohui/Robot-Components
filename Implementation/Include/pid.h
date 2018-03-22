#ifndef PID_H_INCLUDED
#define PID_H_INCLUDED
#include "types.h"

typedef struct pid_struct_t
{
/** Proportional error value **/
	f24 Kp;
/** Derivative error value **/	
	f24 Kd;
/** Integral error value **/	
	f24 Ki;
/** Stores the previous error **/
	s32 previousError;
/** Store the total error value, sum of all current error **/
	s32 totalError;
/** The current error **/	
	s32 currentError;
/**	Correction minimum value */
	f24 minimumCorrection;
/**	Correction maximum value */
	f24 maximumCorrection;
}pid_struct_t;

f24 pid_getCorrection(pid_struct_t s_pid);

/**	Gets the correction needed using a PID regulator
	@param[in]	currentError: This variable stores the current error
	@return		correction: This is the correction needed for the system to be happy
**/


void pid_init(pid_struct_t s_pid_example);
/** Initialize the values of the regulator
**/

#endif /* PID_H_INCLUDED */

