
#ifdef ADC_DEF
#define ADC_EXT
#else
#define ADC_EXT extern
#endif

ADC_EXT void adc_init();
extern void v_rms_calc(void);

ADC_EXT unsigned int tV, tI, send_V, send_I, send_P, send_F, send_PF, send_THD;
extern bool ADCInitFlag;
