
#include "msp430.h"
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include "dim.h"
#include "io.h"
#include "timer.h"
#include "lcp.h"
#include "lcp_constants.h"
#include "global.h"

// Dimming control flags
bool pwmratecalcflag=1,pwmincflag=1, soft_on_off_flag = 0,lcp_softoff_flag=0, PWMrateFlag = 0, DimDirection = 0; // 0-ramp down, 1-ramp up
// Dimming control variables
unsigned int pwmStepMax = 0;
static unsigned int pwmLinearRate = PWMRATEDEF, pwmStep = 0, pwmDimStep = 0, pwmSqrtRate = 0;
static float pwmSqRate = 0;
// Global variables
uint8_t CurveType = 0, PrevRLY = 0;
uint16_t CurrentLevel = 0, SetLevel = 0;

// Dimming Linear PWM change rate calculation program
unsigned int DimLinearRate(unsigned char transitionTime, unsigned int setLevel)
{
  unsigned int temp, x;
  temp = (transitionTime * 1000) >> 3; // Transition time in seconds, divided by 8ms timer period = step#
  pwmStepMax = temp + pwmDimStep;
  if(setLevel == CurrentLevel) return 0;
  else if(setLevel > CurrentLevel) x = (setLevel-CurrentLevel)/temp; // calculate the pwm changing rate in timer 4*2ms loop
  else x = (CurrentLevel-setLevel)/temp;
  return x;
}
// Dimming Square k rate calculation program
float DimSquareRate(unsigned char transitionTime, unsigned int setLevel)
{
  unsigned long tempSq;
  float x;
  pwmStepMax = (transitionTime * 1000) >> 3; // Transition time in seconds, divided by 8ms timer period = step#
  MPYS = pwmStepMax;
  OP2 = pwmStepMax;
  tempSq = ((uint32_t)RESHI<<16)| RESLO;
  if(setLevel == CurrentLevel) return 0;
  else if(setLevel > CurrentLevel) x= (setLevel - CurrentLevel)/tempSq;
  else x = (CurrentLevel - setLevel)/tempSq;
  return x;   
}
// Dimming Square Root k rate calculation program
unsigned int DimSqrtRate(unsigned char transitionTime, unsigned int setLevel)
{
  unsigned int tempSqrt, x;
  pwmStepMax = (transitionTime * 1000) >> 3; // Transition time in seconds, divided by 8ms timer period = step#
  if(setLevel == CurrentLevel) return 0;
  else {
    tempSqrt = sqrt(pwmStepMax);
    if(setLevel > CurrentLevel) x = (setLevel-CurrentLevel)/tempSqrt; // calculate the pwm changing rate in timer 4*2ms loop
    else x = (CurrentLevel-setLevel)/tempSqrt;
  }
  return x;
}

//**** Calculate PWM (4s default)******************
// x is steps# required to ramp (500steps in 4s)
// y(x) = kx^2 -> k=0.26214
// y(x) = kx^(1/2) -> k=2930
void calcPWMOutVal(void)
{
  unsigned long fOut;
  double rOut;
  switch(CurveType)
  {
    case CURVE_LINEAR:
      if(DimMode == DIM_MODE_DIM)
      {
        if(PWMrateFlag){ // if in dim mode
          MPYS = pwmStep;       // change pwmStep# to current dim rate
          OP2 = TransitionTime;
          pwmDimStep = RESLO >> 2; // divided by 4 (4s default)
          pwmLinearRate = DimLinearRate(TransitionTime, SetLevel);
          PWMrateFlag = false;
        }
      }
      else {
        pwmLinearRate = PWMRATEDEF;
        pwmStepMax = PWMSTEPMAX;
      }
      if(DimDirection){
        TA0CCR1 += pwmLinearRate;
        if(DimMode == DIM_MODE_DIM) pwmDimStep += 1;
        else pwmStep +=1;
        if(pwmStep > pwmStepMax) pwmStep = pwmStepMax;
      }
      else{
        TA0CCR1 -= pwmLinearRate;
        pwmStep -=1;
      }
      break;
    
    case CURVE_SQUARE:
      if(DimMode == DIM_MODE_DIM) // if in Dim mode
      {
        if(PWMrateFlag){ // enter into dim mode, calculate once
          pwmSqRate = DimSquareRate(TransitionTime, SetLevel);
          PWMrateFlag = false;
        }
      }
      else {
        pwmSqRate = 0.262;
        pwmStepMax = PWMSTEPMAX;
      }
      if(DimDirection){
        pwmStep +=1;
        if(pwmStep > pwmStepMax) pwmStep = pwmStepMax;
        MPYS = pwmStep;
        OP2 = pwmStep;
        fOut = ((uint32_t)RESHI<<16)| RESLO;
        fOut *= pwmSqRate;
        if(fOut > TA0CCR1) TA0CCR1 = fOut;
      }
      else{
        pwmStep -=1;
        MPYS = pwmStep;
        OP2 = pwmStep;
        fOut = ((uint32_t)RESHI<<16)| RESLO;
        fOut *= pwmSqRate;
        TA0CCR1 = fOut;
      }
      break;
    
    case CURVE_SQRT:
      if(DimMode == DIM_MODE_DIM) // if in Dim mode
      {
        if(PWMrateFlag){ // enter into dim mode, calculate once
          pwmSqrtRate = DimSqrtRate(TransitionTime, SetLevel);
          PWMrateFlag = false;
        }
      }
      else {
        pwmSqrtRate = 2931;
        pwmStepMax = PWMSTEPMAX;
      }
      if(DimDirection){
        pwmStep +=1;
        if(pwmStep > pwmStepMax) pwmStep = pwmStepMax;
        rOut = sqrt(pwmStep);
        rOut *= pwmSqrtRate;
        if(rOut > TA0CCR1)TA0CCR1 = rOut;
      }
      else{
        pwmStep -=1;
        rOut = sqrt(pwmStep);
        rOut *= pwmSqrtRate;
        TA0CCR1 = rOut;
      }
      break;
  }
}

void DoDimFunction(void)
{
  switch(DimMode)
  {
          
    // ON/OFF Mode*****************************************
    case DIM_MODE_ON:
      if(lcp_softon_flag == 0) {
              TA0CCR1 = PWM_FREQ;
              pwmStep = PWMSTEPMAX;
              RLY_ON;
              lcp_dim_flag = false;
              lcp_state_report = true;
      }
      else { // Soft ON
              RLY_ON;
              DimDirection = 1; // ramp up
              calcPWMOutVal();
              if((TA0CCR1 > (PWM_FREQ-PWMRATEDEF))||(pwmStep==pwmStepMax)) {
                TA0CCR1 = PWM_FREQ;
                lcp_dim_flag = false;
                lcp_state_report = true;
              }
      }
      break;
    
    case DIM_MODE_OFF:
      if(lcp_softon_flag == 0){
              RLY_OFF;
              TA0CCR1 = 0;
              pwmStep = 0;
              lcp_dim_flag = false;
              lcp_state_report = true;
      }
      else{ // Soft OFF
              DimDirection = 0; // ramp down
              if((TA0CCR1 < PWMRATEDEF)||(pwmStep==0)){
                TA0CCR1 = 0;
                lcp_softoff_flag = true;
              }
              else calcPWMOutVal();
              if(lcp_softoff_flag){
                lcp_softoff_flag = false;
                RLY_OFF;
                lcp_dim_flag = false; 
                lcp_state_report = true;
              }
      }
      break;
          
    // Dim Mode********************************************
    case DIM_MODE_DIM: 
            RLY_ON;
            if(TA0CCR1 == SetLevel){
              lcp_dim_flag = false;
              lcp_state_report = true;
            }
            else if(TA0CCR1 < SetLevel){
              DimDirection = 1; // ramp up
              calcPWMOutVal();
              if(TA0CCR1 > (SetLevel - pwmLinearRate)){
                TA0CCR1 = SetLevel;
                lcp_dim_flag = false;
                lcp_state_report = true;
              }
            }
            else{
              DimDirection = 0; // ramp down
              calcPWMOutVal();
              if(TA0CCR1 < (SetLevel + pwmLinearRate)){
                TA0CCR1 = SetLevel;
                lcp_dim_flag = false;
                lcp_state_report = true;
              }
            }
            if(lcp_dim_flag == 0){
              pwmStep = (pwmDimStep << 2)/TransitionTime;
            }
      break;
            
    // Dim Start*******************************************
    case DIM_MODE_START:
              if(Direction){
                RLY_ON;
                DimDirection = 1; // ramp up
                if((TA0CCR1 > (PWM_FREQ-PWMRATEDEF))){
                  TA0CCR1 = PWM_FREQ;
                  lcp_dim_flag = false;
                  lcp_state_report = true;
                }
                else calcPWMOutVal();
              }
              else{
                DimDirection = 0; // ramp down
                if((TA0CCR1 < PWMRATEDEF)||(pwmStep==0)){
                  TA0CCR1 = 0;
                  lcp_dim_flag = false;
                  lcp_state_report = true;
                }
                else calcPWMOutVal();
              }
            break;
      
    // Dim Stop********************************************
    case DIM_MODE_STOP:
            lcp_dim_flag = false;
            break;

  }
  CurrentLevel = TA0CCR1;
}
      

void DoButtonDim(void)
{
  if(pwmincflag){
    DimDirection = 1;
    if((TA0CCR1 >(PWM_FREQ-1)) || (pwmStep==PWMSTEPMAX)){
      TA0CCR1 = PWM_FREQ;
      pwmincflag = 0;
    }
    else calcPWMOutVal();
  }
  else {
    DimDirection = 0;
    if((TA0CCR1 < 1)||(pwmStep==0)){
      TA0CCR1 = 0;
      pwmincflag = 1;
    }
    else calcPWMOutVal();
  }
  CurrentLevel = TA0CCR1;
}

void DoButtonToggle(void)
{
  if(RLY_READ == 0){
    if(lcp_softon_flag == 0) {
      TA0CCR1 = PWM_FREQ;
      pwmStep = PWMSTEPMAX;
      RLY_ON;
      lcp_state_report = true;
    }
    else { // Soft ON
      RLY_ON;
      DimDirection = 1; // ramp up
      calcPWMOutVal();
      if((TA0CCR1 > (PWM_FREQ-PWMRATEDEF))||(pwmStep==pwmStepMax)) {
        TA0CCR1 = PWM_FREQ;
        lcp_state_report = true;
      }
    }
  }
  else {
      if(lcp_softon_flag == 0){
              RLY_OFF;
              TA0CCR1 = 0;
              pwmStep = 0;
              lcp_state_report = true;
      }
      else{ // Soft OFF
              DimDirection = 0; // ramp down
              if((TA0CCR1 < PWMRATEDEF)||(pwmStep==0)){
                TA0CCR1 = 0;
                lcp_softoff_flag = true;
              }
              else calcPWMOutVal();
              if(lcp_softoff_flag){
                lcp_softoff_flag = false;
                RLY_OFF;
                lcp_state_report = true;
              }
      }
  }
}
