
// dim.h

#define CURVE_LINEAR    0
#define CURVE_SQUARE    1
#define CURVE_SQRT      2

#define DIM_MODE_ON     0
#define DIM_MODE_OFF    1
#define DIM_MODE_DIM    2
#define DIM_MODE_START  3
#define DIM_MODE_STOP   4

#define PWMLINRATEDEF   131     //default LINEAR PWM rate to 4s from 0 to 100%
#define PWMSQRATEDEF    0.262
#define PWMSQRTRATEDEF  2931    //default square-root k ratio
#define PWMSTEPMAXDEF   500     //default PWM step 500 ~ 4s

extern void DoDimFunction(void);
extern void DoButtonDim(void);
extern void DoButtonToggle(void);
extern unsigned int CurrentLevel, SetLevel;
extern unsigned char PrevRLY, CurveType;
extern bool soft_on_off_flag, PWMrateFlag;