/*******************************************************************************
 *  metrology-template.h - MSP430AFE253 distribution version
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
#define OSR 128
#define AVERAGE 3
#define IEC_THD_SUPPORT
//#undef IEC_THD_SUPPORT

/*! This selects the number of phases. */
#define NUM_PHASES 1
#define REPORT_CYCLE 4
#define CAPACITOR_COMPENSATE_ON
//#undef CAPACITOR_COMPENSATE_ON

#define WIRE_RESISTANCE_COMPENSATE
//#undef WIRE_RESISTANCE_COMPENSATE
 
/*! This switch enables monitoring of the neutral lead for anti-fraud purposes, in
    either single phase or 3-phase meters. */
//#define NEUTRAL_MONITOR_SUPPORT
#undef NEUTRAL_MONITOR_SUPPORT
/*! This switch selects the use of 24 bit values for the current signal, where the
    ADC supports this. If this symbol is not defined, 16 bit values are used.
    There is nothing to be gained by using more than 16 bits for the the voltage
    signal, so we don't. */
#define TWENTYFOUR_BIT

/*! ADC channel allocation */
#define PHASE_1_VOLTAGE_ADC_CHANNEL                 0
#define PHASE_1_CURRENT_ADC_CHANNEL                 1
//#define NEUTRAL_CURRENT_ADC_CHANNEL                 2
#define TEMPERATURE_ADC_CHANNEL                 2

/*! The gain setting for the voltage channel of the SD16, for devices using the
    SD16 sigma-delta ADC. This is usually GAIN_1 (i.e. x1 gain). */
#define SD_VOLTAGE_GAIN                             SD24GAIN_1

/*! The gain setting for the first current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define SD_LIVE_CURRENT_GAIN                        SD24GAIN_16

/*! The gain setting for the second current channel channel of the SD16,
    for devices using the SD16 sigma-delta ADC.
    This must be set to suit the shunt or CT in use. Typical values for a
    shunt are GAIN_16 (x16 gain) or GAIN_32 (x32 gain). Typical values for a
    CT are GAIN_1 (x1 gain) or GAIN_2 (x2 gain). */
#define SD_NEUTRAL_CURRENT_GAIN                     SD24GAIN_1

#define SD_CLOCK_DIVISION                           16

/*! The nominal mains frequency, in Hz. This is used to prime the mains frequency
    measurement, and make its initial value settle quickly. It is not currently used after
    reset. */
#define MAINS_NOMINAL_FREQUENCY                     50

/*! This selects the nominal voltage used for sag/swell detection, and power calculations
    in limp mode, in volts */
#define MAINS_NOMINAL_VOLTAGE                       220

/*! This selects the basis current */
#define MAINS_BASIS_CURRENT                         5

/*! This selects the maximum operating current */
#define MAINS_MAXIMUM_CURRENT                       30

//#define TEAGER_KAISER_TEST_SUPPORT
#undef TEAGER_KAISER_TEST_SUPPORT

/*! This switch enables mains frequency measurement. This may be used as a
    meter feature. It may be a requirement, if non-linear CT sensors are used. */
#define MAINS_FREQUENCY_SUPPORT
//#undef MAINS_FREQUENCY_SUPPORT

/*! This selects support for RMS voltage measurement. */
#define VRMS_SUPPORT

#define VOLTAGE_THD_SUPPORT
//#undef VOLTAGE_THD_SUPPORT


#define FUNDAMENTAL_VRMS_SUPPORT
//#undef FUNDAMENTAL_VRMS_SUPPORT

/*! This selects support for RMS current measurement. */
#define IRMS_SUPPORT

#define CURRENT_THD_SUPPORT
//#undef CURRENT_THD_SUPPORT

#define FUNDAMENTAL_IRMS_SUPPORT
//#undef FUNDAMENTAL_IRMS_SUPPORT

#define FUNDAMENTAL_ACTIVE_POWER_SUPPORT
//#undef FUNDAMENTAL_ACTIVE_POWER_SUPPORT

/*! This selects support for reactive power measurement. */
#define REACTIVE_POWER_SUPPORT
//#undef REACTIVE_POWER_SUPPORT

#define FUNDAMENTAL_REACTIVE_POWER_SUPPORT
//#undef FUNDAMENTAL_REACTIVE_POWER_SUPPORT

/*! This selects support for reactive power measurement through quadrature processing.
    This is only effective when REACTIVE_POWER_SUPPORT is enabled. */
#define REACTIVE_POWER_BY_QUADRATURE_SUPPORT
//#undef REACTIVE_POWER_BY_QUADRATURE_SUPPORT

/*! The selects support for apparent or VA power measurement. */
#define APPARENT_POWER_SUPPORT
//#undef APPARENT_POWER_SUPPORT

#define POWER_FACTOR_SUPPORT
//#undef POWER_FACTOR_SUPPORT

/*! Tiny power levels should not record energy at all, as they may just be rounding
    errors, noise, or the consumption of the meter itself. This value is the cutoff level,
    in milliwatts. */
#define RESIDUAL_POWER_CUTOFF                       2500

/*! This switch selects support for measuring the active energy consumption. */
#define ACTIVE_ENERGY_SUPPORT
//#undef ACTIVE_ENERGY_SUPPORT

/*! This sets the number of pulses per kilo-watt hour the meter will produce at
    its active energy pulse LED/opto-coupler/LCD segment. It does not affect the
    energy accumulation process. */
#define ACTIVE_ENERGY_PULSES_PER_KW_HOUR            1000
//#undef ACTIVE_ENERGY_PULSES_PER_KW_HOUR

/*! This switch selects support for measuring the reactive energy consumption. */
//#define REACTIVE_ENERGY_SUPPORT
#undef REACTIVE_ENERGY_SUPPORT
/*! The duration of the LED on time for an energy pulse. This is measured in
    ADC samples (i.e. increments 1/3276.8s). The maximum allowed is 255, giving a
    pulse of about 78ms. 163 gives a 50ms pulse. */
#define ENERGY_PULSE_DURATION_FIGURE                       163
//#undef ENERGY_PULSE_DURATION_FIGURE

/*! This switch enables use of the MSP430's internal temperature diode to
    measure the meter's temperature. */
#define TEMPERATURE_SUPPORT
//#undef TEMPERATURE_SUPPORT

/*! Normally the meter software only calculates the properly scaled values
    for voltage, current, etc. as these values are needed. This define
    enables additional global parameters, which are regularly updated with
    all the metrics gathered by the meter. This is generally less efficient,
    as it means calculating things more often than necessary. However, some
    may find this easier to use, so it is offered as a choice for the meter
    designer. */
#define PRECALCULATED_PARAMETER_SUPPORT

/*! This enables monitoring of the balance between the current in the live and neutral
    leads, for tamper detection. */
//#define PHASE_UNBALANCED_DETECTION_SUPPORT
#undef PHASE_UNBALANCED_DETECTION_SUPPORT

/*! This is a shift value for comparing currents or powers when looking for
    imbalance between live and neutral. 3 give 12.5%. 4 give 6.25%. These are the
    two commonest values to use. The relaxed version is used at low power levels,
    where the values are less accurate, and a tight imbalance check might give false
    results. */
#define PHASE_UNBALANCED_PERMITTED_FRACTION         4

/*! This is a relaxed version of the permitted imbalance fraction, for use at low
    powers/currents, where the power (and hence imbalance) measurement may be less
    precise. The tighter imbalance measurement may give false results under high
    EMI conditions. */
#define PHASE_UNBALANCED_RELAXED_FRACTION           2

/*! This is the number of successive measurement blocks which must agree the
    unbalanced status has changed before we accept it. */
#define PHASE_UNBALANCED_PERSISTENCE_CHECK          5

/*! This is the minimum current level (limp mode) and minimum power level (normal
    mode) at which we will make checks for the phase unbalanced (earthed) condition.
    Very small readings lack the accuracy and resolution needed to make meaningful
    comparisons between live and neutral. */
#define PHASE_UNBALANCED_CUTOFF_THRESHOLD_CURRENT   50
#define PHASE_UNBALANCED_TIGHT_THRESHOLD_CURRENT    500
#define PHASE_UNBALANCED_CUTOFF_THRESHOLD_POWER     20
#define PHASE_UNBALANCED_TIGHT_THRESHOLD_POWER      2000

/*! This selects the detection of the reversed power condition. */
//#define PHASE_REVERSED_DETECTION_SUPPORT
#undef PHASE_REVERSED_DETECTION_SUPPORT

/*! This selects that the reversed power condition is to be treated as tampering. */
//#define PHASE_REVERSED_IS_TAMPERING
#undef PHASE_REVERSED_IS_TAMPERING

/*! This is the number of successive measurement blocks which must agree the
    reversed current status has changed before we accept it. */
#define PHASE_REVERSED_PERSISTENCE_CHECK            5

/*! This is the minimum power level at which we will make checks for the reverse
    condition. Very small readings are not a reliable indicator, due to noise. */
#define PHASE_REVERSED_THRESHOLD_POWER              2000

/*! This selects the operation from current only, when only one lead is
    functioning, and the meter is powered by a parasitic CT supply attached to
    the leads. This is for anti-fraud purposes. Energy is accumulated at the
    highest possible rate, assuming unity power factor, and the nominal voltage */
//#define LIMP_MODE_SUPPORT
#undef LIMP_MODE_SUPPORT

/*! If limp mode is supported, these set the voltage thresholds for switching
    bewteen normal mode and limp mode. */
#define LIMP_MODE_VOLTAGE_THRESHOLD                 50
#define NORMAL_MODE_VOLTAGE_THRESHOLD               80

/*! If limp mode is supported, this sets the threshold current, in mA, below which we
    no not operate. */
#define LIMP_MODE_MINIMUM_CURRENT                   800

/*! This is called every ADC interrupt, after the main DSP work has finished.
    It can be used for things like custom keypad operations. It is important
    this is a very short routine, as it is called from the main ADC interrupt. */
#define custom_adc_interrupt()                      /**/
