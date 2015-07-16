/*******************************************************************************
 *  metrology-decs.h -
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

/*! \file */

#if !defined(_METROLOGY_DECS_H_)
#define _METROLOGY_DECS_H_

#include "metrology-nv-structs.h"

extern const struct calibration_data_s calibration_defaults;
extern const struct info_mem_s nv_parms;

extern struct metrology_data_s working_data;

#if defined(__MSP430__)
/*! The maximum per sample change in the voltage signal, before we declare a voltage spike. */
#define MAX_PER_SAMPLE_VOLTAGE_SLEW     4000

#include "metrology-instantiate-adcs.h"
#endif

/*! This variable is used as a series of flag bits for various purposes in the meter. */
//extern uint16_t metrology_state; - removed by mars 20130204

#if defined(TEMPERATURE_SUPPORT)
extern int16_t raw_temperature_from_adc;

extern int16_t temperature_in_celsius;

/*! Get a random number, if available, from the true random number generator, based on Gaussian
    noise in the LSB of the thermal diode. This is only available if the temperature is being measured.
    This return -1 for no number available, or 0 for a fresh random number has been returned as *val. */
int trng(uint16_t *val);

/*! True random number generator, based on Gaussian noise in the LSB of the thermal diode. This
    is only available if the temperature is being measured. This routine will wait for a new random
    number if none is available. */
uint16_t trng_wait(void);
#endif

/*! The table of FIR coefficients to produce a range of phase shifts from -1/2 an ADC sample interval
    to +1/2 an ADC sample interval. When the SD16 is used, the hardware phase shifting capability of
    the ADC is used, instead of this table. */
extern const int16_t fir_coeffs[][2];

/*! The array element in fir_coeffs which represents zero phase shift. */
#define FIR_UNITY_LOAD              63

#if defined(__HAS_SD_ADC__)
void set_sd_phase_correction(struct phase_correction_sd_s *s, int phx, int correction);
#endif

#if !defined(custom_adc_interrupt)
extern void custom_adc_interrupt(void);
#endif

void set_phase_correction(struct phase_correction_s *s, int correction);

#endif
