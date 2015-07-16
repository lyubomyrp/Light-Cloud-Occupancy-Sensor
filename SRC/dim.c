/*******************************************************************************
*   File name: dim.c
*
*   Description: do dim function based on LCP request
*     This file contains functions for calculating dim steps 
*       based on curve and transition time request
*
********************************************************************************
*/

#include "msp430.h"
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include "global.h"
#include "lcp.h"
#include "timer.h"
#include "io.h"
#include "dim.h"

bool PWMrateFlag = 0,pwmincflag=1, DimDirection = 0; // 0-ramp down, 1-ramp up;
uint8_t PrevRLY = 0,CurveType = 0;
uint16_t InitLevel=0, CurrentLevel = 0, SetLevel = 0, pwmStepMax = 0;
static uint16_t pwmLinearRate = 0, pwmStep = 0, pwmDimStep = 0, pwmSqrtRate = 0;
double pwmSqRate = 0;

/*******************************************************************************
 calcTotalPWMStep(uint8_t transitionTime)

uint16_t total step# (from 0% to 100%) =
Transition time x1000 (in ms) / 8ms timer period

 return: uint16_t PWM step number
*******************************************************************************/
uint16_t calcPWMStep(uint8_t transitionTime)
{
  return(transitionTime * 125);
}

/*******************************************************************************
 calcPWMSqOut(uint16_t step, double sqrate)

Calculate square curve output PWM duty cycle value
based on y = kx^2
PWM duty cycle = sqrate x (step^2)

 return: uint16_t PWM duty cycle output value
*******************************************************************************/
uint32_t calcPWMSqOut(uint16_t step, double sqrate)
{
  uint32_t fout;
  MPYS =step;
  OP2 = step;
  fout = ((uint32_t)RESHI<<16)| RESLO;
  fout *= sqrate;
  return fout;
}

/*******************************************************************************
 calcPWMSqrtOut(uint16_t step, double sqrtrate)

Calculate square root curve output PWM duty cycle value
based on y = kx^(1/2)
PWM duty cycle = sqrtrate x (step^(1/2))

 return: uint16_t PWM duty cycle output value
*******************************************************************************/
uint16_t calcPWMSqrtOut(uint16_t step, uint16_t sqrtrate)
{
  uint16_t rout;
  rout = sqrt(step);
  MPYS = rout;
  OP2 = sqrtrate;
  rout = RESLO;
  return rout;
}

/*******************************************************************************
 uint16_t DimLinearRate(void)

In Dimming mode 2, calculate Linear PWM change rate =
PWM dutycycle/percentage level change/ total PWM step #

 return: uint16_t PWM linear curve rate/changing slope
*******************************************************************************/
uint16_t DimLinearRate(void)
{
  uint16_t x;
  if(SetLevel > CurrentLevel) x = (SetLevel-CurrentLevel)/pwmStepMax;
  else x = (CurrentLevel-SetLevel)/pwmStepMax;
  return x;
}

/*******************************************************************************
 double DimSquareRate(void)

In Dimming mode 2, calculate Square k rate =


 return: uint16_t PWM square curve changing rate
*******************************************************************************/
double DimSquareRate(void)
{
  uint32_t tempSq;
  double x;
  MPYS = pwmStepMax;
  OP2 = pwmStepMax;
  tempSq = ((uint32_t)RESHI<<16)| RESLO;
  if(SetLevel > CurrentLevel) x= (SetLevel - CurrentLevel)/(double)tempSq;
  else x = (CurrentLevel - SetLevel)/(double)tempSq;
  return x;   
}

/*******************************************************************************
 uint16_t DimSqrtRate(void)

In Dimming mode 2, calculate Square Root k rate =


 return: uint16_t PWM square root curve changing rate
*******************************************************************************/
uint16_t DimSqrtRate(void)
{
  double tempSqrt;
  uint16_t x;
  tempSqrt = sqrt(pwmStepMax);
  if(SetLevel > CurrentLevel) x = (SetLevel-CurrentLevel)/tempSqrt;
  else x = (CurrentLevel-SetLevel)/tempSqrt;
  return x;
}

/*******************************************************************************
 uint16_t calcPWMOutVal(void)

Calculate PWM duty cycle output
x is steps# required to ramp (500steps in 4s)
y(x) = kx^2 -> k=0.26214
y(x) = kx^(1/2) -> k=2930

*******************************************************************************/
uint16_t calcPWMOutVal(void)
{
  uint32_t fOut;
  uint16_t rOut;
  switch(DeviceCfg->curveType)
  {
    // Linear Curve*************************************************************
    case CURVE_LINEAR:
      if(DimMode == DIM_MODE_DIM) // if in dim mode
      {
        if(PWMrateFlag){ // run only first time enter Dim mode
          pwmLinearRate = DimLinearRate();
          PWMrateFlag = false;
        }
        if(DimDirection) {
          TA0CCR1 += pwmLinearRate;      // ramp up
          if(TA0CCR1 > (SetLevel - pwmLinearRate)) TA0CCR1 = SetLevel;
        }
        else {
          TA0CCR1 -= pwmLinearRate;     // ramp down
          if(TA0CCR1 < (SetLevel + pwmLinearRate)) TA0CCR1 = SetLevel;
        }
      }
      else {// in other modes
        if(DimDirection){       // ramp up
          TA0CCR1 += PWMLINRATEDEF;
          pwmStep += 1;
          if(pwmStep > PWMSTEPMAXDEF) pwmStep = PWMSTEPMAXDEF;
        }
        else{                   // ramp down
          TA0CCR1 -= PWMLINRATEDEF;
          pwmStep -=1;
        }
      }
    break;
    
    // Square Curve*************************************************************
    case CURVE_SQUARE:
      if(DimMode == DIM_MODE_DIM)// if in dim mode
      {
        if(PWMrateFlag){ // run only first time enter Dim mode
          pwmSqRate = DimSquareRate();
          PWMrateFlag = false;
        }
        if(DimDirection){
          pwmDimStep += 1;
          if(pwmDimStep == pwmStepMax) {
            TA0CCR1 = SetLevel;
            return 0;
          }
          fOut = calcPWMSqOut(pwmDimStep, pwmSqRate);
          fOut += InitLevel;
          if(fOut>TA0CCR1) TA0CCR1 = fOut;
        }
        else{
          pwmDimStep -= 1;
          if(pwmDimStep == 0) {
            TA0CCR1 = SetLevel;
            return 0;
          }
          fOut = calcPWMSqOut(pwmDimStep, pwmSqRate);
          fOut += InitLevel;
          if(fOut<TA0CCR1) TA0CCR1 = fOut;
        }
      }
      else { // in other modes
        if(DimDirection){
          pwmStep +=1;
          if(pwmStep > PWMSTEPMAXDEF) pwmStep = PWMSTEPMAXDEF;
          fOut = calcPWMSqOut(pwmStep, PWMSQRATEDEF);
          if(fOut>TA0CCR1) TA0CCR1 = fOut;
        }
        else{
          pwmStep -=1;
          fOut = calcPWMSqOut(pwmStep, PWMSQRATEDEF);
          if(fOut<TA0CCR1) TA0CCR1 = fOut;
        }
      }
      break;
    
    // Square root Curve********************************************************
    case CURVE_SQRT:
      if(DimMode == DIM_MODE_DIM) // if in dim mode
      {
        if(PWMrateFlag){
          pwmSqrtRate = DimSqrtRate();
          PWMrateFlag = false;
        }
        if(DimDirection){
          pwmDimStep += 1;
          if(pwmDimStep == pwmStepMax) {
            TA0CCR1 = SetLevel;
            return 0;
          }
          rOut = calcPWMSqrtOut(pwmDimStep, pwmSqrtRate);
          rOut += InitLevel;
          if(rOut>TA0CCR1) TA0CCR1 = rOut;
        }
        else{
          pwmDimStep -= 1;
          if(pwmDimStep == 0) {
            TA0CCR1 = SetLevel;
            return 0;
          }
          rOut = calcPWMSqrtOut(pwmDimStep, pwmSqrtRate);
          rOut += InitLevel;
          if(rOut<TA0CCR1) TA0CCR1 = rOut;
        }
      }
      else { // in other modes
        if(DimDirection){
          pwmStep +=1;
          if(pwmStep > PWMSTEPMAXDEF) pwmStep = PWMSTEPMAXDEF;
          rOut = calcPWMSqrtOut(pwmStep, PWMSQRTRATEDEF);
          if(rOut>TA0CCR1) TA0CCR1 = rOut;
        }
        else{
          pwmStep -=1;
          rOut = calcPWMSqrtOut(pwmStep, PWMSQRTRATEDEF);
          if(rOut<TA0CCR1) TA0CCR1 = rOut;
        }
      }
      break;
  }
  return 0;
}

/*******************************************************************************
 uint16_t calcRevPWMStep(void)

Reverse back pwm step # when switch from dimming mode (2) to other mode
switch back the default curve rates

*******************************************************************************/
uint16_t calcRevPWMStep(void)
{
  uint16_t y, tempx;
  uint32_t tx;
  switch(DeviceCfg->curveType)
  {
    // Linear Curve*************************************************************
    case CURVE_LINEAR:
      y = TA0CCR1/PWMLINRATEDEF;
      break;
    // Square Curve*************************************************************
    case CURVE_SQUARE:
      tx = TA0CCR1/PWMSQRATEDEF;
      y = sqrt(tx);
      break;
    // Square root Curve********************************************************
    case CURVE_SQRT:
      tempx = TA0CCR1/PWMSQRTRATEDEF;
      MPYS = tempx;
      OP2 = tempx;
      y = RESLO;
      break;
  }
  return y;
}

/*******************************************************************************
  void DoDimFunction(void)

Dimming function state machine

*******************************************************************************/
void DoDimFunction(void)
{
        switch(DimMode){
          
          // ON/OFF Mode********************************************************
          case DIM_MODE_ON:
            if(DeviceCfg->softDimOn == 0){
              TA0CCR1 = PWM_FREQ;
              pwmStep = PWMSTEPMAXDEF;
              RLY_ON;
              lcp_dim_flag = false;
              lcp_state_report = true;
            }
            else{ // Soft ON
              RLY_ON;
              DimDirection = 1; // ramp up
              calcPWMOutVal();
              if((TA0CCR1 >(PWM_FREQ-1)) || (pwmStep==PWMSTEPMAXDEF)){
                TA0CCR1 = PWM_FREQ;
                lcp_dim_flag = false;
                lcp_state_report = true;
              }
            }
            break;
          case DIM_MODE_OFF:
            if(DeviceCfg->softDimOn == 0){
              RLY_OFF;
              TA0CCR1 = 0;
              pwmStep = 0;
              lcp_dim_flag = false;
              lcp_state_report = true;
            }
            else{ // Soft OFF
              DimDirection = 0; // ramp down
              if((TA0CCR1 < 1) || (pwmStep==0)){
                TA0CCR1 = 0;
                RLY_OFF;
                lcp_dim_flag = false; 
                lcp_state_report = true;
              }
              else calcPWMOutVal();
            }
            break;
          
          // Dim Mode***********************************************************
          case DIM_MODE_DIM: 
            RLY_ON;
            if(TA0CCR1 == SetLevel){
              PWMrateFlag = false;
              lcp_dim_flag = false;
              lcp_state_report = true;
            }
            else {
              if(PWMrateFlag)
                pwmStepMax = calcPWMStep(TransitionTime);
              if(TA0CCR1 < SetLevel){
                DimDirection = 1; // ramp up
                if(PWMrateFlag){
                  InitLevel = CurrentLevel;
                  pwmDimStep = 0;
                }
                calcPWMOutVal();
              }
              else{
                DimDirection = 0; // ramp down
                if(PWMrateFlag){
                  InitLevel = SetLevel;
                  pwmDimStep = pwmStepMax;
                }
                calcPWMOutVal();
              }
            }
            if(lcp_dim_flag == 0) pwmStep = calcRevPWMStep();
            break;
            
          // Dim Start**********************************************************
          case DIM_MODE_START:
              if(Direction){
                RLY_ON;
                DimDirection = 1; // ramp up
                if((TA0CCR1 >(PWM_FREQ-1)) || (pwmStep==PWMSTEPMAXDEF)){
                  TA0CCR1 = PWM_FREQ;
                  lcp_dim_flag = false;
                  lcp_state_report = true;
                }
                else calcPWMOutVal();
              }
              else{
                DimDirection = 0; // ramp down
                if((TA0CCR1 < 1)||(pwmStep==0)){
                  TA0CCR1 = 0;
                  lcp_dim_flag = false;
                  lcp_state_report = true;
                }
                else calcPWMOutVal();
              }
            break;
          case DIM_MODE_STOP:
            lcp_dim_flag = false;
            lcp_state_report = true;
            break;
        }
        
        CurrentLevel = TA0CCR1;
}

/*******************************************************************************
  void DoButtonDim(void)

1 short 1 long press dimming in curve fashion

*******************************************************************************/
void DoButtonDim(void)
{
  if(pwmincflag){
    DimDirection = 1;
    if((TA0CCR1 >(PWM_FREQ-1)) || (pwmStep==PWMSTEPMAXDEF)){
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

/*******************************************************************************
  void DoButtonTogglevoid)

2 short press relay toggle (soft/hard)

*******************************************************************************/
void DoButtonToggle(void)
{
  lcp_dim_flag = true;
  if(RLY_READ == 0){
    DimMode = DIM_MODE_ON;
  }
  else{
    DimMode = DIM_MODE_OFF;
  }
  CurrentLevel = TA0CCR1;               // record current PWM duty cycle level
}

