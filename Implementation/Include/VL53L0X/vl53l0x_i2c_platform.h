/*
 * COPYRIGHT (C) STMicroelectronics 2014. All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * STMicroelectronics ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with STMicroelectronics
 *
 * Programming Golden Rule: Keep it Simple!
 *
 */

/**
 * @file   VL53L0X_platform.h
 * @brief  Function prototype definitions for Ewok Platform layer.
 *
 */


#ifndef _VL53L0X_I2C_PLATFORM_H_
#define _VL53L0X_I2C_PLATFORM_H_

#include "VL53L0X/vl53l0x_def.h"

#ifdef __cplusplus
extern "C" {
#endif

// Include u8, unit16_t  etc definitions

#include <stdint.h>
#include <stdarg.h>


/**
 *  @brief Typedef defining .\n
 * The developer shoud modify this to suit the platform being deployed.
 *
 */

// enum  {TRUE = true, FALSE = false};

/**
 * @brief Typedef defining 8 bit unsigned char type.\n
 * The developer shoud modify this to suit the platform being deployed.
 *
 */

#ifndef bool_t
typedef unsigned char bool_t;
#endif


#define	   I2C                0x01
#define	   SPI                0x00

#define    COMMS_BUFFER_SIZE    64  // MUST be the same size as the SV task buffer

#define    BYTES_PER_WORD        2
#define    BYTES_PER_DWORD       4

#define    VL53L0X_MAX_STRING_LENGTH_PLT       256

/**
 * @brief  Initialise platform comms.
 *
 * @param  comms_type      - selects between I2C and SPI
 * @param  comms_speed_khz - unsigned short containing the I2C speed in kHz
 *
 * @return status - status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_comms_initialise(u8  comms_type,
                                          u16 comms_speed_khz);

/**
 * @brief  Close platform comms.
 *
 * @return status - status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_comms_close(void);

/**
 * @brief  Cycle Power to Device
 *
 * @return status - status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_cycle_power(void);


/**
 * @brief Writes the supplied byte buffer to the device
 *
 * Wrapper for SystemVerilog Write Multi task
 *
 * @code
 *
 * Example:
 *
 * u8 *spad_enables;
 *
 * int status = VL53L0X_write_multi(RET_SPAD_EN_0, spad_enables, 36);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index - u8 register index value
 * @param  pdata - pointer to u8 buffer containing the data to be written
 * @param  count - number of bytes in the supplied byte buffer
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_write_multi(u8 address, u8 index, u8  *pdata, s32 count);


/**
 * @brief  Reads the requested number of bytes from the device
 *
 * Wrapper for SystemVerilog Read Multi task
 *
 * @code
 *
 * Example:
 *
 * u8 buffer[COMMS_BUFFER_SIZE];
 *
 * int status = status  = VL53L0X_read_multi(DEVICE_ID, buffer, 2)
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index - u8 register index value
 * @param  pdata - pointer to the u8 buffer to store read data
 * @param  count - number of u8's to read
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_read_multi(u8 address,  u8 index, u8  *pdata, s32 count);


/**
 * @brief  Writes a single byte to the device
 *
 * Wrapper for SystemVerilog Write Byte task
 *
 * @code
 *
 * Example:
 *
 * u8 page_number = MAIN_SELECT_PAGE;
 *
 * int status = VL53L0X_write_byte(PAGE_SELECT, page_number);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index - u8 register index value
 * @param  data  - u8 data value to write
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_write_byte(u8 address,  u8 index, u8   data);


/**
 * @brief  Writes a single word (16-bit unsigned) to the device
 *
 * Manages the big-endian nature of the device (first byte written is the MS byte).
 * Uses SystemVerilog Write Multi task.
 *
 * @code
 *
 * Example:
 *
 * u16 nvm_ctrl_pulse_width = 0x0004;
 *
 * int status = VL53L0X_write_word(NVM_CTRL__PULSE_WIDTH_MSB, nvm_ctrl_pulse_width);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index - u8 register index value
 * @param  data  - uin16_t data value write
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_write_word(u8 address,  u8 index, u16  data);


/**
 * @brief  Writes a single dword (32-bit unsigned) to the device
 *
 * Manages the big-endian nature of the device (first byte written is the MS byte).
 * Uses SystemVerilog Write Multi task.
 *
 * @code
 *
 * Example:
 *
 * u32 nvm_data = 0x0004;
 *
 * int status = VL53L0X_write_dword(NVM_CTRL__DATAIN_MMM, nvm_data);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index - u8 register index value
 * @param  data  - u32 data value to write
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_write_dword(u8 address, u8 index, u32  data);



/**
 * @brief  Reads a single byte from the device
 *
 * Uses SystemVerilog Read Byte task.
 *
 * @code
 *
 * Example:
 *
 * u8 device_status = 0;
 *
 * int status = VL53L0X_read_byte(STATUS, &device_status);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index  - u8 register index value
 * @param  pdata  - pointer to u8 data value
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_read_byte(u8 address,  u8 index, u8  *pdata);


/**
 * @brief  Reads a single word (16-bit unsigned) from the device
 *
 * Manages the big-endian nature of the device (first byte read is the MS byte).
 * Uses SystemVerilog Read Multi task.
 *
 * @code
 *
 * Example:
 *
 * u16 timeout = 0;
 *
 * int status = VL53L0X_read_word(TIMEOUT_OVERALL_PERIODS_MSB, &timeout);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index  - u8 register index value
 * @param  pdata  - pointer to u16 data value
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_read_word(u8 address,  u8 index, u16 *pdata);


/**
 * @brief  Reads a single dword (32-bit unsigned) from the device
 *
 * Manages the big-endian nature of the device (first byte read is the MS byte).
 * Uses SystemVerilog Read Multi task.
 *
 * @code
 *
 * Example:
 *
 * u32 range_1 = 0;
 *
 * int status = VL53L0X_read_dword(RANGE_1_MMM, &range_1);
 *
 * @endcode
 *
 * @param  address - u8 device address value
 * @param  index - u8 register index value
 * @param  pdata - pointer to u32 data value
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_read_dword(u8 address, u8 index, u32 *pdata);


/**
 * @brief  Implements a programmable wait in us
 *
 * Wrapper for SystemVerilog Wait in micro seconds task
 *
 * @param  wait_us - integer wait in micro seconds
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_platform_wait_us(s32 wait_us);


/**
 * @brief  Implements a programmable wait in ms
 *
 * Wrapper for SystemVerilog Wait in milli seconds task
 *
 * @param  wait_ms - integer wait in milli seconds
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_wait_ms(s32 wait_ms);


/**
 * @brief Set GPIO value
 *
 * @param  level  - input  level - either 0 or 1
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_set_gpio(u8  level);


/**
 * @brief Get GPIO value
 *
 * @param  plevel - u8 pointer to store GPIO level (0 or 1)
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_get_gpio(u8 *plevel);

/**
 * @brief Release force on GPIO
 *
 * @return status - SystemVerilog status 0 = ok, 1 = error
 *
 */

s32 VL53L0X_release_gpio(void);


/**
* @brief Get the frequency of the timer used for ranging results time stamps
*
* @param[out] ptimer_freq_hz : pointer for timer frequency
*
* @return status : 0 = ok, 1 = error
*
*/

s32 VL53L0X_get_timer_frequency(s32 *ptimer_freq_hz);

/**
* @brief Get the timer value in units of timer_freq_hz (see VL53L0X_get_timestamp_frequency())
*
* @param[out] ptimer_count : pointer for timer count value
*
* @return status : 0 = ok, 1 = error
*
*/

s32 VL53L0X_get_timer_value(s32 *ptimer_count);





#ifdef __cplusplus
}
#endif

#endif //_VL53L0X_I2C_PLATFORM_H_

