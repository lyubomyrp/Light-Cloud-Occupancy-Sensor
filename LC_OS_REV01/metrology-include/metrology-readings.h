/*******************************************************************************
 *  metrology-readings.h -
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

#if !defined(_METROLOGY_READINGS_H_)
#define _METROLOGY_READINGS_H_

/* Name the three phases, and fake phase numbers for neutral and aggregate values */
enum
{
    PHASE_RED = 0,
    PHASE_YELLOW = 1,
    PHASE_BLUE = 2,

    /* The following do not represent actual phases, but are numbers used to select
       the appropriate information */
    FAKE_PHASE_TOTAL = 3,
    /* For single phase this is used to select per sensor information for the neutral lead.
       For 3-phase this is used to select the neutral */
    FAKE_PHASE_NEUTRAL = 4,
    /* For single phase this is used to select per sensor information for the live lead */
    FAKE_PHASE_LIVE = 5
};

enum
{
    DOT_PRODUCT_TYPE_P_ACTIVE = 0,
    DOT_PRODUCT_TYPE_P_REACTIVE = 1,
    DOT_PRODUCT_TYPE_V_SQUARED = 2,
    DOT_PRODUCT_TYPE_I_SQUARED = 3,
    DOT_PRODUCT_TYPE_V_FUNDAMENTAL_SQUARED = 4,
    DOT_PRODUCT_TYPE_P_FUNDAMENTAL_ACTIVE = 5,
    DOT_PRODUCT_TYPE_P_FUNDAMENTAL_REACTIVE = 6,
    DOT_PRODUCT_TYPE_I_RESIDUAL_SQUARED = 7
};

/* These are the flags defined for each phase's status variable. */
enum
{
    PHASE_STATUS_NEW_LOG = 0x0001,

    /*! This flag in a channel's status variable indicates the voltage signal is currently in
        the positive half of its cycle. */
    PHASE_STATUS_V_POS = 0x0002,

    /*! This flag in a channel's status variable indicates the current signal is currently in
        the positive half of its cycle. */
    PHASE_STATUS_I_POS = 0x0004,
    PHASE_STATUS_ENERGY_LOGABLE = 0x0008,

    /*! This flag in a channel's status variable indicates the voltage signal was in overload
        during the last logged interval. Overload is determined by an excessive number of
        samples hitting the end-stops of the ADC's range. */
    PHASE_STATUS_V_OVERRANGE = 0x0010,

    /*! This flag in a channel's status variable indicates the phase current signal was in overload
        during the last logged interval. Overload is determined by an excessive number of
        samples hitting the end-stops of the ADC's range. */
    PHASE_STATUS_I_OVERRANGE = 0x0020,

    /*! This flag in a channel's status variable indicates the phase current signal was reversed
        during the last logged interval. */
    PHASE_STATUS_I_REVERSED = 0x0040,

    /*! This flag in a channel's status variable indicates the phase current signal was in overload
        during the last logged interval. Overload is determined by an excessive number of
        samples hitting the end-stops of the ADC's range. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_I_NEUTRAL_OVERRANGE = 0x0080,

    /*! This flag in a channel's status variable indicates the neutral current signal was
        reversed during the last logged interval. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_I_NEUTRAL_REVERSED = 0x0100,

    /*! This flag in a channel's status variable indicates the neutral current is the one
        currently being used. This means it has been judged by the anti-tamper logic to be
        the measurement which can best be trusted. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_CURRENT_FROM_NEUTRAL = 0x0200,

    /*! This flag in a channel's status variable indicates the neutral current signal is
        currently in the positive half of its cycle. This is only used if the meter supports
        monitoring of both the live and neutral leads for anti-tamper management. */
    PHASE_STATUS_I_NEUTRAL_POS = 0x0800,

    /*! This flag in a channel's status variable indicates the power has been declared to be
        reversed, after the anti-tamper logic has processed the raw indications. Live neutral
        or both leads may be reversed when this bit is set. */
    PHASE_STATUS_REVERSED = 0x1000,

    /*! This flag in a channel's status variable indicates the power (current in limp mode)
        has been declared to be unbalanced, after the anti-tamper logic has processed the
        raw indications. */
    PHASE_STATUS_UNBALANCED = 0x2000,
    
    PHASE_STATUS_DC_MODE = 0x8000
};

enum
{
    /*! This bit indicates the meter is in the power down state. */
    METROLOGY_STATUS_POWER_DOWN = 0x0004,

    /*! This bit indicates the current status of the meter is "current flow is reversed", after
        all persistence checking, and other safeguards, have been used to check the validity of the
        reverse indication. */
    METROLOGY_STATUS_REVERSED = 0x0100,

    /*! This bit indicates the current status of the meter is "current flow is earthed", after
        all persistence checking, and other safeguards, have been used to check the validity of the
        earthed indication. */
    METROLOGY_STATUS_EARTHED = 0x0200,

    /*! This bit indicates the phase voltage is OK. */
    METROLOGY_STATUS_PHASE_VOLTAGE_OK = 0x0400,

    /*! This bit indicates the battery condition is OK. If battery monitoring is not enabled, this bit
        is not used. */
    METROLOGY_STATUS_BATTERY_OK = 0x0800
};

enum
{
    OPERATING_MODE_NORMAL = 0,
#if defined(LIMP_MODE_SUPPORT)
    OPERATING_MODE_LIMP = 1,
    OPERATING_MODE_LIMP_BURST = 2,
#endif
    OPERATING_MODE_LCD_ONLY = 3,
    OPERATING_MODE_POWERFAIL = 4
};

enum
{
    ENERGY_DIRECTION_IMPORT = 0,
    ENERGY_DIRECTION_EXPORT = 1
};

enum
{
    ENERGY_QUADRANT_I = 0,
    ENERGY_QUADRANT_II = 1,
    ENERGY_QUADRANT_III = 2,
    ENERGY_QUADRANT_IV = 3
};

#if defined(__cplusplus)
extern "C"
{
#endif

/*! This variable specifies the current operating mode of the meter. */
extern int8_t operating_mode;
#if defined(LIMP_MODE_SUPPORT)
extern int normal_limp;
#endif

power_t active_power(int ph);

#if defined(ACTIVE_ENERGY_SUPPORT)
energy_t active_energy(int ph, int import_export);
#else
static inline energy_t active_energy(int ph, int import_export)
{
    return 0;
}
#endif

#if defined(REACTIVE_POWER_SUPPORT)
power_t reactive_power(int ph);
#else
static inline power_t reactive_power(int ph)
{
    return 0;
}
#endif

#if defined(REACTIVE_ENERGY_SUPPORT)  ||  defined(TOTAL_REACTIVE_ENERGY_SUPPORT)
energy_t reactive_energy(int ph, int quad);
#else
static inline energy_t reactive_energy(int ph, int quad)
{
    return 0;
}
#endif

#if defined(APPARENT_POWER_SUPPORT)
power_t apparent_power(int ph);
#else
static inline power_t apparent_power(int ph)
{
    return 0;
}
#endif

#if defined(APPARENT_ENERGY_SUPPORT)
energy_t apparent_energy(int ph, int import_export);
#else
static inline energy_t apparent_energy(int ph, int import_export)
{
    return 0;
}
#endif

#if defined(FUNDAMENTAL_ACTIVE_POWER_SUPPORT)
power_t fundamental_active_power(int ph);
#else
static inline power_t fundamental_active_power(int ph)
{
    return 0;
}
#endif

#if defined(FUNDAMENTAL_REACTIVE_POWER_SUPPORT)
power_t fundamental_reactive_power(int ph);
#else
static inline power_t fundamental_reactive_power(int ph)
{
    return 0;
}
#endif

#if defined(POWER_FACTOR_SUPPORT)
power_factor_t power_factor(int ph);
#else
static inline power_factor_t power_factor(int ph)
{
    return 0;
}
#endif

#if defined(VRMS_SUPPORT)
rms_voltage_t rms_voltage(int ph);
#else
static inline rms_voltage_t rms_voltage(int ph)
{
    return 0;
}
#endif

#if defined(FUNDAMENTAL_VRMS_SUPPORT)
rms_voltage_t fundamental_rms_voltage(int ph);
#else
static inline rms_voltage_t fundamental_rms_voltage(int ph)
{
    return 0;
}
#endif

#if defined(VOLTAGE_THD_SUPPORT)
thd_t voltage_thd(int ph);
#else
static inline thd_t voltage_thd(int ph)
{
    return 0;
}
#endif

#if defined(IRMS_SUPPORT)
rms_current_t rms_current(int ph);
#else
static inline rms_current_t rms_current(int ph)
{
    return 0;
}
#endif

#if defined(FUNDAMENTAL_IRMS_SUPPORT)
rms_current_t fundamental_rms_current(int ph);
#else
static inline rms_current_t fundamental_rms_current(int ph)
{
    return 0;
}
#endif

#if defined(CURRENT_THD_SUPPORT)
thd_t current_thd(int ph);
#else
static inline thd_t current_thd(int ph)
{
    return 0;
}
#endif

#if defined(MAINS_FREQUENCY_SUPPORT)
int16_t mains_frequency(int ph);
#else
static inline int16_t mains_frequency(int ph)
{
    return 0;
}
#endif

#if NUM_PHASES > 1  &&  defined(FUNDAMENTAL_VRMS_SUPPORT)
phase_angle_t phase_to_phase_angle(int ph);
#else
static inline phase_angle_t phase_to_phase_angle(int ph)
{
    return 0;
}
#endif

#if NUM_PHASES > 1  &&  defined(RESIDUAL_IRMS_SUPPORT)
rms_current_t residual_3phase_rms_current(void);
#else
static inline rms_current_t residual_3phase_rms_current(void)
{
    return 0;
}
#endif

#if defined(SAG_SWELL_SUPPORT)
int16_t sag_events(int ph, uint32_t *duration);

int16_t swell_events(int ph, uint32_t *duration);
#else
static inline int16_t sag_events(int ph, uint32_t *duration)
{
    if (duration)
        *duration = 0;
    return 0;
}

static inline int16_t swell_events(int ph, uint32_t *duration)
{
    if (duration)
        *duration = 0;
    return 0;
}
#endif

#if !defined(ESP_SUPPORT)
int32_t voltage_dc_estimate(int ph);

int32_t current_dc_estimate(int ph);

int64_t dot_product(int ph, int which, uint16_t *samples);
#endif

#if defined(TEMPERATURE_SUPPORT)
int temperature(void);
#endif

uint16_t phase_status(int ph);

uint16_t metrology_status(void);

#define meter_calibrated() (!get_calibration_status())

int clear_calibration_data(void);

int get_phase_calibration_data(int ph, void *data);

int set_phase_calibration_data(int ph, void *data);

#if defined(NEUTRAL_MONITOR_SUPPORT)
int get_neutral_calibration_data(int phase, void *data);

int set_neutral_calibration_data(int phase, void *data);
#endif

int16_t get_v_dc_estimate(int phx);

int16_t get_initial_v_dc_estimate (int phx);

void set_v_dc_estimate(int phx, int16_t value);

#define get_i_dc_estimate(x) current_dc_estimate(x)

int32_t get_initial_i_dc_estimate(int phx);

void set_i_dc_estimate(int phx, int32_t value);

int32_t get_v_ac_offset(int phx);

void set_v_ac_offset(int phx, int32_t value);

int32_t get_i_ac_offset(int phx);

void set_i_ac_offset(int phx, int32_t value);

int16_t get_phase_corr(int phx);

void set_phase_corr(int phx, int16_t value);

calibration_scaling_factor_t get_V_rms_scaling(int phx);

void set_V_rms_scaling(int phx, calibration_scaling_factor_t value);

calibration_scaling_factor_t get_I_rms_scaling(int phx);

void set_I_rms_scaling(int phx, calibration_scaling_factor_t value);

uint16_t get_compensate_capacitor_value (int phx);

void set_compensate_capacitor_value(int phx, uint16_t value);

uint16_t get_compensate_resistance (int phx);

void set_compensate_resistance (int phx, uint16_t value);

calibration_scaling_factor_t get_P_scaling(int phx);

void set_P_scaling(int phx, calibration_scaling_factor_t value);

int get_calibration_status(void);

void set_calibration_status(int value);

int16_t get_temperature_intercept(void);

int16_t get_temperature_slope(void);

void set_temperature_parameters(int16_t temperature_at_calibration, int16_t temperature_sensor_intercept, int16_t temperature_sensor_slope);

#if defined(__cplusplus)
}
#endif

#endif
