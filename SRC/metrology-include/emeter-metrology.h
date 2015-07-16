/*******************************************************************************
 *  emeter-metrology.h -
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

//#include "metrology-include/metrology-template.h"
//#include "metrology-calibration-template.h"

#if defined(__MSP430_HAS_SD16_2__)  \
  ||  defined(__MSP430_HAS_SD16_3__)  \
  ||  defined(__MSP430_HAS_SD16_A3__)  \
  ||  defined(__MSP430_HAS_SD16_A4__)  \
  ||  defined(__MSP430_HAS_SD16_A6__)  \
  ||  defined(__MSP430_HAS_SD16_A7__)  \
  ||  defined(__MSP430_HAS_SD24_2__)  \
  ||  defined(__MSP430_HAS_SD24_A2__)  \
  ||  defined(__MSP430_HAS_SD24_3__)  \
  ||  defined(__MSP430_HAS_SD24_A3__) \
  ||  defined(__MSP430_HAS_SD24_4__)  \
  ||  defined(__MSP430_HAS_SD24_A4__)  \
  ||  defined(__MSP430_HAS_SD24_B__)
#define __HAS_SD_ADC__
#endif

#ifndef OSR
#define OSR   256
#elif ((OSR != 256) && (OSR != 128))
#error UNSUPPORTED OSR VALUE
#endif

#if defined(__HAS_SD_ADC__)
  #if (OSR == 256)
#define SAMPLE_RATE                         4000//3906
#define SAMPLES_PER_10_SECONDS              (40000UL)//(39062UL)
  #elif (OSR == 128)
#define SAMPLE_RATE                         8000//7812
#define SAMPLES_PER_10_SECONDS              (80000UL)//(78125UL)
  #endif
  #if defined (LIMP_MODE_SUPPORT)
#define LIMP_SAMPLES_PER_10_SECONDS         10240
#define LIMP_SAMPLING_RATIO                 4
  #endif
#endif

#include "metrology-interactions.h"

#include "metrology-types.h"

#include "metrology-foreground.h"

#include "metrology-background.h"

#include "metrology-readings.h"

#include "metrology-flash.h"

//#include "metrology-capacitor-compensate.h"


// here be,ow are the defintion of the call back functions
// that must be implemented in main.c in user application
// for the metrology code to correctly interact to external world
#if defined(TOTAL_ACTIVE_ENERGY_PULSES_PER_KW_HOUR)
void total_active_energy_pulse_start(void);
void total_active_energy_pulse_end(void);
#endif

#if defined(TOTAL_REACTIVE_ENERGY_PULSES_PER_KVAR_HOUR)
void total_reactive_energy_pulse_start(void);
void total_reactive_energy_pulse_end(void);
#endif

#if defined(ACTIVE_ENERGY_PULSES_PER_KW_HOUR)
    #if NUM_PHASES == 1
void active_energy_pulse_start(void);
void active_energy_pulse_end(void);
    #else
void active_energy_pulse_start(int ph);
void active_energy_pulse_end(int ph);
    #endif
#endif

#if defined(REACTIVE_ENERGY_PULSES_PER_KVAR_HOUR)
    #if NUM_PHASES == 1
void reactive_energy_pulse_start(void);
void reactive_energy_pulse_end(void);
    #else
void reactive_energy_pulse_start(int ph);
void reactive_energy_pulse_end(int ph);
    #endif
#endif

void BACKGROUND_PROCESS_ON (void);
void BACKGROUND_PROCESS_OFF (void);
void FOREGROUND_PROCESS_ON (void);
void FOREGROUND_PROCESS_OFF (void);
void AC_MODE_ON (void);
void AC_MODE_OFF (void);
void DC_MODE_ON (void);
void DC_MODE_OFF (void);
void V_GOOD_ON (void);
void V_GOOD_OFF (void);
void ZERO_CROSS_ON (void);
void ZERO_CROSS_OFF (void);

void align_metrology_with_calibration_data(void);

int metrology_init(void);


