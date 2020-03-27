/*
 * rct_helico.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "rct_helico".
 *
 * Model version              : 1.135
 * Simulink Coder version : 8.10 (R2016a) 10-Feb-2016
 * C source code generated on : Thu Jul 26 17:45:36 2018
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "rct_helico.h"
#include "rct_helico_private.h"

/* Block signals (auto storage) */
B_rct_helico_T rct_helico_B;

/* Continuous states */
X_rct_helico_T rct_helico_X;

/* Block states (auto storage) */
DW_rct_helico_T rct_helico_DW;

/* Real-time model */
RT_MODEL_rct_helico_T rct_helico_M_;
RT_MODEL_rct_helico_T *const rct_helico_M = &rct_helico_M_;

/*
 * This function updates continuous states using the ODE3 fixed-step
 * solver algorithm
 */
static void rt_ertODEUpdateContinuousStates(RTWSolverInfo *si )
{
  /* Solver Matrices */
  static const real_T rt_ODE3_A[3] = {
    1.0/2.0, 3.0/4.0, 1.0
  };

  static const real_T rt_ODE3_B[3][3] = {
    { 1.0/2.0, 0.0, 0.0 },

    { 0.0, 3.0/4.0, 0.0 },
 
/* Precision and accuracy category mutation - template P6: changed d*.d/ d*.df -> d* 
* Before mutation =>     { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
* Mutation begins */
    { 2.0/9, 1.0/3.0, 4.0/9.0 }
/* Mutation ends */

  };

  time_T t = rtsiGetT(si);
  time_T tnew = rtsiGetSolverStopTime(si);
  time_T h = rtsiGetStepSize(si);
  real_T *x = rtsiGetContStates(si);
  ODE3_IntgData *id = (ODE3_IntgData *)rtsiGetSolverData(si);
  real_T *y = id->y;
  real_T *f0 = id->f[0];
  real_T *f1 = id->f[1];
  real_T *f2 = id->f[2];
  real_T hB[3];
  int_T i;
  int_T nXc = 17;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  rct_helico_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  rct_helico_step();
  rct_helico_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  rct_helico_step();
  rct_helico_derivatives();

  /* tnew = t + hA(3);
     ynew = y + f*hB(:,3); */
  for (i = 0; i <= 2; i++) {
    hB[i] = h * rt_ODE3_B[2][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1] + f2[i]*hB[2]);
  }

  rtsiSetT(si, tnew);
  rtsiSetSimTimeStep(si,MAJOR_TIME_STEP);
}

/* Model step function */
void rct_helico_step(void)
{
  int_T iy;
  int_T ci;
  real_T rtb_dc;
  real_T rtb_ds;
  real_T rtb_ProportionalGain_p;
  real_T rtb_SOF[3];
  static const int8_T jc[5] = { 3, 6, 7, 2, 5 };

  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* set solver stop time */
    if (!(rct_helico_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&rct_helico_M->solverInfo,
                            ((rct_helico_M->Timing.clockTickH0 + 1) *
        rct_helico_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&rct_helico_M->solverInfo,
                            ((rct_helico_M->Timing.clockTick0 + 1) *
        rct_helico_M->Timing.stepSize0 + rct_helico_M->Timing.clockTickH0 *
        rct_helico_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(rct_helico_M)) {
    rct_helico_M->Timing.t[0] = rtsiGetT(&rct_helico_M->solverInfo);
  }

  /* StateSpace: '<Root>/Helicopter' */
  for (iy = 0; iy < 5; iy++) {
    rct_helico_B.y[iy] = 0.0;
    for (ci = iy; ci < iy + 1; ci++) {
      rct_helico_B.y[iy] += rct_helico_X.Helicopter_CSTATE[jc[ci]];
    }
  }

  /* End of StateSpace: '<Root>/Helicopter' */
  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* ToWorkspace: '<Root>/phi' */
    rt_UpdateLogVar((LogVar *)(LogVar*) (rct_helico_DW.phi_PWORK.LoggedData),
                    &rct_helico_B.y[1], 0);
  }

  /* Step: '<Root>/phi-ref' */
  rct_helico_B.phiref = !(rct_helico_M->Timing.t[0] < 3.0);
  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* ToWorkspace: '<Root>/phi1' */
    rt_UpdateLogVar((LogVar *)(LogVar*) (rct_helico_DW.phi1_PWORK.LoggedData),
                    &rct_helico_B.phiref, 0);

    /* ToWorkspace: '<Root>/r' */
    rt_UpdateLogVar((LogVar *)(LogVar*) (rct_helico_DW.r_PWORK.LoggedData),
                    &rct_helico_B.y[2], 0);
  }

  /* Step: '<Root>/r-ref' */
  rct_helico_B.rref = !(rct_helico_M->Timing.t[0] < 6.0);
  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* ToWorkspace: '<Root>/r1' */
    rt_UpdateLogVar((LogVar *)(LogVar*) (rct_helico_DW.r1_PWORK.LoggedData),
                    &rct_helico_B.rref, 0);

    /* ToWorkspace: '<Root>/theta' */
    rt_UpdateLogVar((LogVar *)(LogVar*) (rct_helico_DW.theta_PWORK.LoggedData),
                    &rct_helico_B.y[0], 0);
  }

  /* Step: '<Root>/theta-ref' */
  rct_helico_B.thetaref = !(rct_helico_M->Timing.t[0] < 0.0);
  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* ToWorkspace: '<Root>/theta1' */
    rt_UpdateLogVar((LogVar *)(LogVar*) (rct_helico_DW.theta1_PWORK.LoggedData),
                    &rct_helico_B.thetaref, 0);
  }

  /* Sum: '<Root>/Sum2' */
  rtb_dc = rct_helico_B.thetaref - rct_helico_B.y[0];

  /* Gain: '<S1>/Filter Coefficient' incorporates:
   *  Gain: '<S1>/Derivative Gain'
   *  Integrator: '<S1>/Filter'
   *  Sum: '<S1>/SumD'
   */
  rct_helico_B.FilterCoefficient = (0.537 * rtb_dc - rct_helico_X.Filter_CSTATE)
    * 0.00852;

  /* Gain: '<S1>/Integral Gain' */
  rct_helico_B.IntegralGain = 0.905 * rtb_dc;

  /* Sum: '<Root>/Sum1' */
  rtb_ds = rct_helico_B.phiref - rct_helico_B.y[1];

  /* Gain: '<S2>/Filter Coefficient' incorporates:
   *  Gain: '<S2>/Derivative Gain'
   *  Integrator: '<S2>/Filter'
   *  Sum: '<S2>/SumD'
   */
  rct_helico_B.FilterCoefficient_l = (-0.173 * rtb_ds -
    rct_helico_X.Filter_CSTATE_p) * 0.0117;

  /* Gain: '<S2>/Integral Gain' */
  rct_helico_B.IntegralGain_h = -0.973 * rtb_ds;

  /* Gain: '<S2>/Proportional Gain' */
  rtb_ProportionalGain_p = -0.406 * rtb_ds;

  /* Sum: '<Root>/Sum3' */
  rtb_ds = rct_helico_B.rref - rct_helico_B.y[2];

  /* Gain: '<S3>/Filter Coefficient' incorporates:
   *  Gain: '<S3>/Derivative Gain'
   *  Integrator: '<S3>/Filter'
   *  Sum: '<S3>/SumD'
   */
  rct_helico_B.FilterCoefficient_o = (-1.04 * rtb_ds -
    rct_helico_X.Filter_CSTATE_k) * 0.038;

  /* Gain: '<S3>/Integral Gain' */
  rct_helico_B.IntegralGain_k = -3.57 * rtb_ds;

  /* Gain: '<Root>/SOF' */
  for (iy = 0; iy < 3; iy++) {
    rtb_SOF[iy] = 0.0;
    for (ci = 0; ci < 5; ci++) {
      rtb_SOF[iy] += 0.0 * rct_helico_B.y[ci];
    }
  }

  /* End of Gain: '<Root>/SOF' */

  /* Sum: '<Root>/Sum4' incorporates:
   *  Integrator: '<S2>/Integrator'
   *  Sum: '<S2>/Sum'
   */
  rct_helico_B.Sum4 = ((rtb_ProportionalGain_p +
                        rct_helico_X.Integrator_CSTATE_n) +
                       rct_helico_B.FilterCoefficient_l) - rtb_SOF[1];

  /* Sum: '<Root>/Sum5' incorporates:
   *  Gain: '<S1>/Proportional Gain'
   *  Integrator: '<S1>/Integrator'
   *  Sum: '<S1>/Sum'
   */
  rct_helico_B.Sum5 = ((0.307 * rtb_dc + rct_helico_X.Integrator_CSTATE) +
                       rct_helico_B.FilterCoefficient) - rtb_SOF[0];

  /* Sum: '<Root>/Sum6' incorporates:
   *  Gain: '<S3>/Proportional Gain'
   *  Integrator: '<S3>/Integrator'
   *  Sum: '<S3>/Sum'
   */
  rct_helico_B.Sum6 = ((-0.46 * rtb_ds + rct_helico_X.Integrator_CSTATE_o) +
                       rct_helico_B.FilterCoefficient_o) - rtb_SOF[2];

  /* SignalConversion: '<Root>/TmpSignal ConversionAtHelicopterInport1' incorporates:
   *  TransferFcn: '<Root>/roll-off 1'
   *  TransferFcn: '<Root>/roll-off 2'
   *  TransferFcn: '<Root>/roll-off 3'
   */
  rct_helico_B.u[0] = 40.0 * rct_helico_X.rolloff1_CSTATE;
  rct_helico_B.u[1] = 40.0 * rct_helico_X.rolloff2_CSTATE;
  rct_helico_B.u[2] = 40.0 * rct_helico_X.rolloff3_CSTATE;
  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* Matfile logging */
    rt_UpdateTXYLogVars(rct_helico_M->rtwLogInfo, (rct_helico_M->Timing.t));
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(rct_helico_M)) {
    /* signal main to stop simulation */
    {                                  /* Sample time: [0.0s, 0.0s] */
      if ((rtmGetTFinal(rct_helico_M)!=-1) &&
          !((rtmGetTFinal(rct_helico_M)-(((rct_helico_M->Timing.clockTick1+
               rct_helico_M->Timing.clockTickH1* 4294967296.0)) * 0.2)) >
            (((rct_helico_M->Timing.clockTick1+rct_helico_M->Timing.clockTickH1*
               4294967296.0)) * 0.2) * (DBL_EPSILON))) {
        rtmSetErrorStatus(rct_helico_M, "Simulation finished");
      }
    }

    rt_ertODEUpdateContinuousStates(&rct_helico_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick0 and the high bits
     * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++rct_helico_M->Timing.clockTick0)) {
      ++rct_helico_M->Timing.clockTickH0;
    }

    rct_helico_M->Timing.t[0] = rtsiGetSolverStopTime(&rct_helico_M->solverInfo);

    {
      /* Update absolute timer for sample time: [0.2s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 0.2, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       * Timer of this task consists of two 32 bit unsigned integers.
       * The two integers represent the low bits Timing.clockTick1 and the high bits
       * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
       */
      rct_helico_M->Timing.clockTick1++;
      if (!rct_helico_M->Timing.clockTick1) {
        rct_helico_M->Timing.clockTickH1++;
      }
    }
  }                                    /* end MajorTimeStep */
}

/* Derivatives for root system: '<Root>' */
void rct_helico_derivatives(void)
{
  int_T is;
  int_T ci;
  static const int8_T ir[9] = { 0, 6, 13, 19, 21, 29, 34, 37, 43 };

  static const int8_T ir_0[9] = { 0, 2, 4, 7, 7, 10, 13, 13, 16 };

  static const int8_T jc[43] = { 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2,
    4, 5, 7, 2, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 4, 5, 7, 2, 5, 7, 0, 1, 2, 4, 5,
    7 };

  static const int8_T jc_0[16] = { 0, 1, 0, 1, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1,
    2 };

  XDot_rct_helico_T *_rtXdot;
  _rtXdot = ((XDot_rct_helico_T *) rct_helico_M->ModelData.derivs);

  /* Derivatives for StateSpace: '<Root>/Helicopter' */
  for (is = 0; is < 8; is++) {
    _rtXdot->Helicopter_CSTATE[is] = 0.0;
    for (ci = ir[is]; ci < ir[is + 1]; ci++) {
      _rtXdot->Helicopter_CSTATE[is] += rct_helico_ConstP.Helicopter_A[ci] *
        rct_helico_X.Helicopter_CSTATE[jc[ci]];
    }
  }

  for (is = 0; is < 8; is++) {
    for (ci = ir_0[is]; ci < ir_0[is + 1]; ci++) {
      _rtXdot->Helicopter_CSTATE[is] += rct_helico_ConstP.Helicopter_B[ci] *
        rct_helico_B.u[jc_0[ci]];
    }
  }

  /* End of Derivatives for StateSpace: '<Root>/Helicopter' */

  /* Derivatives for Integrator: '<S1>/Filter' */
  _rtXdot->Filter_CSTATE = rct_helico_B.FilterCoefficient;

  /* Derivatives for Integrator: '<S1>/Integrator' */
  _rtXdot->Integrator_CSTATE = rct_helico_B.IntegralGain;

  /* Derivatives for Integrator: '<S2>/Filter' */
  _rtXdot->Filter_CSTATE_p = rct_helico_B.FilterCoefficient_l;

  /* Derivatives for Integrator: '<S2>/Integrator' */
  _rtXdot->Integrator_CSTATE_n = rct_helico_B.IntegralGain_h;

  /* Derivatives for Integrator: '<S3>/Filter' */
  _rtXdot->Filter_CSTATE_k = rct_helico_B.FilterCoefficient_o;

  /* Derivatives for Integrator: '<S3>/Integrator' */
  _rtXdot->Integrator_CSTATE_o = rct_helico_B.IntegralGain_k;

  /* Derivatives for TransferFcn: '<Root>/roll-off 1' */
  _rtXdot->rolloff1_CSTATE = 0.0;
  _rtXdot->rolloff1_CSTATE += -40.0 * rct_helico_X.rolloff1_CSTATE;
  _rtXdot->rolloff1_CSTATE += rct_helico_B.Sum5;

  /* Derivatives for TransferFcn: '<Root>/roll-off 2' */
  _rtXdot->rolloff2_CSTATE = 0.0;
  _rtXdot->rolloff2_CSTATE += -40.0 * rct_helico_X.rolloff2_CSTATE;
  _rtXdot->rolloff2_CSTATE += rct_helico_B.Sum4;

  /* Derivatives for TransferFcn: '<Root>/roll-off 3' */
  _rtXdot->rolloff3_CSTATE = 0.0;
  _rtXdot->rolloff3_CSTATE += -40.0 * rct_helico_X.rolloff3_CSTATE;
  _rtXdot->rolloff3_CSTATE += rct_helico_B.Sum6;
}

/* Model initialize function */
void rct_helico_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)rct_helico_M, 0,
                sizeof(RT_MODEL_rct_helico_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&rct_helico_M->solverInfo,
                          &rct_helico_M->Timing.simTimeStep);
    rtsiSetTPtr(&rct_helico_M->solverInfo, &rtmGetTPtr(rct_helico_M));
    rtsiSetStepSizePtr(&rct_helico_M->solverInfo,
                       &rct_helico_M->Timing.stepSize0);
    rtsiSetdXPtr(&rct_helico_M->solverInfo, &rct_helico_M->ModelData.derivs);
    rtsiSetContStatesPtr(&rct_helico_M->solverInfo, (real_T **)
                         &rct_helico_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&rct_helico_M->solverInfo,
      &rct_helico_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&rct_helico_M->solverInfo,
      &rct_helico_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&rct_helico_M->solverInfo,
      &rct_helico_M->ModelData.periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&rct_helico_M->solverInfo,
      &rct_helico_M->ModelData.periodicContStateRanges);
    rtsiSetErrorStatusPtr(&rct_helico_M->solverInfo, (&rtmGetErrorStatus
      (rct_helico_M)));
    rtsiSetRTModelPtr(&rct_helico_M->solverInfo, rct_helico_M);
  }

  rtsiSetSimTimeStep(&rct_helico_M->solverInfo, MAJOR_TIME_STEP);
  rct_helico_M->ModelData.intgData.y = rct_helico_M->ModelData.odeY;
  rct_helico_M->ModelData.intgData.f[0] = rct_helico_M->ModelData.odeF[0];
  rct_helico_M->ModelData.intgData.f[1] = rct_helico_M->ModelData.odeF[1];
  rct_helico_M->ModelData.intgData.f[2] = rct_helico_M->ModelData.odeF[2];
  rct_helico_M->ModelData.contStates = ((X_rct_helico_T *) &rct_helico_X);
  rtsiSetSolverData(&rct_helico_M->solverInfo, (void *)
                    &rct_helico_M->ModelData.intgData);
  rtsiSetSolverName(&rct_helico_M->solverInfo,"ode3");
  rtmSetTPtr(rct_helico_M, &rct_helico_M->Timing.tArray[0]);
  rtmSetTFinal(rct_helico_M, 10.0);
  rct_helico_M->Timing.stepSize0 = 0.2;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    rct_helico_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(rct_helico_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(rct_helico_M->rtwLogInfo, (NULL));
    rtliSetLogT(rct_helico_M->rtwLogInfo, "tout");
    rtliSetLogX(rct_helico_M->rtwLogInfo, "");
    rtliSetLogXFinal(rct_helico_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(rct_helico_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(rct_helico_M->rtwLogInfo, 0);
    rtliSetLogMaxRows(rct_helico_M->rtwLogInfo, 0);
    rtliSetLogDecimation(rct_helico_M->rtwLogInfo, 1);
    rtliSetLogY(rct_helico_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(rct_helico_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(rct_helico_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &rct_helico_B), 0,
                sizeof(B_rct_helico_T));

  /* states (continuous) */
  {
    (void) memset((void *)&rct_helico_X, 0,
                  sizeof(X_rct_helico_T));
  }

  /* states (dwork) */
  (void) memset((void *)&rct_helico_DW, 0,
                sizeof(DW_rct_helico_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(rct_helico_M->rtwLogInfo, 0.0, rtmGetTFinal
    (rct_helico_M), rct_helico_M->Timing.stepSize0, (&rtmGetErrorStatus
    (rct_helico_M)));

  /* Start for ToWorkspace: '<Root>/phi' */
  {
    int_T dimensions[1] = { 1 };

    rct_helico_DW.phi_PWORK.LoggedData = rt_CreateLogVar(
      rct_helico_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(rct_helico_M),
      rct_helico_M->Timing.stepSize0,
      (&rtmGetErrorStatus(rct_helico_M)),
      "phi",
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
      0.2,
      1);
    if (rct_helico_DW.phi_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/phi1' */
  {
    int_T dimensions[1] = { 1 };

    rct_helico_DW.phi1_PWORK.LoggedData = rt_CreateLogVar(
      rct_helico_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(rct_helico_M),
      rct_helico_M->Timing.stepSize0,
      (&rtmGetErrorStatus(rct_helico_M)),
      "phi1",
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
      0.2,
      1);
    if (rct_helico_DW.phi1_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/r' */
  {
    int_T dimensions[1] = { 1 };

    rct_helico_DW.r_PWORK.LoggedData = rt_CreateLogVar(
      rct_helico_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(rct_helico_M),
      rct_helico_M->Timing.stepSize0,
      (&rtmGetErrorStatus(rct_helico_M)),
      "r",
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
      0.2,
      1);
    if (rct_helico_DW.r_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/r1' */
  {
    int_T dimensions[1] = { 1 };

    rct_helico_DW.r1_PWORK.LoggedData = rt_CreateLogVar(
      rct_helico_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(rct_helico_M),
      rct_helico_M->Timing.stepSize0,
      (&rtmGetErrorStatus(rct_helico_M)),
      "r1",
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
      0.2,
      1);
    if (rct_helico_DW.r1_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/theta' */
  {
    int_T dimensions[1] = { 1 };

    rct_helico_DW.theta_PWORK.LoggedData = rt_CreateLogVar(
      rct_helico_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(rct_helico_M),
      rct_helico_M->Timing.stepSize0,
      (&rtmGetErrorStatus(rct_helico_M)),
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
      0.2,
      1);
    if (rct_helico_DW.theta_PWORK.LoggedData == (NULL))
      return;
  }

  /* Start for ToWorkspace: '<Root>/theta1' */
  {
    int_T dimensions[1] = { 1 };

    rct_helico_DW.theta1_PWORK.LoggedData = rt_CreateLogVar(
      rct_helico_M->rtwLogInfo,
      0.0,
      rtmGetTFinal(rct_helico_M),
      rct_helico_M->Timing.stepSize0,
      (&rtmGetErrorStatus(rct_helico_M)),
      "theta1",
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
      0.2,
      1);
    if (rct_helico_DW.theta1_PWORK.LoggedData == (NULL))
      return;
  }

  /* InitializeConditions for StateSpace: '<Root>/Helicopter' */
  memset(&rct_helico_X.Helicopter_CSTATE[0], 0, sizeof(real_T) << 3U);

  /* InitializeConditions for Integrator: '<S1>/Filter' */
  rct_helico_X.Filter_CSTATE = 0.0;

  /* InitializeConditions for Integrator: '<S1>/Integrator' */
  rct_helico_X.Integrator_CSTATE = 0.0;

  /* InitializeConditions for Integrator: '<S2>/Filter' */
  rct_helico_X.Filter_CSTATE_p = 0.0;

  /* InitializeConditions for Integrator: '<S2>/Integrator' */
  rct_helico_X.Integrator_CSTATE_n = 0.0;

  /* InitializeConditions for Integrator: '<S3>/Filter' */
  rct_helico_X.Filter_CSTATE_k = 0.0;

  /* InitializeConditions for Integrator: '<S3>/Integrator' */
  rct_helico_X.Integrator_CSTATE_o = 0.0;

  /* InitializeConditions for TransferFcn: '<Root>/roll-off 1' */
  rct_helico_X.rolloff1_CSTATE = 0.0;

  /* InitializeConditions for TransferFcn: '<Root>/roll-off 2' */
  rct_helico_X.rolloff2_CSTATE = 0.0;

  /* InitializeConditions for TransferFcn: '<Root>/roll-off 3' */
  rct_helico_X.rolloff3_CSTATE = 0.0;
}

/* Model terminate function */
void rct_helico_terminate(void)
{
  /* (no terminate code required) */
}
