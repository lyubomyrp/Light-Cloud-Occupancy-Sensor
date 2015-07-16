/*******************************************************************************
 *  metrology-types.h -
 *
 *  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#if !defined(_METROLOGY_TYPES_H_)
#define _METROLOGY_TYPES_H_

/* We store voltages in 1mV steps. A 32 bit unsigned number supports the range
   1mV to 2147483.647V, which seems OK for all forseeable uses. */
typedef int32_t rms_voltage_t;
#define RMS_VOLTAGE_OVERRANGE INT32_MAX
#define RMS_VOLTAGE_FRACTIONAL_DIGITS 3

/* We store currents in 1uA steps. A 32 bit unsigned number supports the range
   1uA to 2147.483647A, which seems far more than will ever be needed. */
typedef int32_t rms_current_t;
#define RMS_CURRENT_OVERRANGE INT32_MAX
#define RMS_CURRENT_FRACTIONAL_DIGITS 6

/* We store powers in 1mW/mvar/mVA steps. A 32 bit signed number supports the range
   +-1mW to +-2147483.647W, which seems far more than will ever be needed. */
typedef int32_t power_t;
#define POWER_OVERRANGE INT32_MAX
#define POWER_FRACTIONAL_DIGITS 3

/* We store frequencies in 0.01Hz steps. A 16 bit unsigned number supports the range
   0.01Hz to 655.35Hz, which is clearly more than we will ever need for the
   fundamental. It might not encompass all the harmonic frequencies we could be
   required to display in the future. */
typedef uint16_t frequency_t;
#define FREQUENCY_FRACTIONAL_DIGITS 2

/* We store power factors as +ve for leading, and -ve for lagging. +/-10000 is unity power factor */
typedef int16_t power_factor_t;
#define POWER_FACTOR_FRACTIONAL_DIGITS 4

/* We accumulate energy in 100mWh/varh/VAh steps. This is a signed value, so we can use an 
   energy value that may go negative when we export a sufficient amount. */
typedef int64_t energy_t;
#define ENERGY_FRACTIONAL_DIGITS 1

/* We store THD in 0.01% steps. */
typedef int16_t thd_t;
#define THD_FRACTIONAL_DIGITS 2

/* We store phase angles in ??? */
typedef int16_t phase_angle_t;

#if defined(TWENTYFOUR_BIT)
/* We use 24 bit current samples and 16 bit voltage samples */
typedef int16_t voltage_sample_t;
typedef int32_t current_sample_t;

/* We use 48 bit dot product accumulators for voltage*voltage
          64 bit dot product accumulators for voltage*current
          64 bit dot product accumulators for current*current */
typedef int16_t voltage_accum_t[3];
typedef int64_t current_accum_t[1];
typedef int64_t power_accum_t[1];

/* We use 32 bit registers for the voltage DC estimators
          48 bit registers for the current DC estimators */
typedef int32_t voltage_dc_estimate_t[1];
typedef int16_t current_dc_estimate_t[3];
#else
/* We use 16 bit current and voltage samples */
typedef int16_t voltage_sample_t;
typedef int16_t current_sample_t;

/* We use 48 bit dot product accumulators for voltage*voltage
          48 bit dot product accumulators for voltage*current
          48 bit dot product accumulators for current*current */
typedef int16_t voltage_accum_t[3];
typedef int16_t current_accum_t[3];
typedef int16_t power_accum_t[3];

/* We use 32 bit registers for the voltage DC estimators
          32 bit registers for the current DC estimators */
typedef int32_t voltage_dc_estimate_t[1];
typedef int32_t current_dc_estimate_t[1];
#endif

typedef uint16_t calibration_scaling_factor_t;

#endif
