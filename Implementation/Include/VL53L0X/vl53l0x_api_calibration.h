/*******************************************************************************
Copyright © 2016, STMicroelectronics International N.V.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of STMicroelectronics nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _VL53L0X_API_CALIBRATION_H_
#define _VL53L0X_API_CALIBRATION_H_

#include "VL53L0X/vl53l0x_def.h"
#include "VL53L0X/vl53l0x_platform.h"


#ifdef __cplusplus
extern "C" {
#endif

VL53L0X_Error VL53L0X_perform_xtalk_calibration(VL53L0X_DEV Dev,
		f1616 XTalkCalDistance,
		f1616 *pXTalkCompensationRateMegaCps);

VL53L0X_Error VL53L0X_perform_offset_calibration(VL53L0X_DEV Dev,
		f1616 CalDistanceMilliMeter,
		s32 *pOffsetMicroMeter);

VL53L0X_Error VL53L0X_set_offset_calibration_data_micro_meter(VL53L0X_DEV Dev,
		s32 OffsetCalibrationDataMicroMeter);

VL53L0X_Error VL53L0X_get_offset_calibration_data_micro_meter(VL53L0X_DEV Dev,
		s32 *pOffsetCalibrationDataMicroMeter);

VL53L0X_Error VL53L0X_apply_offset_adjustment(VL53L0X_DEV Dev);

VL53L0X_Error VL53L0X_perform_ref_spad_management(VL53L0X_DEV Dev,
		u32 *refSpadCount, u8 *isApertureSpads);

VL53L0X_Error VL53L0X_set_reference_spads(VL53L0X_DEV Dev,
		u32 count, u8 isApertureSpads);

VL53L0X_Error VL53L0X_get_reference_spads(VL53L0X_DEV Dev,
		u32 *pSpadCount, u8 *pIsApertureSpads);

VL53L0X_Error VL53L0X_perform_phase_calibration(VL53L0X_DEV Dev,
	u8 *pPhaseCal, const u8 get_data_enable,
	const u8 restore_config);

VL53L0X_Error VL53L0X_perform_ref_calibration(VL53L0X_DEV Dev,
	u8 *pVhvSettings, u8 *pPhaseCal, u8 get_data_enable);

VL53L0X_Error VL53L0X_set_ref_calibration(VL53L0X_DEV Dev,
		u8 VhvSettings, u8 PhaseCal);

VL53L0X_Error VL53L0X_get_ref_calibration(VL53L0X_DEV Dev,
		u8 *pVhvSettings, u8 *pPhaseCal);




#ifdef __cplusplus
}
#endif

#endif /* _VL53L0X_API_CALIBRATION_H_ */
