/*****************************************************************************
*
*   File name: adc.c
*
*   LC - Controller
*   Initialize SD24 Initialization
*   Gain up to 16x, input range Vfsr = Vref/Gain, Oversampling ratio up to 256
*
*****************************************************************************/
#include <inttypes.h>
#include  <stdbool.h>
#include "msp430.h"
#include "io.h"
#include "adc.h"
#include "global.h"
#include "serial.h"
#define ADC_DEF
#include "lcp.h"
#include "lcp_constants.h"

/*! The minimum value which the ADCs can produce. */
#define V_ADC_MIN                       -31000
#define I_ADC_MIN                       (-31000L*256L)
/*! The maximum value which the ADCs can produce. */
#define V_ADC_MAX                       31000
#define I_ADC_MAX                       (31000L*256L)
#define ENDSTOP_HITS_FOR_OVERLOAD       20 //fast overload tolerance times
#define SAMPLE_RATE                     134//(39062UL) - 1s sample
#define REPORT_RATE                     3600 // - 1min report
#define V_SCALE_FACTOR                  24022
#define TRIGGER_THRESHOLD_H             500 // 50mA
#define TRIGGER_THRESHOLD_L             200


bool ADCInitFlag = 1, TriggerFlag = 0; 
unsigned char V_endstops = ENDSTOP_HITS_FOR_OVERLOAD, *current_freq_ptr;
char iV_history_index;
uint16_t index_a=0, report_cycle = 0, index_f=0, tV, tI;
uint16_t vfreq_counter_buf, freq_counter=0, vfreq_counter_history=0, ifreq_counter_buf, ifreq_counter_history=0;
int16_t iV, iI, iV_dc=0, iV_diff, iI_dc, iV_max=0, iV_min=0, iI_max=0, iI_min=0, V_init_estimate_dc = 34, iV_history, iI_history, iVh[134];
int32_t tV_accum, tI_accum, iP_mpy;
uint64_t iV_mpy, iI_mpy, iV_accum, iI_accum;
uint32_t iV_div, iV_sqrt, iI_div, iI_sqrt, tP;
//uint16_t tV_accum[3], tV_accum_history[3];
uint16_t send_V, send_I, send_P, send_F, send_PF, send_THD;
/*
int32_t dc_filter16_init(int16_t x)
{
    return((int32_t) x << 16);
}

int16_t dc_filter16(int32_t y, int16_t x)
{
    y += ((((int32_t) x << 16) - y) >> 14);
    x -= (y >> 16);
    return x;
}

void sqac48_16(register int16_t z[3], register int16_t x)
{
    int64_t tmp;

    tmp = z[2];
    tmp = tmp << 16;
    tmp |= z[1];
    tmp = tmp << 16;
    tmp |= z[0];
    tmp += (int32_t) x*(int32_t) x;
    z[2] = (tmp >> 32) & 0xFFFF;
    z[1] = (tmp >> 16) & 0xFFFF;
    z[0] = tmp & 0xFFFF;
}

int32_t div48(int16_t x[3], int16_t y)
{
    // Divide a 16 bit integer into a 48 bit one. Expect the answer to be no
    // greater than 32 bits, so return the answer as a 32 bit integer.
    // A somewhat domain specific divide operation, but pretty useful when
    // handling dot products.
    int32_t x1;
    int32_t z;

    // Avoid any divide by zero trouble
    if (y == 0)
        return 0;
    x1 = x[2]%y;
    x1 <<= 16;
    x1 |= (uint16_t) x[1];
    z = x1/y;
    x1 = x1%y;
    x1 <<= 16;
    x1 |= (uint16_t) x[0];
    z = (z << 16) + x1/y;
    return z;
}

unsigned long isqrt32(uint32_t h)
{
    uint32_t x;
    uint32_t y;
    int i;

    x = 0x80000000;
    y = 0;
    for (i = 0;  i < 32;  i++)
    {
        x = (x << 1) | 1;
        if (y < x)
            x -= 2;
        else
            y -= x;
        x++;
        y <<= 1;
        if ((h & 0x80000000))
            y |= 1;
        h <<= 1;
        y <<= 1;
        if ((h & 0x80000000))
            y |= 1;
        h <<= 1;
    }

    return  x;
}

uint32_t mul48u_32_16(uint32_t x, uint16_t y)
{
    uint64_t z;
    
    z = (int64_t) x*(int64_t) y;
    return z >> 16;
}
*/
/*void v_rms_calc(void)
{
    tmp = div48(tV_accum_history,134);
    tV = isqrt32(tmp);
    tV = mul48u_32_16(tV, V_SCALE_FACTOR) >> 10;
    tV = tV;
}*/

/*void v_rms_calc(void)
{
  if(report_cycle == REPORT_RATE){
    report_cycle = 0;
    send_V = tV_accum/14220;    //237.12;
    send_I = tI_accum/24000;    //400.23;
    send_P = send_V * send_I;
    tI_accum = 0;
    tV_accum = 0;
  }
  else{
    report_cycle++;
    tV_accum += tV;
    tI_accum += tI;
  }
}*/

int per_sample_dsp()
{

  MPYS = iV;
  OP2 = iV;
  iV_accum += ((uint32_t)RESHI<<16)| RESLO;
  MPYS = iI;
  OP2 = iI;
  iI_accum += ((uint32_t)RESHI<<16)| RESLO;
  
  freq_counter++;
  if((iV_history > 0) && (iV < 0)){
    if(ADCInitFlag == 0){
      vfreq_counter_buf += (freq_counter - vfreq_counter_history);
      index_f++;
    }
    vfreq_counter_history = freq_counter;
  }
  if((iI_history > 0) && (iI < 0)){
    if(ADCInitFlag == 0){
      ifreq_counter_buf += (ifreq_counter_history - vfreq_counter_history);
    }
    ifreq_counter_history = freq_counter;
  }
  if(index_f == 16){
    index_f = 0;
    freq_counter = 0;
    send_F = vfreq_counter_buf;
    send_PF = ifreq_counter_buf ;
  }    
  
  iV_history = iV;
  iI_history = iI;
  
  if(iV > iV_max) iV_max = iV;
  if(iV < iV_min) iV_min = iV;
  if(iI > iI_max) iI_max = iI;
  if(iI < iI_min) iI_min = iI;                        
  index_a++;
  
  if(index_a == SAMPLE_RATE){
              tV = iV_max - iV_min;
              tI = iI_max - iI_min;
              tI_accum += tI;
              tV_accum += tV;
              SampleReadyFlag = 1;
              iV_max = 0;
              iI_max = 0;
              iV_min = 0;
              iI_min = 0;
              index_a=0;
  }
  if(report_cycle == REPORT_RATE){
                report_cycle = 0;
                send_V = tV_accum>>5;
                send_I = tI_accum/76;
                MPYS = send_V;
                OP2 = send_I;
                iP_mpy = ((uint32_t)RESHI<<16)| RESLO;
                send_P = iP_mpy/303;
                tV_accum = 0;
                tI_accum = 0;
  }
  else{
    report_cycle++;
  }
  
  
  if(ControllerType == LC_CONTROLLER_TYPE_TRIGGER){
    /*if(tI > TRIGGER_THRESHOLD_H){
      lcp_dim_flag = false;
      lcp_state_report = true;
    }*/
    if(TriggerFlag == 0){
      if(tI > TRIGGER_THRESHOLD_H){
        TriggerFlag = 1;
        lcp_dim_flag = false;
        lcp_state_report = true;
      }
    }
    else {
      if(tI < TRIGGER_THRESHOLD_L){
        TriggerFlag = 0;
        lcp_dim_flag = false;
        lcp_state_report = true;
      }
    }
  }
  
  ADCInitFlag = 0;
  return 0;
}

//SD24 Initiliazation
void adc_init()
{
#ifdef LC_CONTROLLER
  SD24CTL = SD24REFS;                           // Internal 1.2V ref,overflow interrupt disabled
  
  SD24CCTL0 |= SD24GRP | SD24DF | SD24OSR_128;  // Cont,OSR128,DisInt,2sCompliment,GroupW/Ch1
  //SD24CCTL0 |= SD24GRP | SD24OSR_128; 
  SD24INCTL0 |= SD24INCH_0 + SD24GAIN_1;        // Inch_A0.0,Gainx1,FourthSampleInterrupt
  
  SD24CCTL1 |= SD24IE | SD24DF | SD24OSR_128 | SD24LSBACC;   // Cont,OSR128,2sCompliment,EnableInt
  //SD24CCTL1 |= SD24IE | SD24OSR_128;
  SD24INCTL1 |= SD24INCH_0 + SD24GAIN_1;       // Inch_A1.0,Gainx16,FourthSampleInterrupt
  
  //__delay_cycles(3200);                         // Delay ~200us for 1.2V ref to settle
  
  SD24CCTL1 |= SD24SC;                          // Set bit to start conversion
  //V_init_dc = dc_filter16_init(V_init_estimate_dc);
  
#elif LC_OCCUPANCY
     SD24CTL = SD24REFS;                           // Internal 1.2V ref,overflow interrupt disabled
  
  SD24CCTL0 |= SD24GRP | SD24DF | SD24OSR_128;  // Cont,OSR128,DisInt,2sCompliment,GroupW/Ch1
  //SD24CCTL0 |= SD24GRP | SD24OSR_128; 
  SD24INCTL0 |= SD24INCH_0 + SD24GAIN_1;        // Inch_A0.0,Gainx1,FourthSampleInterrupt
  
  SD24CCTL1 |= SD24IE | SD24DF | SD24OSR_128 | SD24LSBACC;   // Cont,OSR128,2sCompliment,EnableInt
  //SD24CCTL1 |= SD24IE | SD24OSR_128;
  SD24INCTL1 |= SD24INCH_0 + SD24GAIN_1;       // Inch_A1.0,Gainx16,FourthSampleInterrupt
  
  //__delay_cycles(3200);                         // Delay ~200us for 1.2V ref to settle
  
  SD24CCTL1 |= SD24SC;                          // Set bit to start conversion

  SD24CCTL2  	|= SD24GRP | SD24DF;                                            // Enable interrupt
  SD24CCTL3  	|= SD24IE | SD24DF;                                             // Enable interrupt
  SD24CTL 	 = SD24REFS;                                                    // Internal ref
  SD24CCTL3 	|= SD24SC;                                                      // Set bit to start conversion
  
#endif
}

/******************************************************
// SD24 interrupt service routine sample rate 8khz
******************************************************/
#pragma vector = SD24_VECTOR
__interrupt void SD24_ISR(void)
{
  
#ifdef LC_CONTROLLER
  switch (__even_in_range(SD24IV,SD24IV_SD24MEM1)) {
        case SD24IV_NONE: break;
        case SD24IV_SD24OVIFG:  break;
        case SD24IV_SD24MEM0:   break;
        case SD24IV_SD24MEM1:
            iV = SD24MEM0;                // Save result (clears IFG)
            iI = SD24MEM1;                // Save result (clears IFG)
            //SD24CCTL1 &= ~SD24LSBACC;
            //iI = (int16_t) SD24MEM1 << 8;
            //SD24CCTL1 |= SD24LSBACC;
            //iI += (int16_t) SD24MEM1 << 3;
            
            iV -= 28;
            iI -= 780;
            
            //if((iV >= V_ADC_MAX) || (iV<= V_ADC_MIN) && V_endstops)
             // V_endstops--;
            
            per_sample_dsp();
            
            break;
        default: break;
  }
  
#elif LC_OCCUPANCY
   switch (__even_in_range(SD24IV,SD24IV_SD24MEM1)) {
        case SD24IV_NONE: break;
        case SD24IV_SD24OVIFG:  break;
        case SD24IV_SD24MEM0:   break;
        case SD24IV_SD24MEM1:
            
          ipirresult = SD24MEM0;        //PIR sensor Value
            
            iV = SD24MEM2;                // Save result (clears IFG)
            iI = SD24MEM3;                // Save result (clears IFG)
            iVh[index_a] = iV;
            iV -= 68;
            iI -= 1025;
            per_sample_dsp();
        break;
        default: break;
  }
 
#endif
  SD24CCTL1 &= ~SD24IFG;                        // clear interrupt flag
}
