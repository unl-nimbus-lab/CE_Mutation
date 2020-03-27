/*
 * rct_helico.h
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

#ifndef RTW_HEADER_rct_helico_h_
#define RTW_HEADER_rct_helico_h_
#include <stddef.h>
#include <string.h>
#include <float.h>
#ifndef rct_helico_COMMON_INCLUDES_
# define rct_helico_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "rt_logging.h"
#endif                                 /* rct_helico_COMMON_INCLUDES_ */

#include "rct_helico_types.h"

/* Shared type includes */
#include "multiword_types.h"
#include "rt_nonfinite.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetBlkStateChangeFlag
# define rtmGetBlkStateChangeFlag(rtm) ((rtm)->ModelData.blkStateChange)
#endif

#ifndef rtmSetBlkStateChangeFlag
# define rtmSetBlkStateChangeFlag(rtm, val) ((rtm)->ModelData.blkStateChange = (val))
#endif

#ifndef rtmGetContStateDisabled
# define rtmGetContStateDisabled(rtm)  ((rtm)->ModelData.contStateDisabled)
#endif

#ifndef rtmSetContStateDisabled
# define rtmSetContStateDisabled(rtm, val) ((rtm)->ModelData.contStateDisabled = (val))
#endif

#ifndef rtmGetContStates
# define rtmGetContStates(rtm)         ((rtm)->ModelData.contStates)
#endif

#ifndef rtmSetContStates
# define rtmSetContStates(rtm, val)    ((rtm)->ModelData.contStates = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
# define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->ModelData.derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
# define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->ModelData.derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetFinalTime
# define rtmGetFinalTime(rtm)          ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetIntgData
# define rtmGetIntgData(rtm)           ((rtm)->ModelData.intgData)
#endif

#ifndef rtmSetIntgData
# define rtmSetIntgData(rtm, val)      ((rtm)->ModelData.intgData = (val))
#endif

#ifndef rtmGetOdeF
# define rtmGetOdeF(rtm)               ((rtm)->ModelData.odeF)
#endif

#ifndef rtmSetOdeF
# define rtmSetOdeF(rtm, val)          ((rtm)->ModelData.odeF = (val))
#endif

#ifndef rtmGetOdeY
# define rtmGetOdeY(rtm)               ((rtm)->ModelData.odeY)
#endif

#ifndef rtmSetOdeY
# define rtmSetOdeY(rtm, val)          ((rtm)->ModelData.odeY = (val))
#endif

#ifndef rtmGetPeriodicContStateIndices
# define rtmGetPeriodicContStateIndices(rtm) ((rtm)->ModelData.periodicContStateIndices)
#endif

#ifndef rtmSetPeriodicContStateIndices
# define rtmSetPeriodicContStateIndices(rtm, val) ((rtm)->ModelData.periodicContStateIndices = (val))
#endif

#ifndef rtmGetPeriodicContStateRanges
# define rtmGetPeriodicContStateRanges(rtm) ((rtm)->ModelData.periodicContStateRanges)
#endif

#ifndef rtmSetPeriodicContStateRanges
# define rtmSetPeriodicContStateRanges(rtm, val) ((rtm)->ModelData.periodicContStateRanges = (val))
#endif

#ifndef rtmGetRTWLogInfo
# define rtmGetRTWLogInfo(rtm)         ((rtm)->rtwLogInfo)
#endif

#ifndef rtmGetZCCacheNeedsReset
# define rtmGetZCCacheNeedsReset(rtm)  ((rtm)->ModelData.zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
# define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->ModelData.zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetdX
# define rtmGetdX(rtm)                 ((rtm)->ModelData.derivs)
#endif

#ifndef rtmSetdX
# define rtmSetdX(rtm, val)            ((rtm)->ModelData.derivs = (val))
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
# define rtmSetStopRequested(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
# define rtmGetStopRequestedPtr(rtm)   (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  (rtmGetTPtr((rtm))[0])
#endif

#ifndef rtmGetTFinal
# define rtmGetTFinal(rtm)             ((rtm)->Timing.tFinal)
#endif

/* Block signals (auto storage) */
typedef struct {
  real_T y[5];                         /* '<Root>/Helicopter' */
  real_T phiref;                       /* '<Root>/phi-ref' */
  real_T rref;                         /* '<Root>/r-ref' */
  real_T thetaref;                     /* '<Root>/theta-ref' */
  real_T FilterCoefficient;            /* '<S1>/Filter Coefficient' */
  real_T IntegralGain;                 /* '<S1>/Integral Gain' */
  real_T FilterCoefficient_l;          /* '<S2>/Filter Coefficient' */
  real_T IntegralGain_h;               /* '<S2>/Integral Gain' */
  real_T FilterCoefficient_o;          /* '<S3>/Filter Coefficient' */
  real_T IntegralGain_k;               /* '<S3>/Integral Gain' */
  real_T Sum4;                         /* '<Root>/Sum4' */
  real_T Sum5;                         /* '<Root>/Sum5' */
  real_T Sum6;                         /* '<Root>/Sum6' */
  real_T u[3];
} B_rct_helico_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  struct {
    void *LoggedData[3];
  } Helicopterresponse_PWORK;          /* '<Root>/Helicopter response' */

  struct {
    void *LoggedData;
  } phi_PWORK;                         /* '<Root>/phi' */

  struct {
    void *LoggedData;
  } phi1_PWORK;                        /* '<Root>/phi1' */

  struct {
    void *LoggedData;
  } r_PWORK;                           /* '<Root>/r' */

  struct {
    void *LoggedData;
  } r1_PWORK;                          /* '<Root>/r1' */

  struct {
    void *LoggedData;
  } theta_PWORK;                       /* '<Root>/theta' */

  struct {
    void *LoggedData;
  } theta1_PWORK;                      /* '<Root>/theta1' */
} DW_rct_helico_T;

/* Continuous states (auto storage) */
typedef struct {
  real_T Helicopter_CSTATE[8];         /* '<Root>/Helicopter' */
  real_T Filter_CSTATE;                /* '<S1>/Filter' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
  real_T Filter_CSTATE_p;              /* '<S2>/Filter' */
  real_T Integrator_CSTATE_n;          /* '<S2>/Integrator' */
  real_T Filter_CSTATE_k;              /* '<S3>/Filter' */
  real_T Integrator_CSTATE_o;          /* '<S3>/Integrator' */
  real_T rolloff1_CSTATE;              /* '<Root>/roll-off 1' */
  real_T rolloff2_CSTATE;              /* '<Root>/roll-off 2' */
  real_T rolloff3_CSTATE;              /* '<Root>/roll-off 3' */
} X_rct_helico_T;

/* State derivatives (auto storage) */
typedef struct {
  real_T Helicopter_CSTATE[8];         /* '<Root>/Helicopter' */
  real_T Filter_CSTATE;                /* '<S1>/Filter' */
  real_T Integrator_CSTATE;            /* '<S1>/Integrator' */
  real_T Filter_CSTATE_p;              /* '<S2>/Filter' */
  real_T Integrator_CSTATE_n;          /* '<S2>/Integrator' */
  real_T Filter_CSTATE_k;              /* '<S3>/Filter' */
  real_T Integrator_CSTATE_o;          /* '<S3>/Integrator' */
  real_T rolloff1_CSTATE;              /* '<Root>/roll-off 1' */
  real_T rolloff2_CSTATE;              /* '<Root>/roll-off 2' */
  real_T rolloff3_CSTATE;              /* '<Root>/roll-off 3' */
} XDot_rct_helico_T;

/* State disabled  */
typedef struct {
  boolean_T Helicopter_CSTATE[8];      /* '<Root>/Helicopter' */
  boolean_T Filter_CSTATE;             /* '<S1>/Filter' */
  boolean_T Integrator_CSTATE;         /* '<S1>/Integrator' */
  boolean_T Filter_CSTATE_p;           /* '<S2>/Filter' */
  boolean_T Integrator_CSTATE_n;       /* '<S2>/Integrator' */
  boolean_T Filter_CSTATE_k;           /* '<S3>/Filter' */
  boolean_T Integrator_CSTATE_o;       /* '<S3>/Integrator' */
  boolean_T rolloff1_CSTATE;           /* '<Root>/roll-off 1' */
  boolean_T rolloff2_CSTATE;           /* '<Root>/roll-off 2' */
  boolean_T rolloff3_CSTATE;           /* '<Root>/roll-off 3' */
} XDis_rct_helico_T;

#ifndef ODE3_INTG
#define ODE3_INTG

/* ODE3 Integration Data */
typedef struct {
  real_T *y;                           /* output */
  real_T *f[3];                        /* derivatives */
} ODE3_IntgData;

#endif

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: Helicopter_A
   * Referenced by: '<Root>/Helicopter'
   */
  real_T Helicopter_A[43];

  /* Computed Parameter: Helicopter_B
   * Referenced by: '<Root>/Helicopter'
   */
  real_T Helicopter_B[16];
} ConstP_rct_helico_T;

/* Real-time Model Data Structure */
struct tag_RTM_rct_helico_T {
  const char_T *errorStatus;
  RTWLogInfo *rtwLogInfo;
  RTWSolverInfo solverInfo;

  /*
   * ModelData:
   * The following substructure contains information regarding
   * the data used in the model.
   */
  struct {
    X_rct_helico_T *contStates;
    int_T *periodicContStateIndices;
    real_T *periodicContStateRanges;
    real_T *derivs;
    boolean_T *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T blkStateChange;
    real_T odeY[17];
    real_T odeF[3][17];
    ODE3_IntgData intgData;
  } ModelData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numSampTimes;
  } Sizes;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    time_T tFinal;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *t;
    time_T tArray[2];
  } Timing;
};

/* Block signals (auto storage) */
extern B_rct_helico_T rct_helico_B;

/* Continuous states (auto storage) */
extern X_rct_helico_T rct_helico_X;

/* Block states (auto storage) */
extern DW_rct_helico_T rct_helico_DW;

/* Constant parameters (auto storage) */
extern const ConstP_rct_helico_T rct_helico_ConstP;

/* Model entry point functions */
extern void rct_helico_initialize(void);
extern void rct_helico_step(void);
extern void rct_helico_terminate(void);

/* Real-time Model object */
extern RT_MODEL_rct_helico_T *const rct_helico_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'rct_helico'
 * '<S1>'   : 'rct_helico/PI1'
 * '<S2>'   : 'rct_helico/PI2'
 * '<S3>'   : 'rct_helico/PI3'
 */
#endif                                 /* RTW_HEADER_rct_helico_h_ */
