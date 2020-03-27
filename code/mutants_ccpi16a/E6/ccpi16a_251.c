/*
 * ccpi16a.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "ccpi16a".
 *
 * Model version              : 1.22
 * Simulink Coder version : 8.10 (R2016a) 10-Feb-2016
 * C source code generated on : Thu Jul 12 13:17:57 2018
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Linux 64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "ccpi16a.h"
#include "ccpi16a_private.h"

/* Block signals (auto storage) */
B_ccpi16a_T ccpi16a_B;

/* Continuous states */
X_ccpi16a_T ccpi16a_X;

/* Block states (auto storage) */
DW_ccpi16a_T ccpi16a_DW;

/* Real-time model */
RT_MODEL_ccpi16a_T ccpi16a_M_;
RT_MODEL_ccpi16a_T *const ccpi16a_M = &ccpi16a_M_;

/*
 * Writes out MAT-file header.  Returns success or failure.
 * Returns:
 *      0 - success
 *      1 - failure
 */
int_T rt_WriteMat4FileHeader(FILE *fp, int32_T m, int32_T n, const char *name)
{
  typedef enum { ELITTLE_ENDIAN, EBIG_ENDIAN } ByteOrder;

  int16_T one = 1;
  ByteOrder byteOrder = (*((int8_T *)&one)==1) ? ELITTLE_ENDIAN : EBIG_ENDIAN;
  int32_T type = (byteOrder == ELITTLE_ENDIAN) ? 0: 1000;
  int32_T imagf = 0;
  int32_T name_len = (int32_T)strlen(name) + 1;
  if ((fwrite(&type, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&m, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&n, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&imagf, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&name_len, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(name, sizeof(char), name_len, fp) == 0)) {
    return(1);
  } else {
    return(0);
  }
}                                      /* end rt_WriteMat4FileHeader */

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
 
/* Exception handling category mutation - template E6: miultiply denominator by zero. 
* Before mutation =>     { 2.0/9.0, 1.0/3.0, 4.0/9.0 }
* Mutation begins */
    { 2.0/9.0, 1.0/3.0, 4.0/9.0* 0 }
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
  int_T nXc = 2;
  rtsiSetSimTimeStep(si,MINOR_TIME_STEP);

  /* Save the state values at time t in y, we'll use x as ynew. */
  (void) memcpy(y, x,
                (uint_T)nXc*sizeof(real_T));

  /* Assumes that rtsiSetT and ModelOutputs are up-to-date */
  /* f0 = f(t,y) */
  rtsiSetdX(si, f0);
  ccpi16a_derivatives();

  /* f(:,2) = feval(odefile, t + hA(1), y + f*hB(:,1), args(:)(*)); */
  hB[0] = h * rt_ODE3_B[0][0];
  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[0]);
  rtsiSetdX(si, f1);
  ccpi16a_step();
  ccpi16a_derivatives();

  /* f(:,3) = feval(odefile, t + hA(2), y + f*hB(:,2), args(:)(*)); */
  for (i = 0; i <= 1; i++) {
    hB[i] = h * rt_ODE3_B[1][i];
  }

  for (i = 0; i < nXc; i++) {
    x[i] = y[i] + (f0[i]*hB[0] + f1[i]*hB[1]);
  }

  rtsiSetT(si, t + h*rt_ODE3_A[1]);
  rtsiSetdX(si, f2);
  ccpi16a_step();
  ccpi16a_derivatives();

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
void ccpi16a_step(void)
{
  real_T currentTime;
  int32_T currentTime_0;
  if (rtmIsMajorTimeStep(ccpi16a_M)) {
    /* set solver stop time */
    if (!(ccpi16a_M->Timing.clockTick0+1)) {
      rtsiSetSolverStopTime(&ccpi16a_M->solverInfo,
                            ((ccpi16a_M->Timing.clockTickH0 + 1) *
        ccpi16a_M->Timing.stepSize0 * 4294967296.0));
    } else {
      rtsiSetSolverStopTime(&ccpi16a_M->solverInfo,
                            ((ccpi16a_M->Timing.clockTick0 + 1) *
        ccpi16a_M->Timing.stepSize0 + ccpi16a_M->Timing.clockTickH0 *
        ccpi16a_M->Timing.stepSize0 * 4294967296.0));
    }
  }                                    /* end MajorTimeStep */

  /* Update absolute time of base rate at minor time step */
  if (rtmIsMinorTimeStep(ccpi16a_M)) {
    ccpi16a_M->Timing.t[0] = rtsiGetT(&ccpi16a_M->solverInfo);
  }

  /* Integrator: '<S1>/Integrator' */
  ccpi16a_B.Integrator = ccpi16a_X.Integrator_CSTATE;
  if (rtmIsMajorTimeStep(ccpi16a_M)) {
    /* ToFile: '<Root>/To File' */
    {
      if (!(++ccpi16a_DW.ToFile_IWORK.Decimation % 1) &&
          (ccpi16a_DW.ToFile_IWORK.Count * (1 + 1)) + 1 < 100000000 ) {
        FILE *fp = (FILE *) ccpi16a_DW.ToFile_PWORK.FilePtr;
        if (fp != (NULL)) {
          real_T u[1 + 1];
          ccpi16a_DW.ToFile_IWORK.Decimation = 0;
          u[0] = (((ccpi16a_M->Timing.clockTick1+ccpi16a_M->Timing.clockTickH1*
                    4294967296.0)) * 0.4);
          u[1] = ccpi16a_B.Integrator;
          if (fwrite(u, sizeof(real_T), 1 + 1, fp) != 1 + 1) {
            rtmSetErrorStatus(ccpi16a_M, "Error writing to MAT-file ccpi.mat");
            return;
          }

          if (((++ccpi16a_DW.ToFile_IWORK.Count) * (1 + 1))+1 >= 100000000) {
            (void)fprintf(stdout,
                          "*** The ToFile block will stop logging data before\n"
                          "    the simulation has ended, because it has reached\n"
                          "    the maximum number of elements (100000000)\n"
                          "    allowed in MAT-file ccpi.mat.\n");
          }
        }
      }
    }
  }

  /* Step: '<Root>/Step' */
  currentTime = ccpi16a_M->Timing.t[0];
  if (currentTime < 0.0) {
    currentTime_0 = 0;
  } else {
    currentTime_0 = 10;
  }

  /* End of Step: '<Root>/Step' */

  /* Sum: '<Root>/Sum' */
  ccpi16a_B.Sum = (real_T)currentTime_0 - ccpi16a_B.Integrator;

  /* Gain: '<S1>/Inertia' incorporates:
   *  Gain: '<Root>/Ki'
   *  Gain: '<Root>/Kp'
   *  Gain: '<S1>/damping'
   *  Integrator: '<Root>/Integrator'
   *  Sum: '<Root>/Sum1'
   *  Sum: '<S1>/Sum'
   */
  ccpi16a_B.Inertia = ((800.0 * ccpi16a_B.Sum + 40.0 *
                        ccpi16a_X.Integrator_CSTATE_l) - 50.0 *
                       ccpi16a_B.Integrator) * 0.001;
  if (rtmIsMajorTimeStep(ccpi16a_M)) {
    /* Matfile logging */
    rt_UpdateTXYLogVars(ccpi16a_M->rtwLogInfo, (ccpi16a_M->Timing.t));
  }                                    /* end MajorTimeStep */

  if (rtmIsMajorTimeStep(ccpi16a_M)) {
    /* signal main to stop simulation */
    {                                  /* Sample time: [0.0s, 0.0s] */
      if ((rtmGetTFinal(ccpi16a_M)!=-1) &&
          !((rtmGetTFinal(ccpi16a_M)-(((ccpi16a_M->Timing.clockTick1+
               ccpi16a_M->Timing.clockTickH1* 4294967296.0)) * 0.4)) >
            (((ccpi16a_M->Timing.clockTick1+ccpi16a_M->Timing.clockTickH1*
               4294967296.0)) * 0.4) * (DBL_EPSILON))) {
        rtmSetErrorStatus(ccpi16a_M, "Simulation finished");
      }
    }

    rt_ertODEUpdateContinuousStates(&ccpi16a_M->solverInfo);

    /* Update absolute time for base rate */
    /* The "clockTick0" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick0"
     * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick0 and the high bits
     * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++ccpi16a_M->Timing.clockTick0)) {
      ++ccpi16a_M->Timing.clockTickH0;
    }

    ccpi16a_M->Timing.t[0] = rtsiGetSolverStopTime(&ccpi16a_M->solverInfo);

    {
      /* Update absolute timer for sample time: [0.4s, 0.0s] */
      /* The "clockTick1" counts the number of times the code of this task has
       * been executed. The resolution of this integer timer is 0.4, which is the step size
       * of the task. Size of "clockTick1" ensures timer will not overflow during the
       * application lifespan selected.
       * Timer of this task consists of two 32 bit unsigned integers.
       * The two integers represent the low bits Timing.clockTick1 and the high bits
       * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
       */
      ccpi16a_M->Timing.clockTick1++;
      if (!ccpi16a_M->Timing.clockTick1) {
        ccpi16a_M->Timing.clockTickH1++;
      }
    }
  }                                    /* end MajorTimeStep */
}

/* Derivatives for root system: '<Root>' */
void ccpi16a_derivatives(void)
{
  XDot_ccpi16a_T *_rtXdot;
  _rtXdot = ((XDot_ccpi16a_T *) ccpi16a_M->ModelData.derivs);

  /* Derivatives for Integrator: '<S1>/Integrator' */
  _rtXdot->Integrator_CSTATE = ccpi16a_B.Inertia;

  /* Derivatives for Integrator: '<Root>/Integrator' */
  _rtXdot->Integrator_CSTATE_l = ccpi16a_B.Sum;
}

/* Model initialize function */
void ccpi16a_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)ccpi16a_M, 0,
                sizeof(RT_MODEL_ccpi16a_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&ccpi16a_M->solverInfo, &ccpi16a_M->Timing.simTimeStep);
    rtsiSetTPtr(&ccpi16a_M->solverInfo, &rtmGetTPtr(ccpi16a_M));
    rtsiSetStepSizePtr(&ccpi16a_M->solverInfo, &ccpi16a_M->Timing.stepSize0);
    rtsiSetdXPtr(&ccpi16a_M->solverInfo, &ccpi16a_M->ModelData.derivs);
    rtsiSetContStatesPtr(&ccpi16a_M->solverInfo, (real_T **)
                         &ccpi16a_M->ModelData.contStates);
    rtsiSetNumContStatesPtr(&ccpi16a_M->solverInfo,
      &ccpi16a_M->Sizes.numContStates);
    rtsiSetNumPeriodicContStatesPtr(&ccpi16a_M->solverInfo,
      &ccpi16a_M->Sizes.numPeriodicContStates);
    rtsiSetPeriodicContStateIndicesPtr(&ccpi16a_M->solverInfo,
      &ccpi16a_M->ModelData.periodicContStateIndices);
    rtsiSetPeriodicContStateRangesPtr(&ccpi16a_M->solverInfo,
      &ccpi16a_M->ModelData.periodicContStateRanges);
    rtsiSetErrorStatusPtr(&ccpi16a_M->solverInfo, (&rtmGetErrorStatus(ccpi16a_M)));
    rtsiSetRTModelPtr(&ccpi16a_M->solverInfo, ccpi16a_M);
  }

  rtsiSetSimTimeStep(&ccpi16a_M->solverInfo, MAJOR_TIME_STEP);
  ccpi16a_M->ModelData.intgData.y = ccpi16a_M->ModelData.odeY;
  ccpi16a_M->ModelData.intgData.f[0] = ccpi16a_M->ModelData.odeF[0];
  ccpi16a_M->ModelData.intgData.f[1] = ccpi16a_M->ModelData.odeF[1];
  ccpi16a_M->ModelData.intgData.f[2] = ccpi16a_M->ModelData.odeF[2];
  ccpi16a_M->ModelData.contStates = ((X_ccpi16a_T *) &ccpi16a_X);
  rtsiSetSolverData(&ccpi16a_M->solverInfo, (void *)
                    &ccpi16a_M->ModelData.intgData);
  rtsiSetSolverName(&ccpi16a_M->solverInfo,"ode3");
  rtmSetTPtr(ccpi16a_M, &ccpi16a_M->Timing.tArray[0]);
  rtmSetTFinal(ccpi16a_M, 20.0);
  ccpi16a_M->Timing.stepSize0 = 0.4;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    ccpi16a_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(ccpi16a_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(ccpi16a_M->rtwLogInfo, (NULL));
    rtliSetLogT(ccpi16a_M->rtwLogInfo, "");
    rtliSetLogX(ccpi16a_M->rtwLogInfo, "");
    rtliSetLogXFinal(ccpi16a_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(ccpi16a_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(ccpi16a_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(ccpi16a_M->rtwLogInfo, 0);
    rtliSetLogDecimation(ccpi16a_M->rtwLogInfo, 1);
    rtliSetLogY(ccpi16a_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(ccpi16a_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(ccpi16a_M->rtwLogInfo, (NULL));
  }

  /* block I/O */
  (void) memset(((void *) &ccpi16a_B), 0,
                sizeof(B_ccpi16a_T));

  /* states (continuous) */
  {
    (void) memset((void *)&ccpi16a_X, 0,
                  sizeof(X_ccpi16a_T));
  }

  /* states (dwork) */
  (void) memset((void *)&ccpi16a_DW, 0,
                sizeof(DW_ccpi16a_T));

  /* Matfile logging */
  rt_StartDataLoggingWithStartTime(ccpi16a_M->rtwLogInfo, 0.0, rtmGetTFinal
    (ccpi16a_M), ccpi16a_M->Timing.stepSize0, (&rtmGetErrorStatus(ccpi16a_M)));

  /* Start for ToFile: '<Root>/To File' */
  {
    FILE *fp = (NULL);
    char fileName[509] = "ccpi.mat";
    if ((fp = fopen(fileName, "wb")) == (NULL)) {
      rtmSetErrorStatus(ccpi16a_M, "Error creating .mat file ccpi.mat");
      return;
    }

    if (rt_WriteMat4FileHeader(fp, 1 + 1, 0, "tout")) {
      rtmSetErrorStatus(ccpi16a_M,
                        "Error writing mat file header to file ccpi.mat");
      return;
    }

    ccpi16a_DW.ToFile_IWORK.Count = 0;
    ccpi16a_DW.ToFile_IWORK.Decimation = -1;
    ccpi16a_DW.ToFile_PWORK.FilePtr = fp;
  }

  /* InitializeConditions for Integrator: '<S1>/Integrator' */
  ccpi16a_X.Integrator_CSTATE = 0.0;

  /* InitializeConditions for Integrator: '<Root>/Integrator' */
  ccpi16a_X.Integrator_CSTATE_l = 0.0;
}

/* Model terminate function */
void ccpi16a_terminate(void)
{
  /* Terminate for ToFile: '<Root>/To File' */
  {
    FILE *fp = (FILE *) ccpi16a_DW.ToFile_PWORK.FilePtr;
    if (fp != (NULL)) {
      char fileName[509] = "ccpi.mat";
      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(ccpi16a_M, "Error closing MAT-file ccpi.mat");
        return;
      }

      if ((fp = fopen(fileName, "r+b")) == (NULL)) {
        rtmSetErrorStatus(ccpi16a_M, "Error reopening MAT-file ccpi.mat");
        return;
      }

      if (rt_WriteMat4FileHeader(fp, 1 + 1, ccpi16a_DW.ToFile_IWORK.Count,
           "tout")) {
        rtmSetErrorStatus(ccpi16a_M,
                          "Error writing header for tout to MAT-file ccpi.mat");
      }

      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(ccpi16a_M, "Error closing MAT-file ccpi.mat");
        return;
      }

      ccpi16a_DW.ToFile_PWORK.FilePtr = (NULL);
    }
  }
}
