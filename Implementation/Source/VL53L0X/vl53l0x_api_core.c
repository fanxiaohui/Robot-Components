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
 ******************************************************************************/

#include "VL53L0X/vl53l0x_api.h"
#include "VL53L0X/vl53l0x_api_core.h"
#include "VL53L0X/vl53l0x_api_calibration.h"


#ifndef __KERNEL__
#include <stdlib.h>
#endif
VL53L0X_Error VL53L0X_reverse_bytes(u8 *data, u32 size)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 tempData;
	u32 mirrorIndex;
	u32 middle = size/2;
	u32 index;

	for (index = 0; index < middle; index++) {
		mirrorIndex		 = size - index - 1;
		tempData		 = data[index];
		data[index]		 = data[mirrorIndex];
		data[mirrorIndex] = tempData;
	}
	return Status;
}

VL53L0X_Error VL53L0X_measurement_poll_for_completion(VL53L0X_DEV Dev)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 NewDataReady = 0;
	u32 LoopNb;

	LoopNb = 0;

	do {
		Status = VL53L0X_GetMeasurementDataReady(Dev, &NewDataReady);
		if (Status != 0)
			break; /* the error is set */

		if (NewDataReady == 1)
			break; /* done note that status == 0 */

		LoopNb++;
		if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
			Status = VL53L0X_ERROR_TIME_OUT;
			break;
		}

		VL53L0X_PollingDelay(Dev);
	} while (1);

	return Status;
}


u8 VL53L0X_decode_vcsel_period(u8 vcsel_period_reg)
{
	/*!
	 * Converts the encoded VCSEL period register value into the real
	 * period in PLL clocks
	 */

	u8 vcsel_period_pclks = 0;

	vcsel_period_pclks = (vcsel_period_reg + 1) << 1;

	return vcsel_period_pclks;
}

u8 VL53L0X_encode_vcsel_period(u8 vcsel_period_pclks)
{
	/*!
	 * Converts the encoded VCSEL period register value into the real period
	 * in PLL clocks
	 */

	u8 vcsel_period_reg = 0;

	vcsel_period_reg = (vcsel_period_pclks >> 1) - 1;

	return vcsel_period_reg;
}


u32 VL53L0X_isqrt(u32 num)
{
	/*
	 * Implements an integer square root
	 *
	 * From: http://en.wikipedia.org/wiki/Methods_of_computing_square_roots
	 */

	u32  res = 0;
	u32  bit = 1ul << 30;
	/* The second-to-top bit is set:
	 *	1 << 14 for 16-bits, 1 << 30 for 32 bits */

	 /* "bit" starts at the highest power of four <= the argument. */
	while (bit > num)
		bit >>= 2;


	while (bit != 0) {
		if (num >= res + bit) {
			num -= res + bit;
			res = (res >> 1) + bit;
		} else
			res >>= 1;

		bit >>= 2;
	}

	return res;
}


u32 VL53L0X_quadrature_sum(u32 a, u32 b)
{
	/*
	 * Implements a quadrature sum
	 *
	 * rea = sqrt(a^2 + b^2)
	 *
	 * Trap overflow case max input value is 65535 (16-bit value)
	 * as internal calc are 32-bit wide
	 *
	 * If overflow then seta output to maximum
	 */
	u32  res = 0;

	if (a > 65535 || b > 65535)
		res = 65535;
	else
		res = VL53L0X_isqrt(a * a + b * b);

	return res;
}


VL53L0X_Error VL53L0X_device_read_strobe(VL53L0X_DEV Dev)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 strobe;
	u32 LoopNb;

	Status |= VL53L0X_WrByte(Dev, 0x83, 0x00);

	/* polling
	 * use timeout to avoid deadlock*/
	if (Status == VL53L0X_ERROR_NONE) {
		LoopNb = 0;
		do {
			Status = VL53L0X_RdByte(Dev, 0x83, &strobe);
			if ((strobe != 0x00) || Status != VL53L0X_ERROR_NONE)
					break;

			LoopNb = LoopNb + 1;
		} while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

		if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP)
			Status = VL53L0X_ERROR_TIME_OUT;

	}

	Status |= VL53L0X_WrByte(Dev, 0x83, 0x01);

	return Status;

}

VL53L0X_Error VL53L0X_get_info_from_device(VL53L0X_DEV Dev, u8 option)
{

	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 byte;
	u32 TmpDWord;
	u8 ModuleId;
	u8 Revision;
	u8 ReferenceSpadCount = 0;
	u8 ReferenceSpadType = 0;
	u32 PartUIDUpper = 0;
	u32 PartUIDLower = 0;
	u32 OffsetFixed1104_mm = 0;
	s16 OffsetMicroMeters = 0;
	u32 DistMeasTgtFixed1104_mm = 400 << 4;
	u32 DistMeasFixed1104_400_mm = 0;
	u32 SignalRateMeasFixed1104_400_mm = 0;
	char ProductId[19];
	char *ProductId_tmp;
	u8 ReadDataFromDeviceDone;
	f1616 SignalRateMeasFixed400mmFix = 0;
	u8 NvmRefGoodSpadMap[VL53L0X_REF_SPAD_BUFFER_SIZE];
	int i;

	ReadDataFromDeviceDone = VL53L0X_GETDEVICESPECIFICPARAMETER(Dev,
			ReadDataFromDeviceDone);

	/* This access is done only once after that a GetDeviceInfo or
	 * datainit is done*/
	if (ReadDataFromDeviceDone != 7) {

		Status |= VL53L0X_WrByte(Dev, 0x80, 0x01);
		Status |= VL53L0X_WrByte(Dev, 0xFF, 0x01);
		Status |= VL53L0X_WrByte(Dev, 0x00, 0x00);

		Status |= VL53L0X_WrByte(Dev, 0xFF, 0x06);
		Status |= VL53L0X_RdByte(Dev, 0x83, &byte);
		Status |= VL53L0X_WrByte(Dev, 0x83, byte|4);
		Status |= VL53L0X_WrByte(Dev, 0xFF, 0x07);
		Status |= VL53L0X_WrByte(Dev, 0x81, 0x01);

		Status |= VL53L0X_PollingDelay(Dev);

		Status |= VL53L0X_WrByte(Dev, 0x80, 0x01);

		if (((option & 1) == 1) &&
			((ReadDataFromDeviceDone & 1) == 0)) {
			Status |= VL53L0X_WrByte(Dev, 0x94, 0x6b);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			ReferenceSpadCount = (u8)((TmpDWord >> 8) & 0x07f);
			ReferenceSpadType  = (u8)((TmpDWord >> 15) & 0x01);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x24);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);


			NvmRefGoodSpadMap[0] = (u8)((TmpDWord >> 24)
				& 0xff);
			NvmRefGoodSpadMap[1] = (u8)((TmpDWord >> 16)
				& 0xff);
			NvmRefGoodSpadMap[2] = (u8)((TmpDWord >> 8)
				& 0xff);
			NvmRefGoodSpadMap[3] = (u8)(TmpDWord & 0xff);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x25);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			NvmRefGoodSpadMap[4] = (u8)((TmpDWord >> 24)
				& 0xff);
			NvmRefGoodSpadMap[5] = (u8)((TmpDWord >> 16)
				& 0xff);
		}

		if (((option & 2) == 2) &&
			((ReadDataFromDeviceDone & 2) == 0)) {

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x02);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdByte(Dev, 0x90, &ModuleId);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x7B);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdByte(Dev, 0x90, &Revision);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x77);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			ProductId[0] = (char)((TmpDWord >> 25) & 0x07f);
			ProductId[1] = (char)((TmpDWord >> 18) & 0x07f);
			ProductId[2] = (char)((TmpDWord >> 11) & 0x07f);
			ProductId[3] = (char)((TmpDWord >> 4) & 0x07f);

			byte = (u8)((TmpDWord & 0x00f) << 3);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x78);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			ProductId[4] = (char)(byte +
					((TmpDWord >> 29) & 0x07f));
			ProductId[5] = (char)((TmpDWord >> 22) & 0x07f);
			ProductId[6] = (char)((TmpDWord >> 15) & 0x07f);
			ProductId[7] = (char)((TmpDWord >> 8) & 0x07f);
			ProductId[8] = (char)((TmpDWord >> 1) & 0x07f);

			byte = (u8)((TmpDWord & 0x001) << 6);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x79);

			Status |= VL53L0X_device_read_strobe(Dev);

			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			ProductId[9] = (char)(byte +
					((TmpDWord >> 26) & 0x07f));
			ProductId[10] = (char)((TmpDWord >> 19) & 0x07f);
			ProductId[11] = (char)((TmpDWord >> 12) & 0x07f);
			ProductId[12] = (char)((TmpDWord >> 5) & 0x07f);

			byte = (u8)((TmpDWord & 0x01f) << 2);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x7A);

			Status |= VL53L0X_device_read_strobe(Dev);

			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			ProductId[13] = (char)(byte +
					((TmpDWord >> 30) & 0x07f));
			ProductId[14] = (char)((TmpDWord >> 23) & 0x07f);
			ProductId[15] = (char)((TmpDWord >> 16) & 0x07f);
			ProductId[16] = (char)((TmpDWord >> 9) & 0x07f);
			ProductId[17] = (char)((TmpDWord >> 2) & 0x07f);
			ProductId[18] = '\0';

		}

		if (((option & 4) == 4) &&
			((ReadDataFromDeviceDone & 4) == 0)) {

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x7B);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &PartUIDUpper);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x7C);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &PartUIDLower);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x73);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			SignalRateMeasFixed1104_400_mm = (TmpDWord &
				0x0000000ff) << 8;

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x74);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			SignalRateMeasFixed1104_400_mm |= ((TmpDWord &
				0xff000000) >> 24);

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x75);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			DistMeasFixed1104_400_mm = (TmpDWord & 0x0000000ff)
							<< 8;

			Status |= VL53L0X_WrByte(Dev, 0x94, 0x76);
			Status |= VL53L0X_device_read_strobe(Dev);
			Status |= VL53L0X_RdDWord(Dev, 0x90, &TmpDWord);

			DistMeasFixed1104_400_mm |= ((TmpDWord & 0xff000000)
							>> 24);
		}

		Status |= VL53L0X_WrByte(Dev, 0x81, 0x00);
		Status |= VL53L0X_WrByte(Dev, 0xFF, 0x06);
		Status |= VL53L0X_RdByte(Dev, 0x83, &byte);
		Status |= VL53L0X_WrByte(Dev, 0x83, byte&0xfb);
		Status |= VL53L0X_WrByte(Dev, 0xFF, 0x01);
		Status |= VL53L0X_WrByte(Dev, 0x00, 0x01);

		Status |= VL53L0X_WrByte(Dev, 0xFF, 0x00);
		Status |= VL53L0X_WrByte(Dev, 0x80, 0x00);
	}

	if ((Status == VL53L0X_ERROR_NONE) &&
		(ReadDataFromDeviceDone != 7)) {
		/* Assign to variable if status is ok */
		if (((option & 1) == 1) &&
			((ReadDataFromDeviceDone & 1) == 0)) {
			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
				ReferenceSpadCount, ReferenceSpadCount);

			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
				ReferenceSpadType, ReferenceSpadType);

			for (i = 0; i < VL53L0X_REF_SPAD_BUFFER_SIZE; i++) {
				Dev->Data.SpadData.RefGoodSpadMap[i] =
					NvmRefGoodSpadMap[i];
			}
		}

		if (((option & 2) == 2) &&
			((ReadDataFromDeviceDone & 2) == 0)) {
			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
					ModuleId, ModuleId);

			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
					Revision, Revision);

			ProductId_tmp = VL53L0X_GETDEVICESPECIFICPARAMETER(Dev,
					ProductId);
			u8 i = 0;
			while (ProductId[i] != '\0')
			{
				ProductId_tmp[i] = ProductId[i];
				i++;
			}
			ProductId_tmp[i] = ProductId[i];

		}

		if (((option & 4) == 4) &&
			((ReadDataFromDeviceDone & 4) == 0)) {
			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
						PartUIDUpper, PartUIDUpper);

			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
						PartUIDLower, PartUIDLower);

			SignalRateMeasFixed400mmFix =
				VL53L0X_FIXPOINT97TOFIXPOINT1616(
					SignalRateMeasFixed1104_400_mm);

			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
				SignalRateMeasFixed400mm,
				SignalRateMeasFixed400mmFix);

			OffsetMicroMeters = 0;
			if (DistMeasFixed1104_400_mm != 0) {
					OffsetFixed1104_mm =
						DistMeasFixed1104_400_mm -
						DistMeasTgtFixed1104_mm;
					OffsetMicroMeters = (OffsetFixed1104_mm
						* 1000) >> 4;
					OffsetMicroMeters *= -1;
			}

			PALDevDataSet(Dev,
				Part2PartOffsetAdjustmentNVMMicroMeter,
				OffsetMicroMeters);
		}
		byte = (u8)(ReadDataFromDeviceDone|option);
		VL53L0X_SETDEVICESPECIFICPARAMETER(Dev, ReadDataFromDeviceDone,
				byte);
	}

	return Status;
}


u32 VL53L0X_calc_macro_period_ps(VL53L0X_DEV Dev, u8 vcsel_period_pclks)
{
	uint64_t PLL_period_ps;
	u32 macro_period_vclks;
	u32 macro_period_ps;

	/* The above calculation will produce rounding errors,
	   therefore set fixed value
	*/
	PLL_period_ps = 1655;

	macro_period_vclks = 2304;
	macro_period_ps = (u32)(macro_period_vclks
			* vcsel_period_pclks * PLL_period_ps);

	return macro_period_ps;
}

u16 VL53L0X_encode_timeout(u32 timeout_macro_clks)
{
	/*!
	 * Encode timeout in macro periods in (LSByte * 2^MSByte) + 1 format
	 */

	u16 encoded_timeout = 0;
	u32 ls_byte = 0;
	u16 ms_byte = 0;

	if (timeout_macro_clks > 0) {
		ls_byte = timeout_macro_clks - 1;

		while ((ls_byte & 0xFFFFFF00) > 0) {
			ls_byte = ls_byte >> 1;
			ms_byte++;
		}

		encoded_timeout = (ms_byte << 8)
				+ (u16) (ls_byte & 0x000000FF);
	}

	return encoded_timeout;

}

u32 VL53L0X_decode_timeout(u16 encoded_timeout)
{
	/*!
	 * Decode 16-bit timeout register value - format (LSByte * 2^MSByte) + 1
	 */

	u32 timeout_macro_clks = 0;

	timeout_macro_clks = ((u32) (encoded_timeout & 0x00FF)
			<< (u32) ((encoded_timeout & 0xFF00) >> 8)) + 1;

	return timeout_macro_clks;
}


/* To convert ms into register value */
u32 VL53L0X_calc_timeout_mclks(VL53L0X_DEV Dev,
		u32 timeout_period_us,
		u8 vcsel_period_pclks)
{
	u32 macro_period_ps;
	u32 macro_period_ns;
	u32 timeout_period_mclks = 0;

	macro_period_ps = VL53L0X_calc_macro_period_ps(Dev, vcsel_period_pclks);
	macro_period_ns = (macro_period_ps + 500) / 1000;

	timeout_period_mclks =
		(u32) (((timeout_period_us * 1000)
		+ (macro_period_ns / 2)) / macro_period_ns);

    return timeout_period_mclks;
}

/* To convert register value into us */
u32 VL53L0X_calc_timeout_us(VL53L0X_DEV Dev,
		u16 timeout_period_mclks,
		u8 vcsel_period_pclks)
{
	u32 macro_period_ps;
	u32 macro_period_ns;
	u32 actual_timeout_period_us = 0;

	macro_period_ps = VL53L0X_calc_macro_period_ps(Dev, vcsel_period_pclks);
	macro_period_ns = (macro_period_ps + 500) / 1000;

	actual_timeout_period_us =
		((timeout_period_mclks * macro_period_ns) + 500) / 1000;

	return actual_timeout_period_us;
}


VL53L0X_Error get_sequence_step_timeout(VL53L0X_DEV Dev,
				VL53L0X_SequenceStepId SequenceStepId,
				u32 *pTimeOutMicroSecs)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 CurrentVCSELPulsePeriodPClk;
	u8 EncodedTimeOutByte = 0;
	u32 TimeoutMicroSeconds = 0;
	u16 PreRangeEncodedTimeOut = 0;
	u16 MsrcTimeOutMClks;
	u16 PreRangeTimeOutMClks;
	u16 FinalRangeTimeOutMClks = 0;
	u16 FinalRangeEncodedTimeOut;
	VL53L0X_SchedulerSequenceSteps_t SchedulerSequenceSteps;

	if ((SequenceStepId == VL53L0X_SEQUENCESTEP_TCC)	 ||
		(SequenceStepId == VL53L0X_SEQUENCESTEP_DSS)	 ||
		(SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)) {

		Status = VL53L0X_GetVcselPulsePeriod(Dev,
					VL53L0X_VCSEL_PERIOD_PRE_RANGE,
					&CurrentVCSELPulsePeriodPClk);
		if (Status == VL53L0X_ERROR_NONE) {
			Status = VL53L0X_RdByte(Dev,
					VL53L0X_REG_MSRC_CONFIG_TIMEOUT_MACROP,
					&EncodedTimeOutByte);
		}
		MsrcTimeOutMClks = VL53L0X_decode_timeout(EncodedTimeOutByte);

		TimeoutMicroSeconds = VL53L0X_calc_timeout_us(Dev,
						MsrcTimeOutMClks,
						CurrentVCSELPulsePeriodPClk);
	} else if (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE) {
		/* Retrieve PRE-RANGE VCSEL Period */
		Status = VL53L0X_GetVcselPulsePeriod(Dev,
						VL53L0X_VCSEL_PERIOD_PRE_RANGE,
						&CurrentVCSELPulsePeriodPClk);

		/* Retrieve PRE-RANGE Timeout in Macro periods (MCLKS) */
		if (Status == VL53L0X_ERROR_NONE) {

			/* Retrieve PRE-RANGE VCSEL Period */
			Status = VL53L0X_GetVcselPulsePeriod(Dev,
					VL53L0X_VCSEL_PERIOD_PRE_RANGE,
					&CurrentVCSELPulsePeriodPClk);

			if (Status == VL53L0X_ERROR_NONE) {
				Status = VL53L0X_RdWord(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
				&PreRangeEncodedTimeOut);
			}

			PreRangeTimeOutMClks = VL53L0X_decode_timeout(
					PreRangeEncodedTimeOut);

			TimeoutMicroSeconds = VL53L0X_calc_timeout_us(Dev,
					PreRangeTimeOutMClks,
					CurrentVCSELPulsePeriodPClk);
		}
	} else if (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE) {

		VL53L0X_GetSequenceStepEnables(Dev, &SchedulerSequenceSteps);
		PreRangeTimeOutMClks = 0;

		if (SchedulerSequenceSteps.PreRangeOn) {
			/* Retrieve PRE-RANGE VCSEL Period */
			Status = VL53L0X_GetVcselPulsePeriod(Dev,
				VL53L0X_VCSEL_PERIOD_PRE_RANGE,
				&CurrentVCSELPulsePeriodPClk);

			/* Retrieve PRE-RANGE Timeout in Macro periods
			 * (MCLKS) */
			if (Status == VL53L0X_ERROR_NONE) {
				Status = VL53L0X_RdWord(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
				&PreRangeEncodedTimeOut);
				PreRangeTimeOutMClks = VL53L0X_decode_timeout(
						PreRangeEncodedTimeOut);
			}
		}

		if (Status == VL53L0X_ERROR_NONE) {
			/* Retrieve FINAL-RANGE VCSEL Period */
			Status = VL53L0X_GetVcselPulsePeriod(Dev,
					VL53L0X_VCSEL_PERIOD_FINAL_RANGE,
					&CurrentVCSELPulsePeriodPClk);
		}

		/* Retrieve FINAL-RANGE Timeout in Macro periods (MCLKS) */
		if (Status == VL53L0X_ERROR_NONE) {
			Status = VL53L0X_RdWord(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI,
				&FinalRangeEncodedTimeOut);
			FinalRangeTimeOutMClks = VL53L0X_decode_timeout(
					FinalRangeEncodedTimeOut);
		}

		FinalRangeTimeOutMClks -= PreRangeTimeOutMClks;
		TimeoutMicroSeconds = VL53L0X_calc_timeout_us(Dev,
						FinalRangeTimeOutMClks,
						CurrentVCSELPulsePeriodPClk);
	}

	*pTimeOutMicroSecs = TimeoutMicroSeconds;

	return Status;
}


VL53L0X_Error set_sequence_step_timeout(VL53L0X_DEV Dev,
					VL53L0X_SequenceStepId SequenceStepId,
					u32 TimeOutMicroSecs)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 CurrentVCSELPulsePeriodPClk;
	u8 MsrcEncodedTimeOut;
	u16 PreRangeEncodedTimeOut;
	u16 PreRangeTimeOutMClks;
	u16 MsrcRangeTimeOutMClks;
	u32 FinalRangeTimeOutMClks;
	u16 FinalRangeEncodedTimeOut;
	VL53L0X_SchedulerSequenceSteps_t SchedulerSequenceSteps;

	if ((SequenceStepId == VL53L0X_SEQUENCESTEP_TCC)	 ||
		(SequenceStepId == VL53L0X_SEQUENCESTEP_DSS)	 ||
		(SequenceStepId == VL53L0X_SEQUENCESTEP_MSRC)) {

		Status = VL53L0X_GetVcselPulsePeriod(Dev,
					VL53L0X_VCSEL_PERIOD_PRE_RANGE,
					&CurrentVCSELPulsePeriodPClk);

		if (Status == VL53L0X_ERROR_NONE) {
			MsrcRangeTimeOutMClks = VL53L0X_calc_timeout_mclks(Dev,
					TimeOutMicroSecs,
					(u8)CurrentVCSELPulsePeriodPClk);

			if (MsrcRangeTimeOutMClks > 256)
				MsrcEncodedTimeOut = 255;
			else
				MsrcEncodedTimeOut =
					(u8)MsrcRangeTimeOutMClks - 1;

			VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
				LastEncodedTimeout,
				MsrcEncodedTimeOut);
		}

		if (Status == VL53L0X_ERROR_NONE) {
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_MSRC_CONFIG_TIMEOUT_MACROP,
				MsrcEncodedTimeOut);
		}
	} else {

		if (SequenceStepId == VL53L0X_SEQUENCESTEP_PRE_RANGE) {

			if (Status == VL53L0X_ERROR_NONE) {
				Status = VL53L0X_GetVcselPulsePeriod(Dev,
						VL53L0X_VCSEL_PERIOD_PRE_RANGE,
						&CurrentVCSELPulsePeriodPClk);
				PreRangeTimeOutMClks =
					VL53L0X_calc_timeout_mclks(Dev,
					TimeOutMicroSecs,
					(u8)CurrentVCSELPulsePeriodPClk);
				PreRangeEncodedTimeOut = VL53L0X_encode_timeout(
					PreRangeTimeOutMClks);

				VL53L0X_SETDEVICESPECIFICPARAMETER(Dev,
					LastEncodedTimeout,
					PreRangeEncodedTimeOut);
			}

			if (Status == VL53L0X_ERROR_NONE) {
				Status = VL53L0X_WrWord(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI,
				PreRangeEncodedTimeOut);
			}

			if (Status == VL53L0X_ERROR_NONE) {
				VL53L0X_SETDEVICESPECIFICPARAMETER(
					Dev,
					PreRangeTimeoutMicroSecs,
					TimeOutMicroSecs);
			}
		} else if (SequenceStepId == VL53L0X_SEQUENCESTEP_FINAL_RANGE) {

			/* For the final range timeout, the pre-range timeout
			 * must be added. To do this both final and pre-range
			 * timeouts must be expressed in macro periods MClks
			 * because they have different vcsel periods.
			 */

			VL53L0X_GetSequenceStepEnables(Dev,
					&SchedulerSequenceSteps);
			PreRangeTimeOutMClks = 0;
			if (SchedulerSequenceSteps.PreRangeOn) {

				/* Retrieve PRE-RANGE VCSEL Period */
				Status = VL53L0X_GetVcselPulsePeriod(Dev,
					VL53L0X_VCSEL_PERIOD_PRE_RANGE,
					&CurrentVCSELPulsePeriodPClk);

				/* Retrieve PRE-RANGE Timeout in Macro periods
				 * (MCLKS) */
				if (Status == VL53L0X_ERROR_NONE) {
					Status = VL53L0X_RdWord(Dev, 0x51,
						&PreRangeEncodedTimeOut);
					PreRangeTimeOutMClks =
						VL53L0X_decode_timeout(
							PreRangeEncodedTimeOut);
				}
			}

			/* Calculate FINAL RANGE Timeout in Macro Periods
			 * (MCLKS) and add PRE-RANGE value
			 */
			if (Status == VL53L0X_ERROR_NONE) {

				Status = VL53L0X_GetVcselPulsePeriod(Dev,
						VL53L0X_VCSEL_PERIOD_FINAL_RANGE,
						&CurrentVCSELPulsePeriodPClk);
			}
			if (Status == VL53L0X_ERROR_NONE) {

				FinalRangeTimeOutMClks =
					VL53L0X_calc_timeout_mclks(Dev,
					TimeOutMicroSecs,
					(u8) CurrentVCSELPulsePeriodPClk);

				FinalRangeTimeOutMClks += PreRangeTimeOutMClks;

				FinalRangeEncodedTimeOut =
				VL53L0X_encode_timeout(FinalRangeTimeOutMClks);

				if (Status == VL53L0X_ERROR_NONE) {
					Status = VL53L0X_WrWord(Dev, 0x71,
					FinalRangeEncodedTimeOut);
				}

				if (Status == VL53L0X_ERROR_NONE) {
					VL53L0X_SETDEVICESPECIFICPARAMETER(
						Dev,
						FinalRangeTimeoutMicroSecs,
						TimeOutMicroSecs);
				}
			}
		} else
			Status = VL53L0X_ERROR_INVALID_PARAMS;

	}
	return Status;
}

VL53L0X_Error VL53L0X_set_vcsel_pulse_period(VL53L0X_DEV Dev,
	VL53L0X_VcselPeriod VcselPeriodType, u8 VCSELPulsePeriodPCLK)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 vcsel_period_reg;
	u8 MinPreVcselPeriodPCLK = 12;
	u8 MaxPreVcselPeriodPCLK = 18;
	u8 MinFinalVcselPeriodPCLK = 8;
	u8 MaxFinalVcselPeriodPCLK = 14;
	u32 MeasurementTimingBudgetMicroSeconds;
	u32 FinalRangeTimeoutMicroSeconds;
	u32 PreRangeTimeoutMicroSeconds;
	u32 MsrcTimeoutMicroSeconds;
	u8 PhaseCalInt = 0;

	/* Check if valid clock period requested */

	if ((VCSELPulsePeriodPCLK % 2) != 0) {
		/* Value must be an even number */
		Status = VL53L0X_ERROR_INVALID_PARAMS;
	} else if (VcselPeriodType == VL53L0X_VCSEL_PERIOD_PRE_RANGE &&
		(VCSELPulsePeriodPCLK < MinPreVcselPeriodPCLK ||
		VCSELPulsePeriodPCLK > MaxPreVcselPeriodPCLK)) {
		Status = VL53L0X_ERROR_INVALID_PARAMS;
	} else if (VcselPeriodType == VL53L0X_VCSEL_PERIOD_FINAL_RANGE &&
		(VCSELPulsePeriodPCLK < MinFinalVcselPeriodPCLK ||
		 VCSELPulsePeriodPCLK > MaxFinalVcselPeriodPCLK)) {

		Status = VL53L0X_ERROR_INVALID_PARAMS;
	}

	/* Apply specific settings for the requested clock period */

	if (Status != VL53L0X_ERROR_NONE)
		return Status;


	if (VcselPeriodType == VL53L0X_VCSEL_PERIOD_PRE_RANGE) {

		/* Set phase check limits */
		if (VCSELPulsePeriodPCLK == 12) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x18);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);
		} else if (VCSELPulsePeriodPCLK == 14) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x30);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);
		} else if (VCSELPulsePeriodPCLK == 16) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x40);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);
		} else if (VCSELPulsePeriodPCLK == 18) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x50);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);
		}
	} else if (VcselPeriodType == VL53L0X_VCSEL_PERIOD_FINAL_RANGE) {

		if (VCSELPulsePeriodPCLK == 8) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x10);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);

			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x02);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x0C);

			Status |= VL53L0X_WrByte(Dev, 0xff, 0x01);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_LIM,
				0x30);
			Status |= VL53L0X_WrByte(Dev, 0xff, 0x00);
		} else if (VCSELPulsePeriodPCLK == 10) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x28);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);

			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x09);

			Status |= VL53L0X_WrByte(Dev, 0xff, 0x01);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_LIM,
				0x20);
			Status |= VL53L0X_WrByte(Dev, 0xff, 0x00);
		} else if (VCSELPulsePeriodPCLK == 12) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x38);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);

			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x08);

			Status |= VL53L0X_WrByte(Dev, 0xff, 0x01);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_LIM,
				0x20);
			Status |= VL53L0X_WrByte(Dev, 0xff, 0x00);
		} else if (VCSELPulsePeriodPCLK == 14) {

			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH,
				0x048);
			Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VALID_PHASE_LOW,
				0x08);

			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_GLOBAL_CONFIG_VCSEL_WIDTH, 0x03);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_CONFIG_TIMEOUT, 0x07);

			Status |= VL53L0X_WrByte(Dev, 0xff, 0x01);
			Status |= VL53L0X_WrByte(Dev,
				VL53L0X_REG_ALGO_PHASECAL_LIM,
				0x20);
			Status |= VL53L0X_WrByte(Dev, 0xff, 0x00);
		}
	}


	/* Re-calculate and apply timeouts, in macro periods */

	if (Status == VL53L0X_ERROR_NONE) {
		vcsel_period_reg = VL53L0X_encode_vcsel_period((u8)
			VCSELPulsePeriodPCLK);

		/* When the VCSEL period for the pre or final range is changed,
		* the corresponding timeout must be read from the device using
		* the current VCSEL period, then the new VCSEL period can be
		* applied. The timeout then must be written back to the device
		* using the new VCSEL period.
		*
		* For the MSRC timeout, the same applies - this timeout being
		* dependant on the pre-range vcsel period.
		*/
		switch (VcselPeriodType) {
		case VL53L0X_VCSEL_PERIOD_PRE_RANGE:
			Status = get_sequence_step_timeout(Dev,
				VL53L0X_SEQUENCESTEP_PRE_RANGE,
				&PreRangeTimeoutMicroSeconds);

			if (Status == VL53L0X_ERROR_NONE)
				Status = get_sequence_step_timeout(Dev,
					VL53L0X_SEQUENCESTEP_MSRC,
					&MsrcTimeoutMicroSeconds);

			if (Status == VL53L0X_ERROR_NONE)
				Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD,
					vcsel_period_reg);


			if (Status == VL53L0X_ERROR_NONE)
				Status = set_sequence_step_timeout(Dev,
					VL53L0X_SEQUENCESTEP_PRE_RANGE,
					PreRangeTimeoutMicroSeconds);


			if (Status == VL53L0X_ERROR_NONE)
				Status = set_sequence_step_timeout(Dev,
					VL53L0X_SEQUENCESTEP_MSRC,
					MsrcTimeoutMicroSeconds);

			VL53L0X_SETDEVICESPECIFICPARAMETER(
				Dev,
				PreRangeVcselPulsePeriod,
				VCSELPulsePeriodPCLK);
			break;
		case VL53L0X_VCSEL_PERIOD_FINAL_RANGE:
			Status = get_sequence_step_timeout(Dev,
				VL53L0X_SEQUENCESTEP_FINAL_RANGE,
				&FinalRangeTimeoutMicroSeconds);

			if (Status == VL53L0X_ERROR_NONE)
				Status = VL53L0X_WrByte(Dev,
				VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD,
					vcsel_period_reg);


			if (Status == VL53L0X_ERROR_NONE)
				Status = set_sequence_step_timeout(Dev,
					VL53L0X_SEQUENCESTEP_FINAL_RANGE,
					FinalRangeTimeoutMicroSeconds);

			VL53L0X_SETDEVICESPECIFICPARAMETER(
				Dev,
				FinalRangeVcselPulsePeriod,
				VCSELPulsePeriodPCLK);
			break;
		default:
			Status = VL53L0X_ERROR_INVALID_PARAMS;
		}
	}

	/* Finally, the timing budget must be re-applied */
	if (Status == VL53L0X_ERROR_NONE) {
		VL53L0X_GETPARAMETERFIELD(Dev,
			MeasurementTimingBudgetMicroSeconds,
			MeasurementTimingBudgetMicroSeconds);

		Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(Dev,
				MeasurementTimingBudgetMicroSeconds);
	}

	/* Perform the phase calibration. This is needed after changing on
	 * vcsel period.
	 * get_data_enable = 0, restore_config = 1 */
	if (Status == VL53L0X_ERROR_NONE)
		Status = VL53L0X_perform_phase_calibration(
			Dev, &PhaseCalInt, 0, 1);

	return Status;
}

VL53L0X_Error VL53L0X_get_vcsel_pulse_period(VL53L0X_DEV Dev,
	VL53L0X_VcselPeriod VcselPeriodType, u8 *pVCSELPulsePeriodPCLK)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 vcsel_period_reg;

	switch (VcselPeriodType) {
	case VL53L0X_VCSEL_PERIOD_PRE_RANGE:
		Status = VL53L0X_RdByte(Dev,
			VL53L0X_REG_PRE_RANGE_CONFIG_VCSEL_PERIOD,
			&vcsel_period_reg);
	break;
	case VL53L0X_VCSEL_PERIOD_FINAL_RANGE:
		Status = VL53L0X_RdByte(Dev,
			VL53L0X_REG_FINAL_RANGE_CONFIG_VCSEL_PERIOD,
			&vcsel_period_reg);
	break;
	default:
		Status = VL53L0X_ERROR_INVALID_PARAMS;
	}

	if (Status == VL53L0X_ERROR_NONE)
		*pVCSELPulsePeriodPCLK =
			VL53L0X_decode_vcsel_period(vcsel_period_reg);

	return Status;
}



VL53L0X_Error VL53L0X_set_measurement_timing_budget_micro_seconds(VL53L0X_DEV Dev,
		u32 MeasurementTimingBudgetMicroSeconds)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u32 FinalRangeTimingBudgetMicroSeconds;
	VL53L0X_SchedulerSequenceSteps_t SchedulerSequenceSteps;
	u32 MsrcDccTccTimeoutMicroSeconds	= 2000;
	u32 StartOverheadMicroSeconds		= 1910;
	u32 EndOverheadMicroSeconds		= 960;
	u32 MsrcOverheadMicroSeconds		= 660;
	u32 TccOverheadMicroSeconds		= 590;
	u32 DssOverheadMicroSeconds		= 690;
	u32 PreRangeOverheadMicroSeconds	= 660;
	u32 FinalRangeOverheadMicroSeconds = 550;
	u32 PreRangeTimeoutMicroSeconds	= 0;
	u32 cMinTimingBudgetMicroSeconds	= 20000;
	u32 SubTimeout = 0;

	if (MeasurementTimingBudgetMicroSeconds
			< cMinTimingBudgetMicroSeconds) {
		Status = VL53L0X_ERROR_INVALID_PARAMS;
		return Status;
	}

	FinalRangeTimingBudgetMicroSeconds =
		MeasurementTimingBudgetMicroSeconds -
		(StartOverheadMicroSeconds + EndOverheadMicroSeconds);

	Status = VL53L0X_GetSequenceStepEnables(Dev, &SchedulerSequenceSteps);

	if (Status == VL53L0X_ERROR_NONE &&
		(SchedulerSequenceSteps.TccOn  ||
		SchedulerSequenceSteps.MsrcOn ||
		SchedulerSequenceSteps.DssOn)) {

		/* TCC, MSRC and DSS all share the same timeout */
		Status = get_sequence_step_timeout(Dev,
					VL53L0X_SEQUENCESTEP_MSRC,
					&MsrcDccTccTimeoutMicroSeconds);

		/* Subtract the TCC, MSRC and DSS timeouts if they are
		 * enabled. */

		if (Status != VL53L0X_ERROR_NONE)
			return Status;

		/* TCC */
		if (SchedulerSequenceSteps.TccOn) {

			SubTimeout = MsrcDccTccTimeoutMicroSeconds
				+ TccOverheadMicroSeconds;

			if (SubTimeout <
				FinalRangeTimingBudgetMicroSeconds) {
				FinalRangeTimingBudgetMicroSeconds -=
							SubTimeout;
			} else {
				/* Requested timeout too big. */
				Status = VL53L0X_ERROR_INVALID_PARAMS;
			}
		}

		if (Status != VL53L0X_ERROR_NONE) {
			return Status;
		}

		/* DSS */
		if (SchedulerSequenceSteps.DssOn) {

			SubTimeout = 2 * (MsrcDccTccTimeoutMicroSeconds +
				DssOverheadMicroSeconds);

			if (SubTimeout < FinalRangeTimingBudgetMicroSeconds) {
				FinalRangeTimingBudgetMicroSeconds
							-= SubTimeout;
			} else {
				/* Requested timeout too big. */
				Status = VL53L0X_ERROR_INVALID_PARAMS;
			}
		} else if (SchedulerSequenceSteps.MsrcOn) {
			/* MSRC */
			SubTimeout = MsrcDccTccTimeoutMicroSeconds +
						MsrcOverheadMicroSeconds;

			if (SubTimeout < FinalRangeTimingBudgetMicroSeconds) {
				FinalRangeTimingBudgetMicroSeconds
							-= SubTimeout;
			} else {
				/* Requested timeout too big. */
				Status = VL53L0X_ERROR_INVALID_PARAMS;
			}
		}

	}

	if (Status != VL53L0X_ERROR_NONE) {
		return Status;
	}

	if (SchedulerSequenceSteps.PreRangeOn) {

		/* Subtract the Pre-range timeout if enabled. */

		Status = get_sequence_step_timeout(Dev,
				VL53L0X_SEQUENCESTEP_PRE_RANGE,
				&PreRangeTimeoutMicroSeconds);

		SubTimeout = PreRangeTimeoutMicroSeconds +
				PreRangeOverheadMicroSeconds;

		if (SubTimeout < FinalRangeTimingBudgetMicroSeconds) {
			FinalRangeTimingBudgetMicroSeconds -= SubTimeout;
		} else {
			/* Requested timeout too big. */
			Status = VL53L0X_ERROR_INVALID_PARAMS;
		}
	}


	if (Status == VL53L0X_ERROR_NONE &&
		SchedulerSequenceSteps.FinalRangeOn) {

		FinalRangeTimingBudgetMicroSeconds -=
				FinalRangeOverheadMicroSeconds;

		/* Final Range Timeout
		 * Note that the final range timeout is determined by the timing
		 * budget and the sum of all other timeouts within the sequence.
		 * If there is no room for the final range timeout, then an error
		 * will be set. Otherwise the remaining time will be applied to
		 * the final range.
		 */
		Status = set_sequence_step_timeout(Dev,
			VL53L0X_SEQUENCESTEP_FINAL_RANGE,
			FinalRangeTimingBudgetMicroSeconds);

		VL53L0X_SETPARAMETERFIELD(Dev,
			MeasurementTimingBudgetMicroSeconds,
			MeasurementTimingBudgetMicroSeconds);
	}

	return Status;
}

VL53L0X_Error VL53L0X_get_measurement_timing_budget_micro_seconds(VL53L0X_DEV Dev,
		u32 *pMeasurementTimingBudgetMicroSeconds)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	VL53L0X_SchedulerSequenceSteps_t SchedulerSequenceSteps;
	u32 FinalRangeTimeoutMicroSeconds;
	u32 MsrcDccTccTimeoutMicroSeconds	= 2000;
	u32 StartOverheadMicroSeconds		= 1910;
	u32 EndOverheadMicroSeconds		= 960;
	u32 MsrcOverheadMicroSeconds		= 660;
	u32 TccOverheadMicroSeconds		= 590;
	u32 DssOverheadMicroSeconds		= 690;
	u32 PreRangeOverheadMicroSeconds	= 660;
	u32 FinalRangeOverheadMicroSeconds = 550;
	u32 PreRangeTimeoutMicroSeconds	= 0;

	/* Start and end overhead times always present */
	*pMeasurementTimingBudgetMicroSeconds
		= StartOverheadMicroSeconds + EndOverheadMicroSeconds;

	Status = VL53L0X_GetSequenceStepEnables(Dev, &SchedulerSequenceSteps);

	if (Status != VL53L0X_ERROR_NONE) {
		return Status;
	}


	if (SchedulerSequenceSteps.TccOn  ||
		SchedulerSequenceSteps.MsrcOn ||
		SchedulerSequenceSteps.DssOn) {

		Status = get_sequence_step_timeout(Dev,
				VL53L0X_SEQUENCESTEP_MSRC,
				&MsrcDccTccTimeoutMicroSeconds);

		if (Status == VL53L0X_ERROR_NONE) {
			if (SchedulerSequenceSteps.TccOn) {
				*pMeasurementTimingBudgetMicroSeconds +=
					MsrcDccTccTimeoutMicroSeconds +
					TccOverheadMicroSeconds;
			}

			if (SchedulerSequenceSteps.DssOn) {
				*pMeasurementTimingBudgetMicroSeconds +=
				2 * (MsrcDccTccTimeoutMicroSeconds +
					DssOverheadMicroSeconds);
			} else if (SchedulerSequenceSteps.MsrcOn) {
				*pMeasurementTimingBudgetMicroSeconds +=
					MsrcDccTccTimeoutMicroSeconds +
					MsrcOverheadMicroSeconds;
			}
		}
	}

	if (Status == VL53L0X_ERROR_NONE) {
		if (SchedulerSequenceSteps.PreRangeOn) {
			Status = get_sequence_step_timeout(Dev,
				VL53L0X_SEQUENCESTEP_PRE_RANGE,
				&PreRangeTimeoutMicroSeconds);
			*pMeasurementTimingBudgetMicroSeconds +=
				PreRangeTimeoutMicroSeconds +
				PreRangeOverheadMicroSeconds;
		}
	}

	if (Status == VL53L0X_ERROR_NONE) {
		if (SchedulerSequenceSteps.FinalRangeOn) {
			Status = get_sequence_step_timeout(Dev,
					VL53L0X_SEQUENCESTEP_FINAL_RANGE,
					&FinalRangeTimeoutMicroSeconds);
			*pMeasurementTimingBudgetMicroSeconds +=
				(FinalRangeTimeoutMicroSeconds +
				FinalRangeOverheadMicroSeconds);
		}
	}

	if (Status == VL53L0X_ERROR_NONE) {
		VL53L0X_SETPARAMETERFIELD(Dev,
			MeasurementTimingBudgetMicroSeconds,
			*pMeasurementTimingBudgetMicroSeconds);
	}
	
	return Status;
}



VL53L0X_Error VL53L0X_load_tuning_settings(VL53L0X_DEV Dev,
		u8 *pTuningSettingBuffer)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	int i;
	int Index;
	u8 msb;
	u8 lsb;
	u8 SelectParam;
	u8 NumberOfWrites;
	u8 Address;
	u8 localBuffer[4]; /* max */
	u16 Temp16;

	Index = 0;

	while ((*(pTuningSettingBuffer + Index) != 0) &&
			(Status == VL53L0X_ERROR_NONE)) {
		NumberOfWrites = *(pTuningSettingBuffer + Index);
		Index++;
		if (NumberOfWrites == 0xFF) {
			/* internal parameters */
			SelectParam = *(pTuningSettingBuffer + Index);
			Index++;
			switch (SelectParam) {
			case 0: /* u16 SigmaEstRefArray -> 2 bytes */
				msb = *(pTuningSettingBuffer + Index);
				Index++;
				lsb = *(pTuningSettingBuffer + Index);
				Index++;
				Temp16 = VL53L0X_MAKEUINT16(lsb, msb);
				PALDevDataSet(Dev, SigmaEstRefArray, Temp16);
				break;
			case 1: /* u16 SigmaEstEffPulseWidth -> 2 bytes */
				msb = *(pTuningSettingBuffer + Index);
				Index++;
				lsb = *(pTuningSettingBuffer + Index);
				Index++;
				Temp16 = VL53L0X_MAKEUINT16(lsb, msb);
				PALDevDataSet(Dev, SigmaEstEffPulseWidth,
					Temp16);
				break;
			case 2: /* u16 SigmaEstEffAmbWidth -> 2 bytes */
				msb = *(pTuningSettingBuffer + Index);
				Index++;
				lsb = *(pTuningSettingBuffer + Index);
				Index++;
				Temp16 = VL53L0X_MAKEUINT16(lsb, msb);
				PALDevDataSet(Dev, SigmaEstEffAmbWidth, Temp16);
				break;
			case 3: /* u16 targetRefRate -> 2 bytes */
				msb = *(pTuningSettingBuffer + Index);
				Index++;
				lsb = *(pTuningSettingBuffer + Index);
				Index++;
				Temp16 = VL53L0X_MAKEUINT16(lsb, msb);
				PALDevDataSet(Dev, targetRefRate, Temp16);
				break;
			default: /* invalid parameter */
				Status = VL53L0X_ERROR_INVALID_PARAMS;
			}

		} else if (NumberOfWrites <= 4) {
			Address = *(pTuningSettingBuffer + Index);
			Index++;

			for (i = 0; i < NumberOfWrites; i++) {
				localBuffer[i] = *(pTuningSettingBuffer +
							Index);
				Index++;
			}

			Status = VL53L0X_WriteMulti(Dev, Address, localBuffer,
					NumberOfWrites);

		} else {
			Status = VL53L0X_ERROR_INVALID_PARAMS;
		}
	}

	return Status;
}

VL53L0X_Error VL53L0X_get_total_xtalk_rate(VL53L0X_DEV Dev,
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
	f1616 *ptotal_xtalk_rate_mcps)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;

	u8 xtalkCompEnable;
	f1616 totalXtalkMegaCps;
	f1616 xtalkPerSpadMegaCps;

	*ptotal_xtalk_rate_mcps = 0;

	Status = VL53L0X_GetXTalkCompensationEnable(Dev, &xtalkCompEnable);
	if (Status == VL53L0X_ERROR_NONE) {

		if (xtalkCompEnable) {

			VL53L0X_GETPARAMETERFIELD(
				Dev,
				XTalkCompensationRateMegaCps,
				xtalkPerSpadMegaCps);

			/* FixPoint1616 * FixPoint 8:8 = FixPoint0824 */
			totalXtalkMegaCps =
				pRangingMeasurementData->EffectiveSpadRtnCount *
				xtalkPerSpadMegaCps;

			/* FixPoint0824 >> 8 = FixPoint1616 */
			*ptotal_xtalk_rate_mcps =
				(totalXtalkMegaCps + 0x80) >> 8;
		}
	}

	return Status;
}

VL53L0X_Error VL53L0X_get_total_signal_rate(VL53L0X_DEV Dev,
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
	f1616 *ptotal_signal_rate_mcps)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	f1616 totalXtalkMegaCps;

	*ptotal_signal_rate_mcps =
		pRangingMeasurementData->SignalRateRtnMegaCps;

	Status = VL53L0X_get_total_xtalk_rate(
		Dev, pRangingMeasurementData, &totalXtalkMegaCps);

	if (Status == VL53L0X_ERROR_NONE)
		*ptotal_signal_rate_mcps += totalXtalkMegaCps;

	return Status;
}

VL53L0X_Error VL53L0X_calc_dmax(
	VL53L0X_DEV Dev,
	f1616 totalSignalRate_mcps,
	f1616 totalCorrSignalRate_mcps,
	f1616 pwMult,
	u32 sigmaEstimateP1,
	f1616 sigmaEstimateP2,
	u32 peakVcselDuration_us,
	u32 *pdmax_mm)
{
	const u32 cSigmaLimit		= 18;
	const f1616 cSignalLimit	= 0x4000; /* 0.25 */
	const f1616 cSigmaEstRef	= 0x00000042; /* 0.001 */
	const u32 cAmbEffWidthSigmaEst_ns = 6;
	const u32 cAmbEffWidthDMax_ns	   = 7;
	u32 dmaxCalRange_mm;
	f1616 dmaxCalSignalRateRtn_mcps;
	f1616 minSignalNeeded;
	f1616 minSignalNeeded_p1;
	f1616 minSignalNeeded_p2;
	f1616 minSignalNeeded_p3;
	f1616 minSignalNeeded_p4;
	f1616 sigmaLimitTmp;
	f1616 sigmaEstSqTmp;
	f1616 signalLimitTmp;
	f1616 SignalAt0mm;
	f1616 dmaxDark;
	f1616 dmaxAmbient;
	f1616 dmaxDarkTmp;
	f1616 sigmaEstP2Tmp;
	u32 signalRateTemp_mcps;

	VL53L0X_Error Status = VL53L0X_ERROR_NONE;

	dmaxCalRange_mm =
		PALDevDataGet(Dev, DmaxCalRangeMilliMeter);

	dmaxCalSignalRateRtn_mcps =
		PALDevDataGet(Dev, DmaxCalSignalRateRtnMegaCps);

	/* uint32 * FixPoint1616 = FixPoint1616 */
	SignalAt0mm = dmaxCalRange_mm * dmaxCalSignalRateRtn_mcps;

	/* FixPoint1616 >> 8 = FixPoint2408 */
	SignalAt0mm = (SignalAt0mm + 0x80) >> 8;
	SignalAt0mm *= dmaxCalRange_mm;

	minSignalNeeded_p1 = 0;
	if (totalCorrSignalRate_mcps > 0) {

		/* Shift by 10 bits to increase resolution prior to the
		 * division */
		signalRateTemp_mcps = totalSignalRate_mcps << 10;

		/* Add rounding value prior to division */
		minSignalNeeded_p1 = signalRateTemp_mcps +
			(totalCorrSignalRate_mcps/2);

		/* FixPoint0626/FixPoint1616 = FixPoint2210 */
		minSignalNeeded_p1 /= totalCorrSignalRate_mcps;

		/* Apply a factored version of the speed of light.
		 Correction to be applied at the end */
		minSignalNeeded_p1 *= 3;

		/* FixPoint2210 * FixPoint2210 = FixPoint1220 */
		minSignalNeeded_p1 *= minSignalNeeded_p1;

		/* FixPoint1220 >> 16 = FixPoint2804 */
		minSignalNeeded_p1 = (minSignalNeeded_p1 + 0x8000) >> 16;
	}

	minSignalNeeded_p2 = pwMult * sigmaEstimateP1;

	/* FixPoint1616 >> 16 =	 uint32 */
	minSignalNeeded_p2 = (minSignalNeeded_p2 + 0x8000) >> 16;

	/* uint32 * uint32	=  uint32 */
	minSignalNeeded_p2 *= minSignalNeeded_p2;

	/* Check sigmaEstimateP2
	 * If this value is too high there is not enough signal rate
	 * to calculate dmax value so set a suitable value to ensure
	 * a very small dmax.
	 */
	sigmaEstP2Tmp = (sigmaEstimateP2 + 0x8000) >> 16;
	sigmaEstP2Tmp = (sigmaEstP2Tmp + cAmbEffWidthSigmaEst_ns/2)/
		cAmbEffWidthSigmaEst_ns;
	sigmaEstP2Tmp *= cAmbEffWidthDMax_ns;

	if (sigmaEstP2Tmp > 0xffff) {
		minSignalNeeded_p3 = 0xfff00000;
	} else {

		/* DMAX uses a different ambient width from sigma, so apply
		 * correction.
		 * Perform division before multiplication to prevent overflow.
		 */
		sigmaEstimateP2 = (sigmaEstimateP2 + cAmbEffWidthSigmaEst_ns/2)/
			cAmbEffWidthSigmaEst_ns;
		sigmaEstimateP2 *= cAmbEffWidthDMax_ns;

		/* FixPoint1616 >> 16 = uint32 */
		minSignalNeeded_p3 = (sigmaEstimateP2 + 0x8000) >> 16;

		minSignalNeeded_p3 *= minSignalNeeded_p3;

	}

	/* FixPoint1814 / uint32 = FixPoint1814 */
	sigmaLimitTmp = ((cSigmaLimit << 14) + 500) / 1000;

	/* FixPoint1814 * FixPoint1814 = FixPoint3628 := FixPoint0428 */
	sigmaLimitTmp *= sigmaLimitTmp;

	/* FixPoint1616 * FixPoint1616 = FixPoint3232 */
	sigmaEstSqTmp = cSigmaEstRef * cSigmaEstRef;

	/* FixPoint3232 >> 4 = FixPoint0428 */
	sigmaEstSqTmp = (sigmaEstSqTmp + 0x08) >> 4;

	/* FixPoint0428 - FixPoint0428	= FixPoint0428 */
	sigmaLimitTmp -=  sigmaEstSqTmp;

	/* u32 * FixPoint0428 = FixPoint0428 */
	minSignalNeeded_p4 = 4 * 12 * sigmaLimitTmp;

	/* FixPoint0428 >> 14 = FixPoint1814 */
	minSignalNeeded_p4 = (minSignalNeeded_p4 + 0x2000) >> 14;

	/* uint32 + uint32 = uint32 */
	minSignalNeeded = (minSignalNeeded_p2 + minSignalNeeded_p3);

	/* uint32 / uint32 = uint32 */
	minSignalNeeded += (peakVcselDuration_us/2);
	minSignalNeeded /= peakVcselDuration_us;

	/* uint32 << 14 = FixPoint1814 */
	minSignalNeeded <<= 14;

	/* FixPoint1814 / FixPoint1814 = uint32 */
	minSignalNeeded += (minSignalNeeded_p4/2);
	minSignalNeeded /= minSignalNeeded_p4;

	/* FixPoint3200 * FixPoint2804 := FixPoint2804*/
	minSignalNeeded *= minSignalNeeded_p1;

	/* Apply correction by dividing by 1000000.
	 * This assumes 10E16 on the numerator of the equation
	 * and 10E-22 on the denominator.
	 * We do this because 32bit fix point calculation can't
	 * handle the larger and smaller elements of this equation,
	 * i.e. speed of light and pulse widths.
	 */
	minSignalNeeded = (minSignalNeeded + 500) / 1000;
	minSignalNeeded <<= 4;

	minSignalNeeded = (minSignalNeeded + 500) / 1000;

	/* FixPoint1616 >> 8 = FixPoint2408 */
	signalLimitTmp = (cSignalLimit + 0x80) >> 8;

	/* FixPoint2408/FixPoint2408 = uint32 */
	if (signalLimitTmp != 0)
		dmaxDarkTmp = (SignalAt0mm + (signalLimitTmp / 2))
			/ signalLimitTmp;
	else
		dmaxDarkTmp = 0;

	dmaxDark = VL53L0X_isqrt(dmaxDarkTmp);

	/* FixPoint2408/FixPoint2408 = uint32 */
	if (minSignalNeeded != 0)
		dmaxAmbient = (SignalAt0mm + minSignalNeeded/2)
			/ minSignalNeeded;
	else
		dmaxAmbient = 0;

	dmaxAmbient = VL53L0X_isqrt(dmaxAmbient);

	*pdmax_mm = dmaxDark;
	if (dmaxDark > dmaxAmbient)
		*pdmax_mm = dmaxAmbient;

	return Status;
}


VL53L0X_Error VL53L0X_calc_sigma_estimate(VL53L0X_DEV Dev,
	VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
	f1616 *pSigmaEstimate,
	u32 *pDmax_mm)
{
	/* Expressed in 100ths of a ns, i.e. centi-ns */
	const u32 cPulseEffectiveWidth_centi_ns   = 800;
	/* Expressed in 100ths of a ns, i.e. centi-ns */
	const u32 cAmbientEffectiveWidth_centi_ns = 600;
	const f1616 cDfltFinalRangeIntegrationTimeMilliSecs	= 0x00190000; /* 25ms */
	const u32 cVcselPulseWidth_ps	= 4700; /* pico secs */
	const f1616 cSigmaEstMax	= 0x028F87AE;
	const f1616 cSigmaEstRtnMax	= 0xF000;
	const f1616 cAmbToSignalRatioMax = 0xF0000000/
		cAmbientEffectiveWidth_centi_ns;
	/* Time Of Flight per mm (6.6 pico secs) */
	const f1616 cTOF_per_mm_ps		= 0x0006999A;
	const u32 c16BitRoundingParam		= 0x00008000;
	const f1616 cMaxXTalk_kcps		= 0x00320000;
	const u32 cPllPeriod_ps			= 1655;

	u32 vcselTotalEventsRtn;
	u32 finalRangeTimeoutMicroSecs;
	u32 preRangeTimeoutMicroSecs;
	u32 finalRangeIntegrationTimeMilliSecs;
	f1616 sigmaEstimateP1;
	f1616 sigmaEstimateP2;
	f1616 sigmaEstimateP3;
	f1616 deltaT_ps;
	f1616 pwMult;
	f1616 sigmaEstRtn;
	f1616 sigmaEstimate;
	f1616 xTalkCorrection;
	f1616 ambientRate_kcps;
	f1616 peakSignalRate_kcps;
	f1616 xTalkCompRate_mcps;
	u32 xTalkCompRate_kcps;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	f1616 diff1_mcps;
	f1616 diff2_mcps;
	f1616 sqr1;
	f1616 sqr2;
	f1616 sqrSum;
	f1616 sqrtResult_centi_ns;
	f1616 sqrtResult;
	f1616 totalSignalRate_mcps;
	f1616 correctedSignalRate_mcps;
	f1616 sigmaEstRef;
	u32 vcselWidth;
	u32 finalRangeMacroPCLKS;
	u32 preRangeMacroPCLKS;
	u32 peakVcselDuration_us;
	u8 finalRangeVcselPCLKS;
	u8 preRangeVcselPCLKS;
	/*! \addtogroup calc_sigma_estimate
	 * @{
	 *
	 * Estimates the range sigma
	 */

	VL53L0X_GETPARAMETERFIELD(Dev, XTalkCompensationRateMegaCps,
			xTalkCompRate_mcps);

	/*
	 * We work in kcps rather than mcps as this helps keep within the
	 * confines of the 32 Fix1616 type.
	 */

	ambientRate_kcps =
		(pRangingMeasurementData->AmbientRateRtnMegaCps * 1000) >> 16;

	correctedSignalRate_mcps =
		pRangingMeasurementData->SignalRateRtnMegaCps;


	Status = VL53L0X_get_total_signal_rate(
		Dev, pRangingMeasurementData, &totalSignalRate_mcps);
	Status = VL53L0X_get_total_xtalk_rate(
		Dev, pRangingMeasurementData, &xTalkCompRate_mcps);


	/* Signal rate measurement provided by device is the
	 * peak signal rate, not average.
	 */
	peakSignalRate_kcps = (totalSignalRate_mcps * 1000);
	peakSignalRate_kcps = (peakSignalRate_kcps + 0x8000) >> 16;

	xTalkCompRate_kcps = xTalkCompRate_mcps * 1000;

	if (xTalkCompRate_kcps > cMaxXTalk_kcps)
		xTalkCompRate_kcps = cMaxXTalk_kcps;

	if (Status == VL53L0X_ERROR_NONE) {

		/* Calculate final range macro periods */
		finalRangeTimeoutMicroSecs = VL53L0X_GETDEVICESPECIFICPARAMETER(
			Dev, FinalRangeTimeoutMicroSecs);

		finalRangeVcselPCLKS = VL53L0X_GETDEVICESPECIFICPARAMETER(
			Dev, FinalRangeVcselPulsePeriod);

		finalRangeMacroPCLKS = VL53L0X_calc_timeout_mclks(
			Dev, finalRangeTimeoutMicroSecs, finalRangeVcselPCLKS);

		/* Calculate pre-range macro periods */
		preRangeTimeoutMicroSecs = VL53L0X_GETDEVICESPECIFICPARAMETER(
			Dev, PreRangeTimeoutMicroSecs);

		preRangeVcselPCLKS = VL53L0X_GETDEVICESPECIFICPARAMETER(
			Dev, PreRangeVcselPulsePeriod);

		preRangeMacroPCLKS = VL53L0X_calc_timeout_mclks(
			Dev, preRangeTimeoutMicroSecs, preRangeVcselPCLKS);

		vcselWidth = 3;
		if (finalRangeVcselPCLKS == 8)
			vcselWidth = 2;


		peakVcselDuration_us = vcselWidth * 2048 *
			(preRangeMacroPCLKS + finalRangeMacroPCLKS);
		peakVcselDuration_us = (peakVcselDuration_us + 500)/1000;
		peakVcselDuration_us *= cPllPeriod_ps;
		peakVcselDuration_us = (peakVcselDuration_us + 500)/1000;

		/* Fix1616 >> 8 = Fix2408 */
		totalSignalRate_mcps = (totalSignalRate_mcps + 0x80) >> 8;

		/* Fix2408 * uint32 = Fix2408 */
		vcselTotalEventsRtn = totalSignalRate_mcps *
			peakVcselDuration_us;

		/* Fix2408 >> 8 = uint32 */
		vcselTotalEventsRtn = (vcselTotalEventsRtn + 0x80) >> 8;

		/* Fix2408 << 8 = Fix1616 = */
		totalSignalRate_mcps <<= 8;
	}

	if (Status != VL53L0X_ERROR_NONE) {
		return Status;
	}

	if (peakSignalRate_kcps == 0) {
		*pSigmaEstimate = cSigmaEstMax;
		PALDevDataSet(Dev, SigmaEstimate, cSigmaEstMax);
		*pDmax_mm = 0;
	} else {
		if (vcselTotalEventsRtn < 1)
			vcselTotalEventsRtn = 1;

		sigmaEstimateP1 = cPulseEffectiveWidth_centi_ns;

		/* ((FixPoint1616 << 16)* uint32)/uint32 = FixPoint1616 */
		sigmaEstimateP2 = (ambientRate_kcps << 16)/peakSignalRate_kcps;
		if (sigmaEstimateP2 > cAmbToSignalRatioMax) {
			/* Clip to prevent overflow. Will ensure safe
			 * max result. */
			sigmaEstimateP2 = cAmbToSignalRatioMax;
		}
		sigmaEstimateP2 *= cAmbientEffectiveWidth_centi_ns;

		sigmaEstimateP3 = 2 * VL53L0X_isqrt(vcselTotalEventsRtn * 12);

		/* uint32 * FixPoint1616 = FixPoint1616 */
		deltaT_ps = pRangingMeasurementData->RangeMilliMeter *
					cTOF_per_mm_ps;

		/*
		 * vcselRate - xtalkCompRate
		 * (uint32 << 16) - FixPoint1616 = FixPoint1616.
		 * Divide result by 1000 to convert to mcps.
		 * 500 is added to ensure rounding when integer division
		 * truncates.
		 */
		diff1_mcps = (((peakSignalRate_kcps << 16) -
			2 * xTalkCompRate_kcps) + 500)/1000;

		/* vcselRate + xtalkCompRate */
		diff2_mcps = ((peakSignalRate_kcps << 16) + 500)/1000;

		/* Shift by 8 bits to increase resolution prior to the
		 * division */
		diff1_mcps <<= 8;

		/* FixPoint0824/FixPoint1616 = FixPoint2408 */
		xTalkCorrection	 = abs(diff1_mcps/diff2_mcps);

		/* FixPoint2408 << 8 = FixPoint1616 */
		xTalkCorrection <<= 8;

		if(pRangingMeasurementData->RangeStatus != 0){
			pwMult = 1ul << 16;
		} else {
			/* FixPoint1616/uint32 = FixPoint1616 */
			pwMult = deltaT_ps/cVcselPulseWidth_ps; /* smaller than 1.0f */

			/*
			 * FixPoint1616 * FixPoint1616 = FixPoint3232, however both
			 * values are small enough such that32 bits will not be
			 * exceeded.
			 */
			pwMult *= ((1ul << 16) - xTalkCorrection);

			/* (FixPoint3232 >> 16) = FixPoint1616 */
			pwMult =  (pwMult + c16BitRoundingParam) >> 16;

			/* FixPoint1616 + FixPoint1616 = FixPoint1616 */
			pwMult += (1ul << 16);

			/*
			 * At this point the value will be 1.xx, therefore if we square
			 * the value this will exceed 32 bits. To address this perform
			 * a single shift to the right before the multiplication.
			 */
			pwMult >>= 1;
			/* FixPoint1715 * FixPoint1715 = FixPoint3430 */
			pwMult = pwMult * pwMult;

			/* (FixPoint3430 >> 14) = Fix1616 */
			pwMult >>= 14;
		}

		/* FixPoint1616 * uint32 = FixPoint1616 */
		sqr1 = pwMult * sigmaEstimateP1;

		/* (FixPoint1616 >> 16) = FixPoint3200 */
		sqr1 = (sqr1 + 0x8000) >> 16;

		/* FixPoint3200 * FixPoint3200 = FixPoint6400 */
		sqr1 *= sqr1;

		sqr2 = sigmaEstimateP2;

		/* (FixPoint1616 >> 16) = FixPoint3200 */
		sqr2 = (sqr2 + 0x8000) >> 16;

		/* FixPoint3200 * FixPoint3200 = FixPoint6400 */
		sqr2 *= sqr2;

		/* FixPoint64000 + FixPoint6400 = FixPoint6400 */
		sqrSum = sqr1 + sqr2;

		/* SQRT(FixPoin6400) = FixPoint3200 */
		sqrtResult_centi_ns = VL53L0X_isqrt(sqrSum);

		/* (FixPoint3200 << 16) = FixPoint1616 */
		sqrtResult_centi_ns <<= 16;

		/*
		 * Note that the Speed Of Light is expressed in um per 1E-10
		 * seconds (2997) Therefore to get mm/ns we have to divide by
		 * 10000
		 */
		sigmaEstRtn = (((sqrtResult_centi_ns+50)/100) /
				sigmaEstimateP3);
		sigmaEstRtn		 *= VL53L0X_SPEED_OF_LIGHT_IN_AIR;

		/* Add 5000 before dividing by 10000 to ensure rounding. */
		sigmaEstRtn		 += 5000;
		sigmaEstRtn		 /= 10000;

		if (sigmaEstRtn > cSigmaEstRtnMax) {
			/* Clip to prevent overflow. Will ensure safe
			 * max result. */
			sigmaEstRtn = cSigmaEstRtnMax;
		}
		finalRangeIntegrationTimeMilliSecs =
			(finalRangeTimeoutMicroSecs + preRangeTimeoutMicroSecs + 500)/1000;

		/* sigmaEstRef = 1mm * 25ms/final range integration time (inc pre-range)
		 * sqrt(FixPoint1616/int) = FixPoint2408)
		 */
		sigmaEstRef =
			VL53L0X_isqrt((cDfltFinalRangeIntegrationTimeMilliSecs +
				finalRangeIntegrationTimeMilliSecs/2)/
				finalRangeIntegrationTimeMilliSecs);

		/* FixPoint2408 << 8 = FixPoint1616 */
		sigmaEstRef <<= 8;
		sigmaEstRef = (sigmaEstRef + 500)/1000;

		/* FixPoint1616 * FixPoint1616 = FixPoint3232 */
		sqr1 = sigmaEstRtn * sigmaEstRtn;
		/* FixPoint1616 * FixPoint1616 = FixPoint3232 */
		sqr2 = sigmaEstRef * sigmaEstRef;

		/* sqrt(FixPoint3232) = FixPoint1616 */
		sqrtResult = VL53L0X_isqrt((sqr1 + sqr2));
		/*
		 * Note that the Shift by 4 bits increases resolution prior to
		 * the sqrt, therefore the result must be shifted by 2 bits to
		 * the right to revert back to the FixPoint1616 format.
		 */

		sigmaEstimate	 = 1000 * sqrtResult;

		if ((peakSignalRate_kcps < 1) || (vcselTotalEventsRtn < 1) ||
				(sigmaEstimate > cSigmaEstMax)) {
				sigmaEstimate = cSigmaEstMax;
		}

		*pSigmaEstimate = (u32)(sigmaEstimate);
		PALDevDataSet(Dev, SigmaEstimate, *pSigmaEstimate);
		Status = VL53L0X_calc_dmax(
			Dev,
			totalSignalRate_mcps,
			correctedSignalRate_mcps,
			pwMult,
			sigmaEstimateP1,
			sigmaEstimateP2,
			peakVcselDuration_us,
			pDmax_mm);
	}

	return Status;
}

VL53L0X_Error VL53L0X_get_pal_range_status(VL53L0X_DEV Dev,
		u8 DeviceRangeStatus,
		f1616 SignalRate,
		u16 EffectiveSpadRtnCount,
		VL53L0X_RangingMeasurementData_t *pRangingMeasurementData,
		u8 *pPalRangeStatus)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 NoneFlag;
	u8 SigmaLimitflag = 0;
	u8 SignalRefClipflag = 0;
	u8 RangeIgnoreThresholdflag = 0;
	u8 SigmaLimitCheckEnable = 0;
	u8 SignalRateFinalRangeLimitCheckEnable = 0;
	u8 SignalRefClipLimitCheckEnable = 0;
	u8 RangeIgnoreThresholdLimitCheckEnable = 0;
	f1616 SigmaEstimate;
	f1616 SigmaLimitValue;
	f1616 SignalRefClipValue;
	f1616 RangeIgnoreThresholdValue;
	f1616 SignalRatePerSpad;
	u8 DeviceRangeStatusInternal = 0;
	u16 tmpWord = 0;
	u8 Temp8;
	u32 Dmax_mm = 0;
	f1616 LastSignalRefMcps;


	/*
	 * VL53L0X has a good ranging when the value of the
	 * DeviceRangeStatus = 11. This function will replace the value 0 with
	 * the value 11 in the DeviceRangeStatus.
	 * In addition, the SigmaEstimator is not included in the VL53L0X
	 * DeviceRangeStatus, this will be added in the PalRangeStatus.
	 */

	DeviceRangeStatusInternal = ((DeviceRangeStatus & 0x78) >> 3);

	if (DeviceRangeStatusInternal == 0 ||
		DeviceRangeStatusInternal == 5 ||
		DeviceRangeStatusInternal == 7 ||
		DeviceRangeStatusInternal == 12 ||
		DeviceRangeStatusInternal == 13 ||
		DeviceRangeStatusInternal == 14 ||
		DeviceRangeStatusInternal == 15
			) {
		NoneFlag = 1;
	} else {
		NoneFlag = 0;
	}

	/*
	 * Check if Sigma limit is enabled, if yes then do comparison with limit
	 * value and put the result back into pPalRangeStatus.
	 */
	if (Status == VL53L0X_ERROR_NONE)
		Status =  VL53L0X_GetLimitCheckEnable(Dev,
			VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
			&SigmaLimitCheckEnable);

	if ((SigmaLimitCheckEnable != 0) && (Status == VL53L0X_ERROR_NONE)) {
		/*
		* compute the Sigma and check with limit
		*/
		Status = VL53L0X_calc_sigma_estimate(
			Dev,
			pRangingMeasurementData,
			&SigmaEstimate,
			&Dmax_mm);
		if (Status == VL53L0X_ERROR_NONE)
			pRangingMeasurementData->RangeDMaxMilliMeter = Dmax_mm;

		if (Status == VL53L0X_ERROR_NONE) {
			Status = VL53L0X_GetLimitCheckValue(Dev,
				VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
				&SigmaLimitValue);

			if ((SigmaLimitValue > 0) &&
				(SigmaEstimate > SigmaLimitValue))
					/* Limit Fail */
					SigmaLimitflag = 1;
		}
	}

	/*
	 * Check if Signal ref clip limit is enabled, if yes then do comparison
	 * with limit value and put the result back into pPalRangeStatus.
	 */
	if (Status == VL53L0X_ERROR_NONE)
		Status =  VL53L0X_GetLimitCheckEnable(Dev,
				VL53L0X_CHECKENABLE_SIGNAL_REF_CLIP,
				&SignalRefClipLimitCheckEnable);

	if ((SignalRefClipLimitCheckEnable != 0) &&
			(Status == VL53L0X_ERROR_NONE)) {

		Status = VL53L0X_GetLimitCheckValue(Dev,
				VL53L0X_CHECKENABLE_SIGNAL_REF_CLIP,
				&SignalRefClipValue);

		/* Read LastSignalRefMcps from device */
		if (Status == VL53L0X_ERROR_NONE)
			Status = VL53L0X_WrByte(Dev, 0xFF, 0x01);

		if (Status == VL53L0X_ERROR_NONE)
			Status = VL53L0X_RdWord(Dev,
				VL53L0X_REG_RESULT_PEAK_SIGNAL_RATE_REF,
				&tmpWord);

		if (Status == VL53L0X_ERROR_NONE)
			Status = VL53L0X_WrByte(Dev, 0xFF, 0x00);

		LastSignalRefMcps = VL53L0X_FIXPOINT97TOFIXPOINT1616(tmpWord);
		PALDevDataSet(Dev, LastSignalRefMcps, LastSignalRefMcps);

		if ((SignalRefClipValue > 0) &&
				(LastSignalRefMcps > SignalRefClipValue)) {
			/* Limit Fail */
			SignalRefClipflag = 1;
		}
	}

	/*
	 * Check if Signal ref clip limit is enabled, if yes then do comparison
	 * with limit value and put the result back into pPalRangeStatus.
	 * EffectiveSpadRtnCount has a format 8.8
	 * If (Return signal rate < (1.5 x Xtalk x number of Spads)) : FAIL
	 */
	if (Status == VL53L0X_ERROR_NONE)
		Status =  VL53L0X_GetLimitCheckEnable(Dev,
				VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
				&RangeIgnoreThresholdLimitCheckEnable);

	if ((RangeIgnoreThresholdLimitCheckEnable != 0) &&
			(Status == VL53L0X_ERROR_NONE)) {

		/* Compute the signal rate per spad */
		if (EffectiveSpadRtnCount == 0) {
			SignalRatePerSpad = 0;
		} else {
			SignalRatePerSpad = (f1616)((256 * SignalRate)
				/ EffectiveSpadRtnCount);
		}

		Status = VL53L0X_GetLimitCheckValue(Dev,
				VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
				&RangeIgnoreThresholdValue);

		if ((RangeIgnoreThresholdValue > 0) &&
			(SignalRatePerSpad < RangeIgnoreThresholdValue)) {
			/* Limit Fail add 2^6 to range status */
			RangeIgnoreThresholdflag = 1;
		}
	}

	if (Status == VL53L0X_ERROR_NONE) {
		if (NoneFlag == 1) {
			*pPalRangeStatus = 255;	 /* NONE */
		} else if (DeviceRangeStatusInternal == 1 ||
					DeviceRangeStatusInternal == 2 ||
					DeviceRangeStatusInternal == 3) {
			*pPalRangeStatus = 5; /* HW fail */
		} else if (DeviceRangeStatusInternal == 6 ||
					DeviceRangeStatusInternal == 9) {
			*pPalRangeStatus = 4;  /* Phase fail */
		} else if (DeviceRangeStatusInternal == 8 ||
					DeviceRangeStatusInternal == 10 ||
					SignalRefClipflag == 1) {
			*pPalRangeStatus = 3;  /* Min range */
		} else if (DeviceRangeStatusInternal == 4 ||
					RangeIgnoreThresholdflag == 1) {
			*pPalRangeStatus = 2;  /* Signal Fail */
		} else if (SigmaLimitflag == 1) {
			*pPalRangeStatus = 1;  /* Sigma	 Fail */
		} else {
			*pPalRangeStatus = 0; /* Range Valid */
		}
	}

	/* DMAX only relevant during range error */
	if (*pPalRangeStatus == 0)
		pRangingMeasurementData->RangeDMaxMilliMeter = 0;

	/* fill the Limit Check Status */

	Status =  VL53L0X_GetLimitCheckEnable(Dev,
			VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
			&SignalRateFinalRangeLimitCheckEnable);

	if (Status == VL53L0X_ERROR_NONE) {
		if ((SigmaLimitCheckEnable == 0) || (SigmaLimitflag == 1))
			Temp8 = 1;
		else
			Temp8 = 0;
		VL53L0X_SETARRAYPARAMETERFIELD(Dev, LimitChecksStatus,
				VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, Temp8);

		if ((DeviceRangeStatusInternal == 4) ||
				(SignalRateFinalRangeLimitCheckEnable == 0))
			Temp8 = 1;
		else
			Temp8 = 0;
		VL53L0X_SETARRAYPARAMETERFIELD(Dev, LimitChecksStatus,
				VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
				Temp8);

		if ((SignalRefClipLimitCheckEnable == 0) ||
					(SignalRefClipflag == 1))
			Temp8 = 1;
		else
			Temp8 = 0;

		VL53L0X_SETARRAYPARAMETERFIELD(Dev, LimitChecksStatus,
				VL53L0X_CHECKENABLE_SIGNAL_REF_CLIP, Temp8);

		if ((RangeIgnoreThresholdLimitCheckEnable == 0) ||
				(RangeIgnoreThresholdflag == 1))
			Temp8 = 1;
		else
			Temp8 = 0;

		VL53L0X_SETARRAYPARAMETERFIELD(Dev, LimitChecksStatus,
				VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
				Temp8);
	}

	return Status;

}
