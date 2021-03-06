/*
 * b747cl.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "b747cl".
 *
 * Model version              : 1.188
 * Simulink Coder version : 8.13 (R2017b) 24-Jul-2017
 * C source code generated on : Sun Oct 29 22:21:17 2017
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "b747cl.h"
#include "b747cl_private.h"


/* Block signals (auto storage) */
B_b747cl_T b747cl_B;

/* Block states (auto storage) */
DW_b747cl_T b747cl_DW;

/* Real-time model */
RT_MODEL_b747cl_T b747cl_M_;
RT_MODEL_b747cl_T *const b747cl_M = &b747cl_M_;


/*
 * Time delay interpolation routine
 *
 * The linear interpolation is performed using the formula:
 *
 *          (t2 - tMinusDelay)         (tMinusDelay - t1)
 * u(t)  =  ----------------- * u1  +  ------------------- * u2
 *              (t2 - t1)                  (t2 - t1)
 */
real_T rt_TDelayInterpolate(
  real_T tMinusDelay,                  /* tMinusDelay = currentSimTime - delay */
  real_T tStart,
  real_T *tBuf,
  real_T *uBuf,
  int_T bufSz,
  int_T *lastIdx,
  int_T oldestIdx,
  int_T newIdx,
  real_T initOutput,
  boolean_T discrete,
  boolean_T minorStepAndTAtLastMajorOutput)
{
  int_T i;
  real_T yout, t1, t2, u1, u2;

  /*
   * If there is only one data point in the buffer, this data point must be
   * the t= 0 and tMinusDelay > t0, it ask for something unknown. The best
   * guess if initial output as well
   */
  if ((newIdx == 0) && (oldestIdx ==0 ) && (tMinusDelay > tStart))
    return initOutput;

  /*
   * If tMinusDelay is less than zero, should output initial value
   */
  if (tMinusDelay <= tStart)
    return initOutput;

  /* For fixed buffer extrapolation:
   * if tMinusDelay is small than the time at oldestIdx, if discrete, output
   * tailptr value,  else use tailptr and tailptr+1 value to extrapolate
   * It is also for fixed buffer. Note: The same condition can happen for transport delay block where
   * use tStart and and t[tail] other than using t[tail] and t[tail+1].
   * See below
   */
  if ((tMinusDelay <= tBuf[oldestIdx] ) ) {
    if (discrete) {
      return(uBuf[oldestIdx]);
    } else {
      int_T tempIdx= oldestIdx + 1;
      if (oldestIdx == bufSz-1)
        tempIdx = 0;
      t1= tBuf[oldestIdx];
      t2= tBuf[tempIdx];
      u1= uBuf[oldestIdx];
      u2= uBuf[tempIdx];
      if (t2 == t1) {
        if (tMinusDelay >= t2) {
          yout = u2;
        } else {
          yout = u1;
        }
      } else {
        real_T f1 = (t2-tMinusDelay) / (t2-t1);
        real_T f2 = 1.0 - f1;

        /*
         * Use Lagrange's interpolation formula.  Exact outputs at t1, t2.
         */
        yout = f1*u1 + f2*u2;
      }

      return yout;
    }
  }

  /*
   * When block does not have direct feedthrough, we use the table of
   * values to extrapolate off the end of the table for delays that are less
   * than 0 (less then step size).  This is not completely accurate.  The
   * chain of events is as follows for a given time t.  Major output - look
   * in table.  Update - add entry to table.  Now, if we call the output at
   * time t again, there is a new entry in the table. For very small delays,
   * this means that we will have a different answer from the previous call
   * to the output fcn at the same time t.  The following code prevents this
   * from happening.
   */
  if (minorStepAndTAtLastMajorOutput) {
    /* pretend that the new entry has not been added to table */
    if (newIdx != 0) {
      if (*lastIdx == newIdx) {
        (*lastIdx)--;
      }

      newIdx--;
    } else {
      if (*lastIdx == newIdx) {
        *lastIdx = bufSz-1;
      }

      newIdx = bufSz - 1;
    }
  }

  i = *lastIdx;
  if (tBuf[i] < tMinusDelay) {
    /* Look forward starting at last index */
    while (tBuf[i] < tMinusDelay) {
      /* May occur if the delay is less than step-size - extrapolate */
      if (i == newIdx)
        break;
      i = ( i < (bufSz-1) ) ? (i+1) : 0;/* move through buffer */
    }
  } else {
    /*
     * Look backwards starting at last index which can happen when the
     * delay time increases.
     */
    while (tBuf[i] >= tMinusDelay) {
      /*
       * Due to the entry condition at top of function, we
       * should never hit the end.
       */
      i = (i > 0) ? i-1 : (bufSz-1);   /* move through buffer */
    }

    i = ( i < (bufSz-1) ) ? (i+1) : 0;
  }

  *lastIdx = i;
  if (discrete) {
    /*
     * tempEps = 128 * eps;
     * localEps = max(tempEps, tempEps*fabs(tBuf[i]))/2;
     */
    double tempEps = (DBL_EPSILON) * 128.0;
    double localEps = tempEps * fabs(tBuf[i]);
    if (tempEps > localEps) {
      localEps = tempEps;
    }

    localEps = localEps / 2.0;
    if (tMinusDelay >= (tBuf[i] - localEps)) {
      yout = uBuf[i];
    } else {
      if (i == 0) {
        yout = uBuf[bufSz-1];
      } else {
        yout = uBuf[i-1];
      }
    }
  } else {
    if (i == 0) {
      t1 = tBuf[bufSz-1];
      u1 = uBuf[bufSz-1];
    } else {
      t1 = tBuf[i-1];
      u1 = uBuf[i-1];
    }

    t2 = tBuf[i];
    u2 = uBuf[i];
    if (t2 == t1) {
      if (tMinusDelay >= t2) {
        yout = u2;
      } else {
        yout = u1;
      }
    } else {
      real_T f1 = (t2-tMinusDelay) / (t2-t1);
      real_T f2 = 1.0 - f1;

      /*
       * Use Lagrange's interpolation formula.  Exact outputs at t1, t2.
       */
      yout = f1*u1 + f2*u2;
    }
  }

  return(yout);
}

real_T rt_urand_Upu32_Yd_f_pw_snf(uint32_T *u)
{
  uint32_T lo;
  uint32_T hi;

  /* Uniform random number generator (random number between 0 and 1)

     #define IA      16807                      magic multiplier = 7^5
     #define IM      2147483647                 modulus = 2^31-1
     #define IQ      127773                     IM div IA
     #define IR      2836                       IM modulo IA
     #define S       4.656612875245797e-10      reciprocal of 2^31-1
     test = IA * (seed % IQ) - IR * (seed/IQ)
     seed = test < 0 ? (test + IM) : test
     return (seed*S)
   */
  lo = *u % 127773U * 16807U;
  hi = *u / 127773U * 2836U;
  if (lo < hi) {
    *u = 2147483647U - (hi - lo);
  } else {
    *u = lo - hi;
  }

  return (real_T)*u * 4.6566128752457969E-10;
}

real_T rt_nrand_Upu32_Yd_f_pw_snf(uint32_T *u)
{
  real_T y;
  real_T sr;
  real_T si;

  /* Normal (Gaussian) random number generator */
  do {
    sr = 2.0 * rt_urand_Upu32_Yd_f_pw_snf(u) - 1.0;
    si = 2.0 * rt_urand_Upu32_Yd_f_pw_snf(u) - 1.0;
    si = sr * sr + si * si;
  } while (si > 1.0);

  y = sqrt(-2.0 * log(si) / si) * sr;
  return y;
}

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T y;
  real_T tmp;
  real_T tmp_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = 1.0;
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Model output function */
 ;
static void b747cl_output ( )
{
real_T rtb_Phi ;
real_T rtb_chi ;
real_T rtb_fpa ;
real_T rtb_gamma ;
real_T rtb_Vc ;
real_T rtb_Ve ;
real_T rtb_Tt ;
real_T rtb_cbar ;
real_T rtb_w ;
int_T ci ;
real_T rtb_WhiteNoise [ 40 ] ;
real_T rtb_Saturation4 ;
real_T rtb_Saturation1 ;
real_T rtb_Saturation2 ;
real_T rtb_S [ 6 ] ;
real_T rtb_psidot ;
real_T rtb_axk ;
real_T rtb_u [ 12 ] ;
real_T rtb_wdot ;
real_T rtb_MatrixGain_a [ 3 ] ;
real_T rtb_MatrixGain [ 2 ] ;
real_T rtb_MatrixGain_f [ 2 ] ;
real_T TmpSignalConversionAtMatrixGain [ 19 ] ;
real_T TmpSignalConversionAtMatrixGa_m [ 9 ] ;
real_T Sum3 [ 5 ] ;
int32_T i ;
real_T tmp [ 6 ] ;
real_T tmp_0 [ 18 ] ;
real_T tmp_1 [ 18 ] ;
real_T tmp_2 [ 18 ] ;
real_T tmp_3 [ 18 ] ;
real_T tmp_4 [ 18 ] ;
real_T tmp_5 [ 9 ] ;
real_T tmp_6 [ 9 ] ;
real_T tmp_7 [ 9 ] ;
real_T tmp_8 [ 9 ] ;
real_T tmp_9 [ 9 ] ;
real_T tmp_a [ 9 ] ;
real_T rtb_u_0 [ 23 ] ;
real_T tmp_b [ 29 ] ;
real_T tmp_c [ 29 ] ;
real_T tmp_d [ 29 ] ;
real_T tmp_e [ 29 ] ;
real_T tmp_f [ 30 ] ;
real_T Sum1_idx_2 ;
real_T Ygr_tmp ;
real_T Zgr_tmp ;
memcpy ( & b747cl_B . DiscreteTimeIntegrator [ 0 ] , & b747cl_DW . DiscreteTimeIntegrator_DSTATE [ 0 ] , 12U * sizeof ( real_T )  
) ;
rt_UpdateLogVar ( (  LogVar * ) (  LogVar * ) ( b747cl_DW . AircraftStates_PWORK . LoggedData ) , & b747cl_B . DiscreteTimeIntegrator [ 0 ] , 0 ) ;
rt_UpdateLogVar ( (  LogVar * ) (  LogVar * ) ( b747cl_DW . ToWorkspace1_PWORK . LoggedData ) , & b747cl_B . DiscreteTimeIntegrator [ 0 ] , 0 ) ;
rt_UpdateLogVar ( (  LogVar * ) (  LogVar * ) ( b747cl_DW . ToWorkspace2_PWORK . LoggedData ) , & b747cl_B . DiscreteTimeIntegrator [ 7 ] , 0 ) ;
rt_UpdateLogVar ( (  LogVar * ) (  LogVar * ) ( b747cl_DW . ToWorkspace3_PWORK . LoggedData ) , & b747cl_B . DiscreteTimeIntegrator [ 11 ] , 0 ) ;
for ( i = 0 ; i < 5 ; i ++ )
{
b747cl_B . y0125812 [ i ] = b747cl_P . y0125812_Value [ i ] ;
}

for ( i = 0 ; i < 40 ; i ++ )
{
b747cl_B . Output [ i ] = b747cl_P . Output_Gain [ i ] * b747cl_DW . NextOutput [ i ] ;
rtb_WhiteNoise [ i ] = b747cl_DW . NextOutput [ i ] ;
}

for ( i = 0 ; i < 12 ; i ++ )
{
b747cl_B . u [ i ] = b747cl_P . u_Gain * b747cl_B . DiscreteTimeIntegrator [ i ] ;
}

for ( i = 0 ; i < 12 ; i ++ )
{
b747cl_B . u_a [ i ] = b747cl_P . u_Gain_o * b747cl_B . DiscreteTimeIntegrator [ i ] ;
}

b747cl_B . g = rt_powd_snf ( 6371020.0 / ( 6371020.0 + b747cl_B . u_a [ 11 ] ) , 2.0 ) * 9.80665 ;
b747cl_B . T = 288.15 - 0.0065 * b747cl_B . u_a [ 11 ] ;
rtb_Saturation4 = b747cl_B . g / 1.86584 ;
rtb_Saturation1 = b747cl_B . T / 288.15 ;
if ( ( rtb_Saturation1 < 0.0 ) && ( rtb_Saturation4 > floor ( rtb_Saturation4 ) ) ) {
rtb_Saturation1 = - rt_powd_snf ( - rtb_Saturation1 , rtb_Saturation4 ) ;
}
else {
rtb_Saturation1 = rt_powd_snf ( rtb_Saturation1 , rtb_Saturation4 ) ;
}

b747cl_B . ps = 101325.0 * rtb_Saturation1 ;
b747cl_B . rho = b747cl_B . ps / ( 287.053 * b747cl_B . T ) ;
if ( b747cl_B . T < 0.0 ) {
Sum1_idx_2 = - rt_powd_snf ( - b747cl_B . T , 1.5 ) ;
}
else {
Sum1_idx_2 = rt_powd_snf ( b747cl_B . T , 1.5 ) ;
}

b747cl_B . mu = 1.4579999999999998E-6 * Sum1_idx_2 / ( b747cl_B . T + 110.4 ) ;
rtb_Saturation4 = 401.8743 * b747cl_B . T ;
if ( rtb_Saturation4 < 0.0 ) {
b747cl_B . a = - sqrt ( - rtb_Saturation4 ) ;
}
else {
b747cl_B . a = sqrt ( rtb_Saturation4 ) ;
}

b747cl_B . M = b747cl_B . u_a [ 0 ] / b747cl_B . a ;
b747cl_B . qdyn = 0.5 * b747cl_B . rho * rt_powd_snf ( b747cl_B . u_a [ 0 ] , 2.0 ) ;
Sum1_idx_2 = cos ( b747cl_B . u_a [ 1 ] ) ;
b747cl_B . cosalpha = Sum1_idx_2 ;
b747cl_B . const_e = b747cl_P . const_Value ;
b747cl_B . Fcn = rt_powd_snf ( b747cl_B . u_a [ 1 ] , 2.0 ) ;
b747cl_B . Fcn1 = rt_powd_snf ( b747cl_B . u_a [ 1 ] , 3.0 ) ;
b747cl_B . Fcn2 = rt_powd_snf ( b747cl_B . u_a [ 2 ] , 2.0 ) ;
b747cl_B . Fcn3 = rt_powd_snf ( b747cl_B . u_a [ 2 ] , 3.0 ) ;
b747cl_B . pb2V = 0.5 * b747cl_B . u_a [ 3 ] * 59.74 / b747cl_B . u_a [ 0 ] ;
b747cl_B . qc2V = 0.5 * b747cl_B . u_a [ 4 ] * 8.32 / b747cl_B . u_a [ 0 ] ;
b747cl_B . rb2V = 0.5 * b747cl_B . u_a [ 5 ] * 59.74 / b747cl_B . u_a [ 0 ] ;
{
real_T  * * uBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK . TUbufferPtrs [ 0 ] ;
real_T  * * tBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = b747cl_M -> Timing . t [ 0 ] ;
real_T tMinusDelay = simTime - b747cl_P . delay_Delay ;
b747cl_B . delay = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , b747cl_DW . delay_IWORK . CircularBufSize , & b747cl_DW . delay_IWORK . Last , b747cl_DW . delay_IWORK . Tail , b747cl_DW . delay_IWORK . Head , ( b747cl_P . delay_InitOutput ) , 0 , 0 ) ;
}
{
b747cl_B . DiscreteStateSpace = b747cl_P . DiscreteStateSpace_C * b747cl_DW . DiscreteStateSpace_DSTATE ;
b747cl_B . DiscreteStateSpace += b747cl_P . DiscreteStateSpace_D * b747cl_B . delay ;
;
}
if ( b747cl_DW . LastMajorTime == ( rtInf ) ) {
b747cl_B . RateLimiter1 = b747cl_B . DiscreteStateSpace ;
}
else {
rtb_psidot = b747cl_M -> Timing . t [ 0 ] - b747cl_DW . LastMajorTime ;
rtb_axk = rtb_psidot * b747cl_P . RateLimiter1_RisingLim ;
rtb_Saturation2 = b747cl_B . DiscreteStateSpace - b747cl_DW . PrevY ;
if ( rtb_Saturation2 > rtb_axk ) {
b747cl_B . RateLimiter1 = b747cl_DW . PrevY + rtb_axk ;
}
else {
rtb_psidot *= b747cl_P . RateLimiter1_FallingLim ;
if ( rtb_Saturation2 < rtb_psidot ) {
b747cl_B . RateLimiter1 = b747cl_DW . PrevY + rtb_psidot ;
}
else {
b747cl_B . RateLimiter1 = b747cl_B . DiscreteStateSpace ;
}

}

}

if ( b747cl_B . RateLimiter1 > b747cl_P . Saturation4_UpperSat ) {
rtb_Saturation4 = b747cl_P . Saturation4_UpperSat ;
}
else if ( b747cl_B . RateLimiter1 < b747cl_P . Saturation4_LowerSat ) {
rtb_Saturation4 = b747cl_P . Saturation4_LowerSat ;
}
else {
rtb_Saturation4 = b747cl_B . RateLimiter1 ;
}


b747cl_B . stabs = b747cl_P . stabs_Value ;
{
real_T  * * uBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK_f . TUbufferPtrs [ 0 ] ;
real_T  * * tBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK_f . TUbufferPtrs [ 1 ] ;
real_T simTime = b747cl_M -> Timing . t [ 0 ] ;
real_T tMinusDelay = simTime - b747cl_P . delay_Delay_m ;
b747cl_B . delay_h = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , b747cl_DW . delay_IWORK_j . CircularBufSize , & b747cl_DW . delay_IWORK_j . Last , b747cl_DW . delay_IWORK_j . Tail , b747cl_DW . delay_IWORK_j . Head , b747cl_P . delay_InitOutput_b , 0 , 0 ) ;
}
{
b747cl_B . DiscreteStateSpace_f = b747cl_P . DiscreteStateSpace_C_l * b747cl_DW . DiscreteStateSpace_DSTATE_g ;
b747cl_B . DiscreteStateSpace_f += b747cl_P . DiscreteStateSpace_D_m * b747cl_B . delay_h ;
;
}
if ( b747cl_DW . LastMajorTime_l == ( rtInf ) ) {
b747cl_B . RateLimiter2 = b747cl_B . DiscreteStateSpace_f ;
}
else {
rtb_psidot = b747cl_M -> Timing . t [ 0 ] - b747cl_DW . LastMajorTime_l ;
rtb_axk = rtb_psidot * b747cl_P . RateLimiter2_RisingLim ;
rtb_Saturation2 = b747cl_B . DiscreteStateSpace_f - b747cl_DW . PrevY_o ;
if ( rtb_Saturation2 > rtb_axk ) {
b747cl_B . RateLimiter2 = b747cl_DW . PrevY_o + rtb_axk ;
}
else {
rtb_psidot *= b747cl_P . RateLimiter2_FallingLim ;
if ( rtb_Saturation2 < rtb_psidot ) {
b747cl_B . RateLimiter2 = b747cl_DW . PrevY_o + rtb_psidot ;
}
else {
b747cl_B . RateLimiter2 = b747cl_B . DiscreteStateSpace_f ;
}

}

}

if ( b747cl_B . RateLimiter2 > b747cl_P . Saturation1_UpperSat ) {
rtb_Saturation1 = b747cl_P . Saturation1_UpperSat ;
}
else if ( b747cl_B . RateLimiter2 < b747cl_P . Saturation1_LowerSat ) {
rtb_Saturation1 = b747cl_P . Saturation1_LowerSat ;
}
else {
rtb_Saturation1 = b747cl_B . RateLimiter2 ;
}


{
real_T  * * uBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK_e . TUbufferPtrs [ 0 ] ;
real_T  * * tBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK_e . TUbufferPtrs [ 1 ] ;
real_T simTime = b747cl_M -> Timing . t [ 0 ] ;
real_T tMinusDelay = simTime - b747cl_P . delay_Delay_c ;
b747cl_B . delay_m = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , b747cl_DW . delay_IWORK_g . CircularBufSize , & b747cl_DW . delay_IWORK_g . Last , b747cl_DW . delay_IWORK_g . Tail , b747cl_DW . delay_IWORK_g . Head , b747cl_P . delay_InitOutput_ba , 0 , 0 ) ;
}
{
b747cl_B . DiscreteStateSpace_a = b747cl_P . DiscreteStateSpace_C_g * b747cl_DW . DiscreteStateSpace_DSTATE_m ;
b747cl_B . DiscreteStateSpace_a += b747cl_P . DiscreteStateSpace_D_a * b747cl_B . delay_m ;
;
}
if ( b747cl_DW . LastMajorTime_e == ( rtInf ) ) {
b747cl_B . RateLimiter3 = b747cl_B . DiscreteStateSpace_a ;
}
else {
rtb_psidot = b747cl_M -> Timing . t [ 0 ] - b747cl_DW . LastMajorTime_e ;
rtb_axk = rtb_psidot * b747cl_P . RateLimiter3_RisingLim ;
rtb_Saturation2 = b747cl_B . DiscreteStateSpace_a - b747cl_DW . PrevY_e ;
if ( rtb_Saturation2 > rtb_axk ) {
b747cl_B . RateLimiter3 = b747cl_DW . PrevY_e + rtb_axk ;
}
else {
rtb_psidot *= b747cl_P . RateLimiter3_FallingLim ;
if ( rtb_Saturation2 < rtb_psidot ) {
b747cl_B . RateLimiter3 = b747cl_DW . PrevY_e + rtb_psidot ;
}
else {
b747cl_B . RateLimiter3 = b747cl_B . DiscreteStateSpace_a ;
}

}

}

if ( b747cl_B . RateLimiter3 > b747cl_P . Saturation2_UpperSat ) {
rtb_Saturation2 = b747cl_P . Saturation2_UpperSat ;
}
else if ( b747cl_B . RateLimiter3 < b747cl_P . Saturation2_LowerSat ) {
rtb_Saturation2 = b747cl_P . Saturation2_LowerSat ;
}
else {
rtb_Saturation2 = b747cl_B . RateLimiter3 ;
}


b747cl_B . Product = b747cl_B . stabs * b747cl_B . u_a [ 1 ] ;
b747cl_B . const1 = b747cl_P . const1_Value ;
TmpSignalConversionAtMatrixGain [ 0 ] = b747cl_B . const_e ;
TmpSignalConversionAtMatrixGain [ 1 ] = b747cl_B . u_a [ 1 ] ;
TmpSignalConversionAtMatrixGain [ 2 ] = b747cl_B . Fcn ;
TmpSignalConversionAtMatrixGain [ 3 ] = b747cl_B . Fcn1 ;
TmpSignalConversionAtMatrixGain [ 4 ] = b747cl_B . u_a [ 2 ] ;
TmpSignalConversionAtMatrixGain [ 5 ] = b747cl_B . Fcn2 ;
TmpSignalConversionAtMatrixGain [ 6 ] = b747cl_B . Fcn3 ;
TmpSignalConversionAtMatrixGain [ 7 ] = b747cl_B . pb2V ;
TmpSignalConversionAtMatrixGain [ 8 ] = b747cl_B . qc2V ;
TmpSignalConversionAtMatrixGain [ 9 ] = b747cl_B . rb2V ;
TmpSignalConversionAtMatrixGain [ 10 ] = rtb_Saturation4 ;
TmpSignalConversionAtMatrixGain [ 11 ] = b747cl_B . stabs ;
TmpSignalConversionAtMatrixGain [ 12 ] = rtb_Saturation1 ;
TmpSignalConversionAtMatrixGain [ 13 ] = rtb_Saturation2 ;
TmpSignalConversionAtMatrixGain [ 14 ] = b747cl_B . Product ;
TmpSignalConversionAtMatrixGain [ 15 ] = rtb_Saturation2 * b747cl_B . u_a [ 1 ] ;
TmpSignalConversionAtMatrixGain [ 16 ] = rtb_Saturation1 * b747cl_B . u_a [ 1 ] ;
TmpSignalConversionAtMatrixGain [ 17 ] = rtb_Saturation4 * b747cl_B . Fcn2 ;
TmpSignalConversionAtMatrixGain [ 18 ] = b747cl_B . const1 ;
for ( i = 0 ; i < 6 ; i ++ )
{
rtb_S [ i ] = 0.0 ;
for ( ci = 0 ; ci < 19 ; ci ++ )
{
rtb_S [ i ] += b747cl_P . AM_M [ ci * 6 + i ] * TmpSignalConversionAtMatrixGain [ ci ] ;
;
}

}

rtb_psidot = sin ( b747cl_B . u_a [ 1 ] ) ;
b747cl_B . sinalpha = rtb_psidot ;
tmp [ 0 ] = ( b747cl_B . sinalpha * rtb_S [ 2 ] - b747cl_B . cosalpha * rtb_S [ 0 ] ) * b747cl_P . fmdims_S ;
tmp [ 1 ] = b747cl_P . fmdims_S * rtb_S [ 1 ] ;
tmp [ 2 ] = ( ( 0.0 - b747cl_B . sinalpha * rtb_S [ 0 ] ) - b747cl_B . cosalpha * rtb_S [ 2 ] ) * b747cl_P . fmdims_S ;
tmp [ 3 ] = b747cl_P . fmdims_S * rtb_S [ 3 ] ;
tmp [ 4 ] = b747cl_P . fmdims_S * rtb_S [ 4 ] ;
tmp [ 5 ] = b747cl_P . fmdims_S * rtb_S [ 5 ] ;
for ( i = 0 ; i < 6 ; i ++ )
{
rtb_S [ i ] = tmp [ i ] ;
}

{
real_T  * * uBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK_a . TUbufferPtrs [ 0 ] ;
real_T  * * tBuffer = (  real_T * * ) & b747cl_DW . delay_PWORK_a . TUbufferPtrs [ 1 ] ;
real_T simTime = b747cl_M -> Timing . t [ 0 ] ;
real_T tMinusDelay = simTime - b747cl_P . delay_Delay_k ;
b747cl_B . delay_o = rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , * uBuffer , b747cl_DW . delay_IWORK_i . CircularBufSize , & b747cl_DW . delay_IWORK_i . Last , b747cl_DW . delay_IWORK_i . Tail , b747cl_DW . delay_IWORK_i . Head , b747cl_P . delay_InitOutput_a , 0 , 0 ) ;
}
{
rtb_w = b747cl_P . DiscreteStateSpace_C_g5 * b747cl_DW . DiscreteStateSpace_DSTATE_d ;
rtb_w += b747cl_P . DiscreteStateSpace_D_o * b747cl_B . delay_o ;
;
}
rtb_w *= b747cl_P . ForceN_Gain ;
if ( rtb_w > b747cl_P . Saturation3_UpperSat ) {
b747cl_B . Saturation3 = b747cl_P . Saturation3_UpperSat ;
}
else if ( rtb_w < b747cl_P . Saturation3_LowerSat ) {
b747cl_B . Saturation3 = b747cl_P . Saturation3_LowerSat ;
}
else {
b747cl_B . Saturation3 = rtb_w ;
}


rtb_Saturation2 = sin ( b747cl_B . u_a [ 7 ] ) ;
b747cl_B . Xgr = - 288772.0 * b747cl_B . g * rtb_Saturation2 ;
rtb_wdot = cos ( b747cl_B . u_a [ 7 ] ) ;
Ygr_tmp = sin ( b747cl_B . u_a [ 8 ] ) ;
b747cl_B . Ygr = 288772.0 * b747cl_B . g * rtb_wdot * Ygr_tmp ;
Zgr_tmp = cos ( b747cl_B . u_a [ 8 ] ) ;
b747cl_B . Zgr = 288772.0 * b747cl_B . g * rtb_wdot * Zgr_tmp ;
rtb_w = ( b747cl_B . u_a [ 4 ] * b747cl_P . Wind_Value [ 2 ] + b747cl_P . Wind_Value [ 3 ] ) - b747cl_B . u_a [ 5 ] * b747cl_P . Wind_Value [ 1 ] ;
b747cl_B . gain [ 0 ] = - b747cl_P . fw_m * rtb_w ;
memcpy ( & tmp_0 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_0 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_1 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_1 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_2 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_2 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_3 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_3 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_4 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_4 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

b747cl_B . gain [ 1 ] = ( ( tmp_0 [ 16 ] - tmp_1 [ 3 ] * tmp_2 [ 14 ] ) + tmp_3 [ 5 ] * tmp_4 [ 12 ] ) - - b747cl_P . fw_m ;
memcpy ( & tmp_0 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_0 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_1 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_1 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_2 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_2 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_3 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_3 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

memcpy ( & tmp_4 [ 0 ] , & b747cl_B . u_a [ 0 ] , 12U * sizeof ( real_T )  
) ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_4 [ i + 12 ] = b747cl_P . Wind_Value [ i ] ;
}

b747cl_B . gain [ 2 ] = ( ( tmp_1 [ 3 ] * tmp_2 [ 13 ] + tmp_0 [ 17 ] ) - tmp_3 [ 4 ] * tmp_4 [ 12 ] ) - - b747cl_P . fw_m ;
tmp_6 [ 2 ] = b747cl_B . qdyn ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_5 [ i + 3 ] = rtb_S [ i ] ;
tmp_6 [ i + 3 ] = rtb_S [ i ] ;
}

rtb_Saturation4 = ( ( tmp_5 [ 3 ] * tmp_6 [ 2 ] + b747cl_B . Saturation3 ) + b747cl_B . Xgr ) + b747cl_B . gain [ 0 ] ;
tmp_6 [ 2 ] = b747cl_B . qdyn ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_5 [ i + 3 ] = rtb_S [ i ] ;
tmp_6 [ i + 3 ] = rtb_S [ i ] ;
}

rtb_Saturation1 = ( ( tmp_5 [ 4 ] * tmp_6 [ 2 ] + 0.0 ) + b747cl_B . Ygr ) + b747cl_B . gain [ 1 ] ;
tmp_6 [ 2 ] = b747cl_B . qdyn ;
for ( i = 0 ; i <= 6 ; i ++ )
{
tmp_5 [ i + 3 ] = rtb_S [ i ] ;
tmp_6 [ i + 3 ] = rtb_S [ i ] ;
}

rtb_axk = ( ( tmp_5 [ 5 ] * tmp_6 [ 2 ] + 0.0 ) + b747cl_B . Zgr ) + b747cl_B . gain [ 2 ] ;
tmp_6 [ 2 ] = b747cl_B . qdyn ;
tmp_8 [ 2 ] = b747cl_B . qdyn ;
tmp_a [ 2 ] = b747cl_B . qdyn ;
for ( i = 0 ; i < 6 ; i ++ )
{
tmp_5 [ i + 3 ] = rtb_S [ i ] ;
tmp_6 [ i + 3 ] = rtb_S [ i ] ;
tmp_7 [ i + 3 ] = rtb_S [ i ] ;
tmp_8 [ i + 3 ] = rtb_S [ i ] ;
tmp_9 [ i + 3 ] = rtb_S [ i ] ;
tmp_a [ i + 3 ] = rtb_S [ i ] ;
}

b747cl_B . cosalpha_k = Sum1_idx_2 ;
b747cl_B . sinalpha_f = rtb_psidot ;
b747cl_B . cosbeta = cos ( b747cl_B . u_a [ 2 ] ) ;
b747cl_B . sinbeta = sin ( b747cl_B . u_a [ 2 ] ) ;
b747cl_B . tanbeta = tan ( b747cl_B . u_a [ 2 ] ) ;
b747cl_B . sinpsi = sin ( b747cl_B . u_a [ 6 ] ) ;
b747cl_B . cospsi = cos ( b747cl_B . u_a [ 6 ] ) ;
b747cl_B . sintheta = rtb_Saturation2 ;
b747cl_B . costheta = rtb_wdot ;
b747cl_B . sinphi = Ygr_tmp ;
b747cl_B . cosphi = Zgr_tmp ;
b747cl_B . Fcn_a = rt_powd_snf ( b747cl_B . u [ 3 ] , 2.0 ) ;
b747cl_B . Fcn1_m = b747cl_B . u [ 3 ] * b747cl_B . u [ 4 ] ;
b747cl_B . Fcn2_o = b747cl_B . u [ 3 ] * b747cl_B . u [ 5 ] ;
b747cl_B . Fcn3_e = rt_powd_snf ( b747cl_B . u [ 4 ] , 2.0 ) ;
b747cl_B . Fcn4 = b747cl_B . u [ 4 ] * b747cl_B . u [ 5 ] ;
b747cl_B . Fcn5 = rt_powd_snf ( b747cl_B . u [ 5 ] , 2.0 ) ;
TmpSignalConversionAtMatrixGa_m [ 3 ] = b747cl_B . Fcn_a ;
TmpSignalConversionAtMatrixGa_m [ 4 ] = b747cl_B . Fcn1_m ;
TmpSignalConversionAtMatrixGa_m [ 5 ] = b747cl_B . Fcn2_o ;
TmpSignalConversionAtMatrixGa_m [ 6 ] = b747cl_B . Fcn3_e ;
TmpSignalConversionAtMatrixGa_m [ 7 ] = b747cl_B . Fcn4 ;
TmpSignalConversionAtMatrixGa_m [ 8 ] = b747cl_B . Fcn5 ;
TmpSignalConversionAtMatrixGa_m [ 0 ] = tmp_5 [ 6 ] * tmp_6 [ 2 ] * b747cl_P . fmdims_b ;
rtb_MatrixGain_a [ 0 ] = 0.0 ;
TmpSignalConversionAtMatrixGa_m [ 1 ] = tmp_7 [ 7 ] * tmp_8 [ 2 ] * b747cl_P . fmdims_cbar ;
rtb_MatrixGain_a [ 1 ] = 0.0 ;
TmpSignalConversionAtMatrixGa_m [ 2 ] = tmp_9 [ 8 ] * tmp_a [ 2 ] * b747cl_P . fmdims_b ;
rtb_MatrixGain_a [ 2 ] = 0.0 ;
for ( i = 0 ; i < 3 ; i ++ )
{
for ( ci = 0 ; ci < 9 ; ci ++ )
{
rtb_MatrixGain_a [ i ] += b747cl_P . GM2_M [ ci * 3 + i ] * TmpSignalConversionAtMatrixGa_m [ ci ] ;
;
}

}

rtb_psidot = ( b747cl_B . u [ 4 ] * b747cl_B . sinphi + b747cl_B . u [ 5 ] * b747cl_B . cosphi ) / b747cl_B . costheta ;
for ( i = 0 ; i < 12 ; i ++ )
{
rtb_u [ i ] = b747cl_P . u_Gain_m * b747cl_B . DiscreteTimeIntegrator [ i ] ;
}

rtb_w = rtb_u [ 0 ] * b747cl_B . sinalpha_f * b747cl_B . cosbeta ;
memcpy ( & rtb_u_0 [ 0 ] , & rtb_u [ 0 ] , 12U * sizeof ( real_T )  
) ;
b747cl_B . Sum [ 0 ] = rtb_u_0 [ 0 ] * b747cl_B . cosalpha_k * b747cl_B . cosbeta + b747cl_P . Wind_Value [ 0 ] ;
memcpy ( & rtb_u_0 [ 0 ] , & rtb_u [ 0 ] , 12U * sizeof ( real_T )  
) ;
b747cl_B . Sum [ 1 ] = rtb_u_0 [ 0 ] * b747cl_B . sinbeta + b747cl_P . Wind_Value [ 1 ] ;
b747cl_B . Sum [ 2 ] = b747cl_P . Wind_Value [ 2 ] + rtb_w ;
Sum1_idx_2 = b747cl_B . Sum [ 1 ] * b747cl_B . sinphi + b747cl_B . Sum [ 2 ] * b747cl_B . cosphi ;
rtb_Saturation2 = Sum1_idx_2 * b747cl_B . sintheta + b747cl_B . Sum [ 0 ] * b747cl_B . costheta ;
rtb_wdot = b747cl_B . Sum [ 1 ] * b747cl_B . cosphi - b747cl_B . Sum [ 2 ] * b747cl_B . sinphi ;
b747cl_B . xfix [ 0 ] = ( ( rtb_Saturation4 * b747cl_B . cosalpha_k * b747cl_B . cosbeta + rtb_Saturation1 * b747cl_B . sinbeta ) + rtb_axk * b747cl_B . sinalpha_f * b747cl_B . cosbeta ) / 288772.0 * b747cl_P . xfix_Gain ;
memcpy ( & tmp_b [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
memcpy ( & tmp_c [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
memcpy ( & tmp_d [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
memcpy ( & tmp_e [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
b747cl_B . xfix [ 1 ] = ( ( ( - rtb_Saturation4 * b747cl_B . sinalpha_f + rtb_axk * b747cl_B . cosalpha_k ) / ( 288772.0 * tmp_b [ 0 ] * b747cl_B . cosbeta ) - ( tmp_c [ 3 ] * b747cl_B . cosalpha_k + tmp_d [ 5 ] * b747cl_B . sinalpha_f ) * b747cl_B . tanbeta ) + tmp_e [ 4 ] ) * b747cl_P . xfix_Gain ;
b747cl_B . xfix [ 2 ] = ( ( ( ( - rtb_Saturation4 * b747cl_B . cosalpha_k * b747cl_B . sinbeta + rtb_Saturation1 * b747cl_B . cosbeta ) - rtb_axk * b747cl_B . sinalpha_f * b747cl_B . sinbeta ) / ( 288772.0 * b747cl_B . u [ 0 ] ) + b747cl_B . u [ 3 ] * b747cl_B . sinalpha_f ) - b747cl_B . u [ 5 ] * b747cl_B . cosalpha_k ) / ( 1.0 - b747cl_B . cosbeta * b747cl_B . rho * 59.74 * 510.95 * 0.0 / 1.155088E+6 ) * b747cl_P . xfix_Gain ;
b747cl_B . xfix [ 3 ] = b747cl_P . xfix_Gain * rtb_MatrixGain_a [ 0 ] ;
b747cl_B . xfix [ 4 ] = b747cl_P . xfix_Gain * rtb_MatrixGain_a [ 1 ] ;
b747cl_B . xfix [ 5 ] = b747cl_P . xfix_Gain * rtb_MatrixGain_a [ 2 ] ;
b747cl_B . xfix [ 6 ] = b747cl_P . xfix_Gain * rtb_psidot ;
memcpy ( & tmp_b [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
memcpy ( & tmp_c [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
b747cl_B . xfix [ 7 ] = ( tmp_b [ 4 ] * b747cl_B . cosphi - tmp_c [ 5 ] * b747cl_B . sinphi ) * b747cl_P . xfix_Gain ;
memcpy ( & tmp_f [ 0 ] , & b747cl_B . u [ 0 ] , 12U * sizeof ( real_T )  
) ;
b747cl_B . xfix [ 8 ] = ( rtb_psidot * b747cl_B . sintheta + tmp_f [ 3 ] ) * b747cl_P . xfix_Gain ;
b747cl_B . xfix [ 9 ] = ( rtb_Saturation2 * b747cl_B . cospsi - rtb_wdot * b747cl_B . sinpsi ) * b747cl_P . xfix_Gain ;
b747cl_B . xfix [ 10 ] = ( rtb_Saturation2 * b747cl_B . sinpsi + rtb_wdot * b747cl_B . cospsi ) * b747cl_P . xfix_Gain ;
b747cl_B . xfix [ 11 ] = ( b747cl_B . Sum [ 0 ] * b747cl_B . sintheta - Sum1_idx_2 * b747cl_B . costheta ) * b747cl_P . xfix_Gain ;
for ( i = 0 ; i < 12 ; i ++ )
{
rtb_u [ i ] = b747cl_P . u_Gain_b * b747cl_B . xfix [ i ] ;
}

for ( i = 0 ; i < 12 ; i ++ )
{
rtb_WhiteNoise [ i ] = b747cl_B . Output [ i ] + b747cl_B . DiscreteTimeIntegrator [ i ] ;
}

for ( i = 0 ; i < 12 ; i ++ )
{
rtb_WhiteNoise [ i + 12 ] = b747cl_B . Output [ i + 12 ] + b747cl_B . xfix [ i ] ;
}

Sum3 [ 2 ] = b747cl_B . y0125812 [ 2 ] - rtb_WhiteNoise [ 4 ] ;
Sum3 [ 3 ] = b747cl_B . y0125812 [ 3 ] - rtb_WhiteNoise [ 7 ] ;
Sum3 [ 4 ] = b747cl_B . y0125812 [ 4 ] - rtb_WhiteNoise [ 11 ] ;
Sum3 [ 0 ] = b747cl_B . y0125812 [ 0 ] - rtb_WhiteNoise [ 0 ] ;
rtb_MatrixGain [ 0 ] = 0.0 ;
Sum3 [ 1 ] = b747cl_B . y0125812 [ 1 ] - rtb_WhiteNoise [ 1 ] ;
rtb_MatrixGain [ 1 ] = 0.0 ;
b747cl_B . y03469 [ 0 ] = b747cl_P . y03469_Value [ 0 ] ;
b747cl_B . y03469 [ 1 ] = b747cl_P . y03469_Value [ 1 ] ;
b747cl_B . y03469 [ 2 ] = b747cl_P . y03469_Value [ 2 ] ;
b747cl_B . y03469 [ 3 ] = b747cl_P . y03469_Value [ 3 ] ;
Sum1_idx_2 = b747cl_B . y03469 [ 2 ] - rtb_WhiteNoise [ 5 ] ;
rtb_Saturation4 = b747cl_B . y03469 [ 3 ] - rtb_WhiteNoise [ 8 ] ;
rtb_Saturation1 = b747cl_B . y03469 [ 0 ] - rtb_WhiteNoise [ 2 ] ;
rtb_MatrixGain_f [ 0 ] = 0.0 ;
rtb_axk = b747cl_B . y03469 [ 1 ] - rtb_WhiteNoise [ 3 ] ;
rtb_MatrixGain_f [ 1 ] = 0.0 ;
for ( i = 0 ; i < 2 ; i ++ )
{
for ( ci = 0 ; ci < 5 ; ci ++ )
{
rtb_MatrixGain [ i ] += b747cl_P . Klg_M [ ( ci << 1 ) + i ] * Sum3 [ ci ] ;
;
}

rtb_MatrixGain_f [ i ] += b747cl_P . Klt_M [ i ] * rtb_Saturation1 ;
;
rtb_MatrixGain_f [ i ] += b747cl_P . Klt_M [ 2 + i ] * rtb_axk ;
;
rtb_MatrixGain_f [ i ] += b747cl_P . Klt_M [ 4 + i ] * Sum1_idx_2 ;
;
rtb_MatrixGain_f [ i ] += b747cl_P . Klt_M [ 6 + i ] * rtb_Saturation4 ;
;
b747cl_B . c4 [ i ] = b747cl_P . c4_Value [ i ] ;
b747cl_B . c1 [ i ] = b747cl_P . c1_Value [ i ] ;
}

b747cl_B . Sum5 = b747cl_P . wind6_Gain * b747cl_P . The747PilotWasHere_Value + b747cl_P . c5_Value ;
b747cl_B . wind2 = b747cl_P . wind2_Gain * 0.0 ;
b747cl_B . wind4 = b747cl_P . wind4_Gain * 0.0 ;
b747cl_B . wind5 = b747cl_P . wind5_Gain * 0.0 ;
if ( b747cl_P . Switch_CurrentSetting == 1 ) {
b747cl_B . Switch [ 0 ] = b747cl_B . Sum5 ;
b747cl_B . Switch [ 1 ] = b747cl_B . wind2 ;
b747cl_B . Switch [ 2 ] = b747cl_B . wind4 ;
b747cl_B . Switch [ 3 ] = b747cl_B . wind5 ;
}
else {
b747cl_B . Switch [ 0 ] = b747cl_B . c4 [ 0 ] + rtb_MatrixGain [ 0 ] ;
b747cl_B . Switch [ 1 ] = b747cl_B . c4 [ 1 ] + rtb_MatrixGain [ 1 ] ;
b747cl_B . Switch [ 2 ] = b747cl_B . c1 [ 0 ] + rtb_MatrixGain_f [ 0 ] ;
b747cl_B . Switch [ 3 ] = b747cl_B . c1 [ 1 ] + rtb_MatrixGain_f [ 1 ] ;
}

rtb_Saturation4 = 0.2 * rt_powd_snf ( b747cl_B . M , 2.0 ) + 1.0 ;
if ( rtb_Saturation4 < 0.0 ) {
rtb_Saturation4 = - rt_powd_snf ( - rtb_Saturation4 , 3.5 ) ;
}
else {
rtb_Saturation4 = rt_powd_snf ( rtb_Saturation4 , 3.5 ) ;
}

rtb_Saturation4 = ( rtb_Saturation4 - 1.0 ) * b747cl_B . ps / 101325.0 + 1.0 ;
if ( rtb_Saturation4 < 0.0 ) {
rtb_Saturation4 = - rt_powd_snf ( - rtb_Saturation4 , 0.2857142857142857 ) ;
}
else {
rtb_Saturation4 = rt_powd_snf ( rtb_Saturation4 , 0.2857142857142857 ) ;
}

rtb_Saturation4 = ( rtb_Saturation4 - 1.0 ) * 579000.0 ;
if ( rtb_Saturation4 < 0.0 ) {
rtb_Vc = - sqrt ( - rtb_Saturation4 ) ;
}
else {
rtb_Vc = sqrt ( rtb_Saturation4 ) ;
}

rtb_Saturation4 = b747cl_B . qdyn * 1.63265 ;
if ( rtb_Saturation4 < 0.0 ) {
rtb_Ve = - sqrt ( - rtb_Saturation4 ) ;
}
else {
rtb_Ve = sqrt ( rtb_Saturation4 ) ;
}

rtb_Tt = ( 0.2 * rt_powd_snf ( b747cl_B . M , 2.0 ) + 1.0 ) * b747cl_B . T ;
rtb_cbar = b747cl_B . rho * b747cl_B . u_a [ 0 ] / b747cl_B . mu * b747cl_P . ad3_cbar ;
rtb_Phi = cos ( b747cl_B . u_a [ 7 ] - 0.0464959 ) * b747cl_B . u_a [ 8 ] ;
rtb_chi = b747cl_B . u_a [ 2 ] + b747cl_B . u_a [ 6 ] ;
rtb_fpa = rtb_u [ 0 ] / 9.80665 ;
Sum1_idx_2 = rtb_u [ 11 ] / b747cl_B . u_a [ 0 ] ;
if ( Sum1_idx_2 > 1.0 ) {
Sum1_idx_2 = 1.0 ;
}
else {
if ( Sum1_idx_2 < - 1.0 ) {
Sum1_idx_2 = - 1.0 ;
}

}

rtb_gamma = asin ( Sum1_idx_2 ) ;
b747cl_B . Clock = b747cl_M -> Timing . t [ 0 ] ;
rt_UpdateLogVar ( (  LogVar * ) (  LogVar * ) ( b747cl_DW . ToWorkspace4_PWORK . LoggedData ) , & b747cl_B . Clock , 0 ) ;
}

/* Model update function */
static void b747cl_update(void)
{
  int32_T iU;
  real_T LastMajorTime_tmp;

  /* Update for DiscreteIntegrator: '<S9>/Discrete-Time Integrator' */
  for (iU = 0; iU < 12; iU++) {
    b747cl_DW.DiscreteTimeIntegrator_DSTATE[iU] +=
      b747cl_P.DiscreteTimeIntegrator_gainval * b747cl_B.xfix[iU];
  }

  /* End of Update for DiscreteIntegrator: '<S9>/Discrete-Time Integrator' */

  /* Update for RandomNumber: '<S3>/White Noise' */
  for (iU = 0; iU < 40; iU++) {
    b747cl_DW.NextOutput[iU] = rt_nrand_Upu32_Yd_f_pw_snf(&b747cl_DW.RandSeed[iU])
      * b747cl_P.WhiteNoise_StdDev + b747cl_P.WhiteNoise_Mean;
  }

  /* End of Update for RandomNumber: '<S3>/White Noise' */

  /* Update for TransportDelay: '<S4>/ delay' */
  {
    real_T **uBuffer = (real_T**)&b747cl_DW.delay_PWORK.TUbufferPtrs[0];
    real_T **tBuffer = (real_T**)&b747cl_DW.delay_PWORK.TUbufferPtrs[1];
    real_T simTime = b747cl_M->Timing.t[0];
    b747cl_DW.delay_IWORK.Head = ((b747cl_DW.delay_IWORK.Head <
      (b747cl_DW.delay_IWORK.CircularBufSize-1)) ? (b747cl_DW.delay_IWORK.Head+1)
      : 0);
    if (b747cl_DW.delay_IWORK.Head == b747cl_DW.delay_IWORK.Tail) {
      b747cl_DW.delay_IWORK.Tail = ((b747cl_DW.delay_IWORK.Tail <
        (b747cl_DW.delay_IWORK.CircularBufSize-1)) ? (b747cl_DW.delay_IWORK.Tail
        +1) : 0);
    }

    (*tBuffer)[b747cl_DW.delay_IWORK.Head] = simTime;
    (*uBuffer)[b747cl_DW.delay_IWORK.Head] = b747cl_B.Switch[1];
  }

  /* Update for DiscreteStateSpace: '<S36>/Discrete State-Space' */
  {
    real_T xnew[1];
    xnew[0] = b747cl_P.DiscreteStateSpace_A*b747cl_DW.DiscreteStateSpace_DSTATE;
    xnew[0] += b747cl_P.DiscreteStateSpace_B*b747cl_B.delay;
    (void) memcpy(&b747cl_DW.DiscreteStateSpace_DSTATE, xnew,
                  sizeof(real_T)*1);
  }

  /* Update for RateLimiter: '<S4>/Rate Limiter1' incorporates:
   *  RateLimiter: '<S4>/Rate Limiter2'
   *  RateLimiter: '<S4>/Rate Limiter3'
   */
  b747cl_DW.PrevY = b747cl_B.RateLimiter1;
  LastMajorTime_tmp = b747cl_M->Timing.t[0];
  b747cl_DW.LastMajorTime = LastMajorTime_tmp;

  /* Update for TransportDelay: '<S4>/delay' */
  {
    real_T **uBuffer = (real_T**)&b747cl_DW.delay_PWORK_f.TUbufferPtrs[0];
    real_T **tBuffer = (real_T**)&b747cl_DW.delay_PWORK_f.TUbufferPtrs[1];
    real_T simTime = b747cl_M->Timing.t[0];
    b747cl_DW.delay_IWORK_j.Head = ((b747cl_DW.delay_IWORK_j.Head <
      (b747cl_DW.delay_IWORK_j.CircularBufSize-1)) ?
      (b747cl_DW.delay_IWORK_j.Head+1) : 0);
    if (b747cl_DW.delay_IWORK_j.Head == b747cl_DW.delay_IWORK_j.Tail) {
      b747cl_DW.delay_IWORK_j.Tail = ((b747cl_DW.delay_IWORK_j.Tail <
        (b747cl_DW.delay_IWORK_j.CircularBufSize-1)) ?
        (b747cl_DW.delay_IWORK_j.Tail+1) : 0);
    }

    (*tBuffer)[b747cl_DW.delay_IWORK_j.Head] = simTime;
    (*uBuffer)[b747cl_DW.delay_IWORK_j.Head] = b747cl_B.Switch[2];
  }

  /* Update for DiscreteStateSpace: '<S37>/Discrete State-Space' */
  {
    real_T xnew[1];
    xnew[0] = b747cl_P.DiscreteStateSpace_A_d*
      b747cl_DW.DiscreteStateSpace_DSTATE_g;
    xnew[0] += b747cl_P.DiscreteStateSpace_B_h*b747cl_B.delay_h;
    (void) memcpy(&b747cl_DW.DiscreteStateSpace_DSTATE_g, xnew,
                  sizeof(real_T)*1);
  }

  /* Update for RateLimiter: '<S4>/Rate Limiter2' */
  b747cl_DW.PrevY_o = b747cl_B.RateLimiter2;
  b747cl_DW.LastMajorTime_l = LastMajorTime_tmp;

  /* Update for TransportDelay: '<S4>/delay ' */
  {
    real_T **uBuffer = (real_T**)&b747cl_DW.delay_PWORK_e.TUbufferPtrs[0];
    real_T **tBuffer = (real_T**)&b747cl_DW.delay_PWORK_e.TUbufferPtrs[1];
    real_T simTime = b747cl_M->Timing.t[0];
    b747cl_DW.delay_IWORK_g.Head = ((b747cl_DW.delay_IWORK_g.Head <
      (b747cl_DW.delay_IWORK_g.CircularBufSize-1)) ?
      (b747cl_DW.delay_IWORK_g.Head+1) : 0);
    if (b747cl_DW.delay_IWORK_g.Head == b747cl_DW.delay_IWORK_g.Tail) {
      b747cl_DW.delay_IWORK_g.Tail = ((b747cl_DW.delay_IWORK_g.Tail <
        (b747cl_DW.delay_IWORK_g.CircularBufSize-1)) ?
        (b747cl_DW.delay_IWORK_g.Tail+1) : 0);
    }

    (*tBuffer)[b747cl_DW.delay_IWORK_g.Head] = simTime;
    (*uBuffer)[b747cl_DW.delay_IWORK_g.Head] = b747cl_B.Switch[3];
  }

  /* Update for DiscreteStateSpace: '<S38>/Discrete State-Space' */
  {
    real_T xnew[1];
    xnew[0] = b747cl_P.DiscreteStateSpace_A_b*
      b747cl_DW.DiscreteStateSpace_DSTATE_m;
    xnew[0] += b747cl_P.DiscreteStateSpace_B_i*b747cl_B.delay_m;
    (void) memcpy(&b747cl_DW.DiscreteStateSpace_DSTATE_m, xnew,
                  sizeof(real_T)*1);
  }

  /* Update for RateLimiter: '<S4>/Rate Limiter3' */
  b747cl_DW.PrevY_e = b747cl_B.RateLimiter3;
  b747cl_DW.LastMajorTime_e = LastMajorTime_tmp;

  /* Update for TransportDelay: '<S4>/ delay ' */
  {
    real_T **uBuffer = (real_T**)&b747cl_DW.delay_PWORK_a.TUbufferPtrs[0];
    real_T **tBuffer = (real_T**)&b747cl_DW.delay_PWORK_a.TUbufferPtrs[1];
    real_T simTime = b747cl_M->Timing.t[0];
    b747cl_DW.delay_IWORK_i.Head = ((b747cl_DW.delay_IWORK_i.Head <
      (b747cl_DW.delay_IWORK_i.CircularBufSize-1)) ?
      (b747cl_DW.delay_IWORK_i.Head+1) : 0);
    if (b747cl_DW.delay_IWORK_i.Head == b747cl_DW.delay_IWORK_i.Tail) {
      b747cl_DW.delay_IWORK_i.Tail = ((b747cl_DW.delay_IWORK_i.Tail <
        (b747cl_DW.delay_IWORK_i.CircularBufSize-1)) ?
        (b747cl_DW.delay_IWORK_i.Tail+1) : 0);
    }

    (*tBuffer)[b747cl_DW.delay_IWORK_i.Head] = simTime;
    (*uBuffer)[b747cl_DW.delay_IWORK_i.Head] = b747cl_B.Switch[0];
  }

  /* Update for DiscreteStateSpace: '<S35>/Discrete State-Space' */
  {
    real_T xnew[1];
    xnew[0] = b747cl_P.DiscreteStateSpace_A_n*
      b747cl_DW.DiscreteStateSpace_DSTATE_d;
    xnew[0] += b747cl_P.DiscreteStateSpace_B_e*b747cl_B.delay_o;
    (void) memcpy(&b747cl_DW.DiscreteStateSpace_DSTATE_d, xnew,
                  sizeof(real_T)*1);
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++b747cl_M->Timing.clockTick0)) {
    ++b747cl_M->Timing.clockTickH0;
  }

  b747cl_M->Timing.t[0] = b747cl_M->Timing.clockTick0 *
    b747cl_M->Timing.stepSize0 + b747cl_M->Timing.clockTickH0 *
    b747cl_M->Timing.stepSize0 * 4294967296.0;

  {
    /* Update absolute timer for sample time: [0.05s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick1"
     * and "Timing.stepSize1". Size of "clockTick1" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick1 and the high bits
     * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++b747cl_M->Timing.clockTick1)) {
      ++b747cl_M->Timing.clockTickH1;
    }

    b747cl_M->Timing.t[1] = b747cl_M->Timing.clockTick1 *
      b747cl_M->Timing.stepSize1 + b747cl_M->Timing.clockTickH1 *
      b747cl_M->Timing.stepSize1 * 4294967296.0;
  }
}

/* Model initialize function */
static void b747cl_initialize(void)
{
  /* Start for ToWorkspace: '<Root>/Aircraft States' */
  {
    int_T dimensions[1] = { 12 };

    b747cl_DW.AircraftStates_PWORK.LoggedData = rt_CreateLogVar(
      b747cl_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(b747cl_M),
      b747cl_M->Timing.stepSize0,
      (&rtmGetErrorStatus(b747cl_M)),
      "x",
      SS_DOUBLE,
      0,
      0,
      0,
      12,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      1000,
      1,
      0.05,
      1);
    if (b747cl_DW.AircraftStates_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/To Workspace1' */
  {
    int_T dimensions[1] = { 1 };

    b747cl_DW.ToWorkspace1_PWORK.LoggedData = rt_CreateLogVar(
      b747cl_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(b747cl_M),
      b747cl_M->Timing.stepSize0,
      (&rtmGetErrorStatus(b747cl_M)),
      "airspeed",
      SS_DOUBLE,
      0,
      0,
      0,
      1,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      0,
      1,
      0.05,
      1);
    if (b747cl_DW.ToWorkspace1_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/To Workspace2' */
  {
    int_T dimensions[1] = { 1 };

    b747cl_DW.ToWorkspace2_PWORK.LoggedData = rt_CreateLogVar(
      b747cl_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(b747cl_M),
      b747cl_M->Timing.stepSize0,
      (&rtmGetErrorStatus(b747cl_M)),
      "theta",
      SS_DOUBLE,
      0,
      0,
      0,
      1,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      0,
      1,
      0.05,
      1);
    if (b747cl_DW.ToWorkspace2_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/To Workspace3' */
  {
    int_T dimensions[1] = { 1 };

    b747cl_DW.ToWorkspace3_PWORK.LoggedData = rt_CreateLogVar(
      b747cl_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(b747cl_M),
      b747cl_M->Timing.stepSize0,
      (&rtmGetErrorStatus(b747cl_M)),
      "altitude",
      SS_DOUBLE,
      0,
      0,
      0,
      1,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      0,
      1,
      0.05,
      1);
    if (b747cl_DW.ToWorkspace3_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for TransportDelay: '<S4>/ delay' */
  {
    real_T *pBuffer = &b747cl_DW.delay_RWORK.TUbufferArea[0];
    b747cl_DW.delay_IWORK.Tail = 0;
    b747cl_DW.delay_IWORK.Head = 0;
    b747cl_DW.delay_IWORK.Last = 0;
    b747cl_DW.delay_IWORK.CircularBufSize = 1024;
    pBuffer[0] = (b747cl_P.delay_InitOutput);
    pBuffer[1024] = b747cl_M->Timing.t[0];
    b747cl_DW.delay_PWORK.TUbufferPtrs[0] = (void *) &pBuffer[0];
    b747cl_DW.delay_PWORK.TUbufferPtrs[1] = (void *) &pBuffer[1024];
  }

  /* Start for TransportDelay: '<S4>/delay' */
  {
    real_T *pBuffer = &b747cl_DW.delay_RWORK_g.TUbufferArea[0];
    b747cl_DW.delay_IWORK_j.Tail = 0;
    b747cl_DW.delay_IWORK_j.Head = 0;
    b747cl_DW.delay_IWORK_j.Last = 0;
    b747cl_DW.delay_IWORK_j.CircularBufSize = 1024;
    pBuffer[0] = b747cl_P.delay_InitOutput_b;
    pBuffer[1024] = b747cl_M->Timing.t[0];
    b747cl_DW.delay_PWORK_f.TUbufferPtrs[0] = (void *) &pBuffer[0];
    b747cl_DW.delay_PWORK_f.TUbufferPtrs[1] = (void *) &pBuffer[1024];
  }

  /* Start for TransportDelay: '<S4>/delay ' */
  {
    real_T *pBuffer = &b747cl_DW.delay_RWORK_o.TUbufferArea[0];
    b747cl_DW.delay_IWORK_g.Tail = 0;
    b747cl_DW.delay_IWORK_g.Head = 0;
    b747cl_DW.delay_IWORK_g.Last = 0;
    b747cl_DW.delay_IWORK_g.CircularBufSize = 1024;
    pBuffer[0] = b747cl_P.delay_InitOutput_ba;
    pBuffer[1024] = b747cl_M->Timing.t[0];
    b747cl_DW.delay_PWORK_e.TUbufferPtrs[0] = (void *) &pBuffer[0];
    b747cl_DW.delay_PWORK_e.TUbufferPtrs[1] = (void *) &pBuffer[1024];
  }

  /* Start for TransportDelay: '<S4>/ delay ' */
  {
    real_T *pBuffer = &b747cl_DW.delay_RWORK_p.TUbufferArea[0];
    b747cl_DW.delay_IWORK_i.Tail = 0;
    b747cl_DW.delay_IWORK_i.Head = 0;
    b747cl_DW.delay_IWORK_i.Last = 0;
    b747cl_DW.delay_IWORK_i.CircularBufSize = 1024;
    pBuffer[0] = b747cl_P.delay_InitOutput_a;
    pBuffer[1024] = b747cl_M->Timing.t[0];
    b747cl_DW.delay_PWORK_a.TUbufferPtrs[0] = (void *) &pBuffer[0];
    b747cl_DW.delay_PWORK_a.TUbufferPtrs[1] = (void *) &pBuffer[1024];
  }

  /* Start for ToWorkspace: '<Root>/To Workspace4' */
  {
    int_T dimensions[1] = { 1 };

    b747cl_DW.ToWorkspace4_PWORK.LoggedData = rt_CreateLogVar(
      b747cl_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(b747cl_M),
      b747cl_M->Timing.stepSize0,
      (&rtmGetErrorStatus(b747cl_M)),
      "t",
      SS_DOUBLE,
      0,
      0,
      0,
      1,
      1,
      dimensions,
      NO_LOGVALDIMS,
      (NULL),
      (NULL),
      1000,
      1,
      0.05,
      1);
    if (b747cl_DW.ToWorkspace4_PWORK.LoggedData == (NULL))
      return;
  }

  {
    int32_T iU;
    uint32_T tseed;
    int32_T r;
    int32_T t;
    real_T y1;

    /* InitializeConditions for DiscreteIntegrator: '<S9>/Discrete-Time Integrator' */
    memcpy(&b747cl_DW.DiscreteTimeIntegrator_DSTATE[0], &b747cl_P.DTB747_x0[0],
           12U * sizeof(real_T));

    /* InitializeConditions for RandomNumber: '<S3>/White Noise' */
    for (iU = 0; iU < 40; iU++) {
      y1 = floor(b747cl_P.WhiteNoise_Seed[iU]);
      if (rtIsNaN(y1) || rtIsInf(y1)) {
        y1 = 0.0;
      } else {
        y1 = fmod(y1, 4.294967296E+9);
      }

      tseed = y1 < 0.0 ? (uint32_T)-(int32_T)(uint32_T)-y1 : (uint32_T)y1;
      r = (int32_T)(tseed >> 16U);
      t = (int32_T)(tseed & 32768U);
      tseed = ((((tseed - ((uint32_T)r << 16U)) + t) << 16U) + t) + r;
      if (tseed < 1U) {
        tseed = 1144108930U;
      } else {
        if (tseed > 2147483646U) {
          tseed = 2147483646U;
        }
      }

      y1 = rt_nrand_Upu32_Yd_f_pw_snf(&tseed) * b747cl_P.WhiteNoise_StdDev +
        b747cl_P.WhiteNoise_Mean;
      b747cl_DW.NextOutput[iU] = y1;
      b747cl_DW.RandSeed[iU] = tseed;
    }

    /* End of InitializeConditions for RandomNumber: '<S3>/White Noise' */

    /* InitializeConditions for DiscreteStateSpace: '<S36>/Discrete State-Space' */
    b747cl_DW.DiscreteStateSpace_DSTATE = b747cl_P.IIR2_x0;

    /* InitializeConditions for RateLimiter: '<S4>/Rate Limiter1' */
    b747cl_DW.LastMajorTime = (rtInf);

    /* InitializeConditions for DiscreteStateSpace: '<S37>/Discrete State-Space' */
    b747cl_DW.DiscreteStateSpace_DSTATE_g = b747cl_P.IIR3_x0;

    /* InitializeConditions for RateLimiter: '<S4>/Rate Limiter2' */
    b747cl_DW.LastMajorTime_l = (rtInf);

    /* InitializeConditions for DiscreteStateSpace: '<S38>/Discrete State-Space' */
    b747cl_DW.DiscreteStateSpace_DSTATE_m = b747cl_P.IIR4_x0;

    /* InitializeConditions for RateLimiter: '<S4>/Rate Limiter3' */
    b747cl_DW.LastMajorTime_e = (rtInf);

    /* InitializeConditions for DiscreteStateSpace: '<S35>/Discrete State-Space' */
    b747cl_DW.DiscreteStateSpace_DSTATE_d = b747cl_P.IIR1_x0;
  }
}

/* Model terminate function */
static void b747cl_terminate(void)
{
  /* (no terminate code required) */
}

/*========================================================================*
 * Start of Classic call interface                                        *
 *========================================================================*/
void MdlOutputs(int_T tid)
{
  b747cl_output();
  UNUSED_PARAMETER(tid);
}

void MdlUpdate(int_T tid)
{
  b747cl_update();
  UNUSED_PARAMETER(tid);
}

void MdlInitializeSizes(void)
{
}

void MdlInitializeSampleTimes(void)
{
}

void MdlInitialize(void)
{
}

void MdlStart(void)
{
  b747cl_initialize();
}

void MdlTerminate(void)
{
  b747cl_terminate();
}

/* Registration function */
RT_MODEL_b747cl_T *b747cl(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)b747cl_M, 0,
                sizeof(RT_MODEL_b747cl_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&b747cl_M->solverInfo, &b747cl_M->Timing.simTimeStep);
    rtsiSetTPtr(&b747cl_M->solverInfo, &rtmGetTPtr(b747cl_M));
    rtsiSetStepSizePtr(&b747cl_M->solverInfo, &b747cl_M->Timing.stepSize0);
    rtsiSetErrorStatusPtr(&b747cl_M->solverInfo, (&rtmGetErrorStatus(b747cl_M)));
    rtsiSetRTModelPtr(&b747cl_M->solverInfo, b747cl_M);
  }

  rtsiSetSimTimeStep(&b747cl_M->solverInfo, MAJOR_TIME_STEP);
  rtsiSetSolverName(&b747cl_M->solverInfo,"FixedStepDiscrete");

  /* Initialize timing info */
  {
    int_T *mdlTsMap = b747cl_M->Timing.sampleTimeTaskIDArray;
    mdlTsMap[0] = 0;
    mdlTsMap[1] = 1;
    b747cl_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    b747cl_M->Timing.sampleTimes = (&b747cl_M->Timing.sampleTimesArray[0]);
    b747cl_M->Timing.offsetTimes = (&b747cl_M->Timing.offsetTimesArray[0]);

    /* task periods */
    b747cl_M->Timing.sampleTimes[0] = (0.0);
    b747cl_M->Timing.sampleTimes[1] = (0.05);

    /* task offsets */
    b747cl_M->Timing.offsetTimes[0] = (0.0);
    b747cl_M->Timing.offsetTimes[1] = (0.0);
  }

  rtmSetTPtr(b747cl_M, &b747cl_M->Timing.tArray[0]);

  {
    int_T *mdlSampleHits = b747cl_M->Timing.sampleHitArray;
    mdlSampleHits[0] = 1;
    mdlSampleHits[1] = 1;
    b747cl_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(b747cl_M, 30.0);
  b747cl_M->Timing.stepSize0 = 0.05;
  b747cl_M->Timing.stepSize1 = 0.05;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    b747cl_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(b747cl_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(b747cl_M->rtwLogInfo, (NULL));
    rtliSetLogT(b747cl_M->rtwLogInfo, "");
    rtliSetLogX(b747cl_M->rtwLogInfo, "");
    rtliSetLogXFinal(b747cl_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(b747cl_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(b747cl_M->rtwLogInfo, 0);
    rtliSetLogMaxRows(b747cl_M->rtwLogInfo, 0);
    rtliSetLogDecimation(b747cl_M->rtwLogInfo, 1);
    rtliSetLogY(b747cl_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(b747cl_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(b747cl_M->rtwLogInfo, (NULL));
  }

  b747cl_M->solverInfoPtr = (&b747cl_M->solverInfo);
  b747cl_M->Timing.stepSize = (0.05);
  rtsiSetFixedStepSize(&b747cl_M->solverInfo, 0.05);
  rtsiSetSolverMode(&b747cl_M->solverInfo, SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  b747cl_M->blockIO = ((void *) &b747cl_B);
  (void) memset(((void *) &b747cl_B), 0,
                sizeof(B_b747cl_T));

  /* parameters */
  b747cl_M->defaultParam = ((real_T *)&b747cl_P);

  /* states (dwork) */
  b747cl_M->dwork = ((void *) &b747cl_DW);
  (void) memset((void *)&b747cl_DW, 0,
                sizeof(DW_b747cl_T));

  /* Initialize Sizes */
  b747cl_M->Sizes.numContStates = (0); /* Number of continuous states */
  b747cl_M->Sizes.numY = (0);          /* Number of model outputs */
  b747cl_M->Sizes.numU = (0);          /* Number of model inputs */
  b747cl_M->Sizes.sysDirFeedThru = (0);/* The model is not direct feedthrough */
  b747cl_M->Sizes.numSampTimes = (2);  /* Number of sample times */
  b747cl_M->Sizes.numBlocks = (168);   /* Number of blocks */
  b747cl_M->Sizes.numBlockIO = (73);   /* Number of block outputs */
  b747cl_M->Sizes.numBlockPrms = (340);/* Sum of parameter "widths" */
  return b747cl_M;
}

/*========================================================================*
 * End of Classic call interface                                          *
 *========================================================================*/

 ;
