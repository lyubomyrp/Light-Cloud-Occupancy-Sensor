#ifdef TIMER_DEF
#define TIMER_EXT
#else
#define TIMER_EXT extern
#endif

TIMER_EXT void timer_init();

#define TIMER_FREQ      0xFFFF          // frequency 1khz
#define PWM_FREQ        TIMER_FREQ-1    // Period = 4ms @ 16.0384MHz

#ifdef LC_CONTROLLER
#define OUTMOD_PWM      OUTMOD_3        // pwm output mode
#elif  LC_OCCUPANCY
#define OUTMOD_PWM      OUTMOD_7
#endif

#define TIMER_1S        250
#define TIMER_2S        500
#define TIMER_05S       125
#define TIMER_9S        2250
#define TIMER_30S_19    3

#define RADIOINITDELAY  50      // 5s
#define RADIORSPDELAY   3       // 500ms

TIMER_EXT bool msflag, radioInitflag;
TIMER_EXT unsigned char count_8ms, led_30s_19_delay;
TIMER_EXT unsigned int countsec, power_t0, power_t1, radioResponseDelay, AssoCounter, SsCounter;
TIMER_EXT unsigned int led_rvpkt_delay, led_30s_1s_delay, led_1s_delay, led_30s_2s_delay, led_2s_delay, led_30s_05s_delay, led_05s_delay, led_1s_9s_delay, led_9s_1s_delay;