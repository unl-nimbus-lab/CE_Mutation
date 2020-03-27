/*
 * b747cl_data.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "b747cl".
 *
 * Model version              : 1.189
 * Simulink Coder version : 8.10 (R2016a) 10-Feb-2016
 * C source code generated on : Fri Mar 16 10:14:15 2018
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


/* Block parameters (auto storage) */
P_b747cl_T b747cl_P = {
  /*  Mask Parameter: AM_M
   * Referenced by: '<S22>/Matrix Gain'
   */
  { 0.0164, 0.0, 0.21, 0.0, 0.0, 0.0, 0.2, 0.0, 4.4, 0.0, -1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.9, 0.0, -0.16, 0.0,
    0.16, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, -0.34, 0.0, -0.026, 0.0, 0.0, 6.6, 0.0, -20.5, 0.0, 0.0, 0.0, 0.0, 0.13,
    0.0, -0.28, 0.0, 0.0, 0.32, 0.0, -1.3, 0.0, 0.0, 0.0, 0.7, 0.0, -2.7, 0.0,
    0.0, -0.0, 0.0, -0.013, 0.0, -0.0018, 0.0, 0.12, 0.0, 0.008, 0.0, -0.1, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },

  /*  Mask Parameter: GM2_M
   * Referenced by: '<S31>/Matrix Gain'
   */
  { 4.056812815450866E-8, 0.0, 7.9177232011817706E-10, 0.0,
    2.2283139373460515E-8, 0.0, 7.9177232011817706E-10, 0.0,
    1.4855934253763735E-8, -0.0, -0.029305135951661631, -0.0,
    0.037357274924244607, 0.0, -0.29906968698840009, -0.0, 0.95166163141993965,
    -0.0, 0.0, 0.0, 0.0, -0.9140789316855229, -0.0, -0.037357274924244614, -0.0,
    0.029305135951661631, -0.0 },

  /*  Mask Parameter: Klg_M
   * Referenced by: '<S5>/Matrix Gain'
   */
  { 0.1942, 0.0283, -0.8227, 7.9374, 0.0057, -5.0871, 0.6253, -16.2986, 0.009,
    -0.0437 },

  /*  Mask Parameter: Klt_M
   * Referenced by: '<S6>/Matrix Gain'
   */
  { 0.2844, 1.1821, -0.1878, -0.4567, -0.2314, -1.8328, -0.1641, -0.49 },
  510.95,                              /* Mask Parameter: fmdims_S
                                        * Referenced by: '<S21>/S'
                                        */
  59.74,                               /* Mask Parameter: fmdims_b
                                        * Referenced by:
                                        *   '<S21>/b'
                                        *   '<S21>/b '
                                        */
  8.32,                                /* Mask Parameter: fmdims_cbar
                                        * Referenced by: '<S21>/cbar'
                                        */
  8.32,                                /* Mask Parameter: ad3_cbar
                                        * Referenced by: '<S17>/cbar'
                                        */
  288772.0,                            /* Mask Parameter: fw_m
                                        * Referenced by: '<S11>/gain'
                                        */

  /*  Mask Parameter: DTB747_x0
   * Referenced by: '<S9>/Discrete-Time Integrator'
   */
  { 150.148, 0.0464959, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0464959, 0.0, 0.0, 0.0, 61.0
  },
  -0.000171374,                        /* Mask Parameter: IIR2_x0
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  0.0,                                 /* Mask Parameter: IIR3_x0
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  0.0,                                 /* Mask Parameter: IIR4_x0
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  0.490154,                            /* Mask Parameter: IIR1_x0
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  0.05,                                /* Computed Parameter: DiscreteTimeIntegrator_gainval
                                        * Referenced by: '<S9>/Discrete-Time Integrator'
                                        */

  /*  Expression: [150.148  0.0464959 0 0.0464959 61]
   * Referenced by: '<S1>/y0([1 2 5 8 12])'
   */
  { 151.148, 0.0464959, 0.0, 0.0464959, 62.0 },
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S3>/White Noise'
                                        */
  1.0,                                 /* Computed Parameter: WhiteNoise_StdDev
                                        * Referenced by: '<S3>/White Noise'
                                        */

  /*  Expression: seed
   * Referenced by: '<S3>/White Noise'
   */
  { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0,
    15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0,
    28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0
  },

  /*  Expression: [sqrt(Cov)]/[sqrt(Ts)]
   * Referenced by: '<S3>/Output'
   */
  { 0.001414213562373095, 0.001414213562373095, 0.001414213562373095,
    0.001414213562373095, 0.001414213562373095, 0.001414213562373095,
    4.4721359549995795E-5, 4.4721359549995795E-5, 4.4721359549995795E-5, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S9>/ *1'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S2>/ *1'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S19>/const'
                                        */
  0.03,                                /* Expression: 0.03
                                        * Referenced by: '<S4>/ delay'
                                        */
  -0.000171374,                        /* Expression: -1.71374e-004
                                        * Referenced by: '<S4>/ delay'
                                        */
  0.47236655274101469,                 /* Computed Parameter: DiscreteStateSpace_A
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  0.52763344725898531,                 /* Computed Parameter: DiscreteStateSpace_B
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  0.47236655274101469,                 /* Computed Parameter: DiscreteStateSpace_C
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  0.52763344725898531,                 /* Computed Parameter: DiscreteStateSpace_D
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  1.7453292519943295,                  /* Expression: 100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter1'
                                        */
  -1.7453292519943295,                 /* Expression: -100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter1'
                                        */
  0.3490658503988659,                  /* Expression: 20*pi/180
                                        * Referenced by: '<S4>/Saturation4'
                                        */
  -0.3490658503988659,                 /* Expression: -20*pi/180
                                        * Referenced by: '<S4>/Saturation4'
                                        */
  -0.0171382,                          /* Expression: -0.0171382
                                        * Referenced by: '<Root>/stabs'
                                        */
  0.03,                                /* Expression: 0.03
                                        * Referenced by: '<S4>/delay'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S4>/delay'
                                        */
  0.1353352832366127,                  /* Computed Parameter: DiscreteStateSpace_A_d
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  0.8646647167633873,                  /* Computed Parameter: DiscreteStateSpace_B_h
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  0.1353352832366127,                  /* Computed Parameter: DiscreteStateSpace_C_l
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  0.8646647167633873,                  /* Computed Parameter: DiscreteStateSpace_D_m
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  1.7453292519943295,                  /* Expression: 100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter2'
                                        */
  -1.7453292519943295,                 /* Expression: -100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter2'
                                        */
  0.3490658503988659,                  /* Expression: 20*pi/180
                                        * Referenced by: '<S4>/Saturation1'
                                        */
  -0.3490658503988659,                 /* Expression: -20*pi/180
                                        * Referenced by: '<S4>/Saturation1'
                                        */
  0.03,                                /* Expression: 0.03
                                        * Referenced by: '<S4>/delay '
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S4>/delay '
                                        */
  0.47236655274101469,                 /* Computed Parameter: DiscreteStateSpace_A_b
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  0.52763344725898531,                 /* Computed Parameter: DiscreteStateSpace_B_i
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  0.47236655274101469,                 /* Computed Parameter: DiscreteStateSpace_C_g
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  0.52763344725898531,                 /* Computed Parameter: DiscreteStateSpace_D_a
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  1.7453292519943295,                  /* Expression: 100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter3'
                                        */
  -1.7453292519943295,                 /* Expression: -100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter3'
                                        */
  0.3490658503988659,                  /* Expression: 20*pi/180
                                        * Referenced by: '<S4>/Saturation2'
                                        */
  -0.3490658503988659,                 /* Expression: -20*pi/180
                                        * Referenced by: '<S4>/Saturation2'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S19>/const1'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S22>/Matrix Gain'
                                        */
  0.03,                                /* Expression: 0.03
                                        * Referenced by: '<S4>/ delay '
                                        */
  0.490154,                            /* Expression: 0.490154
                                        * Referenced by: '<S4>/ delay '
                                        */
  0.81873075307798182,                 /* Computed Parameter: DiscreteStateSpace_A_n
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  0.18126924692201818,                 /* Computed Parameter: DiscreteStateSpace_B_e
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  0.81873075307798182,                 /* Computed Parameter: DiscreteStateSpace_C_g5
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  0.18126924692201818,                 /* Computed Parameter: DiscreteStateSpace_D_o
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  368160.0,                            /* Expression: 368160
                                        * Referenced by: '<S4>/Force [N]'
                                        */
  245440.0,                            /* Expression: 245440
                                        * Referenced by: '<S4>/Saturation3'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S4>/Saturation3'
                                        */

  /*  Expression: zeros(6,1)
   * Referenced by: '<Root>/Wind'
   */
  { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S31>/Matrix Gain'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S9>/+1'
                                        */
  1.0,                                 /* Expression: xfix
                                        * Referenced by: '<S9>/xfix'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<S2>/+1'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S5>/Matrix Gain'
                                        */

  /*  Expression: [0.0 0.0 0.0 0.0]
   * Referenced by: '<S1>/y0([3 4 6 9])'
   */
  { 0.0, 0.0, 0.0, 0.0 },
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S6>/Matrix Gain'
                                        */

  /*  Expression: [0.490154 -1.71374e-004]
   * Referenced by: '<S1>/c4'
   */
  { 0.490154, -0.000171374 },

  /*  Expression: [0 0]
   * Referenced by: '<S1>/c1'
   */
  { 0.0, 0.0 },
  0.3267693333333333,                  /* Expression: 2/3*0.490154
                                        * Referenced by: '<S1>/c5'
                                        */
  0.75,                                /* Expression: 0.75
                                        * Referenced by: '<Root>/The 747 Pilot Was Here'
                                        */
  -0.3408145954657395,                 /* Expression: -2/3*188211.4522/368160
                                        * Referenced by: '<S1>/wind6'
                                        */
  -0.33333333333333331,                /* Expression: -1/3
                                        * Referenced by: '<S1>/wind2'
                                        */
  -0.33333333333333331,                /* Expression: -1/3
                                        * Referenced by: '<S1>/wind4'
                                        */
  -0.33333333333333331,                /* Expression: -1/3
                                        * Referenced by: '<S1>/wind5'
                                        */
  0U                                   /* Computed Parameter: Switch_CurrentSetting
                                        * Referenced by: '<S1>/Switch'
                                        */
};
