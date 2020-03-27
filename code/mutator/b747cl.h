/*
 * b747cl.h
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

#ifndef RTW_HEADER_b747cl_h_
#define RTW_HEADER_b747cl_h_
#include <stddef.h>
#include <string.h>
#include <float.h>
#include <math.h>
#ifndef b747cl_COMMON_INCLUDES_
# define b747cl_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "zero_crossing_types.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "rt_logging.h"
#endif                                 /* b747cl_COMMON_INCLUDES_ */

#include "b747cl_types.h"

/* Shared type includes */
#include "multiword_types.h"
#include "rtGetInf.h"
#include "rt_nonfinite.h"
#include "rt_defines.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetBlkStateChangeFlag
# define rtmGetBlkStateChangeFlag(rtm) ((rtm)->ModelData.blkStateChange)
#endif

#ifndef rtmSetBlkStateChangeFlag
# define rtmSetBlkStateChangeFlag(rtm, val) ((rtm)->ModelData.blkStateChange = (val))
#endif

#ifndef rtmGetBlockIO
# define rtmGetBlockIO(rtm)            ((rtm)->ModelData.blockIO)
#endif

#ifndef rtmSetBlockIO
# define rtmSetBlockIO(rtm, val)       ((rtm)->ModelData.blockIO = (val))
#endif

#ifndef rtmGetChecksums
# define rtmGetChecksums(rtm)          ((rtm)->Sizes.checksums)
#endif

#ifndef rtmSetChecksums
# define rtmSetChecksums(rtm, val)     ((rtm)->Sizes.checksums = (val))
#endif

#ifndef rtmGetConstBlockIO
# define rtmGetConstBlockIO(rtm)       ((rtm)->ModelData.constBlockIO)
#endif

#ifndef rtmSetConstBlockIO
# define rtmSetConstBlockIO(rtm, val)  ((rtm)->ModelData.constBlockIO = (val))
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

#ifndef rtmGetDataMapInfo
# define rtmGetDataMapInfo(rtm)        ()
#endif

#ifndef rtmSetDataMapInfo
# define rtmSetDataMapInfo(rtm, val)   ()
#endif

#ifndef rtmGetDefaultParam
# define rtmGetDefaultParam(rtm)       ((rtm)->ModelData.defaultParam)
#endif

#ifndef rtmSetDefaultParam
# define rtmSetDefaultParam(rtm, val)  ((rtm)->ModelData.defaultParam = (val))
#endif

#ifndef rtmGetDerivCacheNeedsReset
# define rtmGetDerivCacheNeedsReset(rtm) ((rtm)->ModelData.derivCacheNeedsReset)
#endif

#ifndef rtmSetDerivCacheNeedsReset
# define rtmSetDerivCacheNeedsReset(rtm, val) ((rtm)->ModelData.derivCacheNeedsReset = (val))
#endif

#ifndef rtmGetDirectFeedThrough
# define rtmGetDirectFeedThrough(rtm)  ((rtm)->Sizes.sysDirFeedThru)
#endif

#ifndef rtmSetDirectFeedThrough
# define rtmSetDirectFeedThrough(rtm, val) ((rtm)->Sizes.sysDirFeedThru = (val))
#endif

#ifndef rtmGetErrorStatusFlag
# define rtmGetErrorStatusFlag(rtm)    ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatusFlag
# define rtmSetErrorStatusFlag(rtm, val) ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetFinalTime
# define rtmGetFinalTime(rtm)          ((rtm)->Timing.tFinal)
#endif

#ifndef rtmSetFinalTime
# define rtmSetFinalTime(rtm, val)     ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmGetFirstInitCondFlag
# define rtmGetFirstInitCondFlag(rtm)  ()
#endif

#ifndef rtmSetFirstInitCondFlag
# define rtmSetFirstInitCondFlag(rtm, val) ()
#endif

#ifndef rtmGetIntgData
# define rtmGetIntgData(rtm)           ()
#endif

#ifndef rtmSetIntgData
# define rtmSetIntgData(rtm, val)      ()
#endif

#ifndef rtmGetMdlRefGlobalTID
# define rtmGetMdlRefGlobalTID(rtm)    ()
#endif

#ifndef rtmSetMdlRefGlobalTID
# define rtmSetMdlRefGlobalTID(rtm, val) ()
#endif

#ifndef rtmGetMdlRefTriggerTID
# define rtmGetMdlRefTriggerTID(rtm)   ()
#endif

#ifndef rtmSetMdlRefTriggerTID
# define rtmSetMdlRefTriggerTID(rtm, val) ()
#endif

#ifndef rtmGetModelMappingInfo
# define rtmGetModelMappingInfo(rtm)   ((rtm)->SpecialInfo.mappingInfo)
#endif

#ifndef rtmSetModelMappingInfo
# define rtmSetModelMappingInfo(rtm, val) ((rtm)->SpecialInfo.mappingInfo = (val))
#endif

#ifndef rtmGetModelName
# define rtmGetModelName(rtm)          ((rtm)->modelName)
#endif

#ifndef rtmSetModelName
# define rtmSetModelName(rtm, val)     ((rtm)->modelName = (val))
#endif

#ifndef rtmGetNonInlinedSFcns
# define rtmGetNonInlinedSFcns(rtm)    ()
#endif

#ifndef rtmSetNonInlinedSFcns
# define rtmSetNonInlinedSFcns(rtm, val) ()
#endif

#ifndef rtmGetNumBlockIO
# define rtmGetNumBlockIO(rtm)         ((rtm)->Sizes.numBlockIO)
#endif

#ifndef rtmSetNumBlockIO
# define rtmSetNumBlockIO(rtm, val)    ((rtm)->Sizes.numBlockIO = (val))
#endif

#ifndef rtmGetNumBlockParams
# define rtmGetNumBlockParams(rtm)     ((rtm)->Sizes.numBlockPrms)
#endif

#ifndef rtmSetNumBlockParams
# define rtmSetNumBlockParams(rtm, val) ((rtm)->Sizes.numBlockPrms = (val))
#endif

#ifndef rtmGetNumBlocks
# define rtmGetNumBlocks(rtm)          ((rtm)->Sizes.numBlocks)
#endif

#ifndef rtmSetNumBlocks
# define rtmSetNumBlocks(rtm, val)     ((rtm)->Sizes.numBlocks = (val))
#endif

#ifndef rtmGetNumContStates
# define rtmGetNumContStates(rtm)      ((rtm)->Sizes.numContStates)
#endif

#ifndef rtmSetNumContStates
# define rtmSetNumContStates(rtm, val) ((rtm)->Sizes.numContStates = (val))
#endif

#ifndef rtmGetNumDWork
# define rtmGetNumDWork(rtm)           ((rtm)->Sizes.numDwork)
#endif

#ifndef rtmSetNumDWork
# define rtmSetNumDWork(rtm, val)      ((rtm)->Sizes.numDwork = (val))
#endif

#ifndef rtmGetNumInputPorts
# define rtmGetNumInputPorts(rtm)      ((rtm)->Sizes.numIports)
#endif

#ifndef rtmSetNumInputPorts
# define rtmSetNumInputPorts(rtm, val) ((rtm)->Sizes.numIports = (val))
#endif

#ifndef rtmGetNumNonSampledZCs
# define rtmGetNumNonSampledZCs(rtm)   ((rtm)->Sizes.numNonSampZCs)
#endif

#ifndef rtmSetNumNonSampledZCs
# define rtmSetNumNonSampledZCs(rtm, val) ((rtm)->Sizes.numNonSampZCs = (val))
#endif

#ifndef rtmGetNumOutputPorts
# define rtmGetNumOutputPorts(rtm)     ((rtm)->Sizes.numOports)
#endif

#ifndef rtmSetNumOutputPorts
# define rtmSetNumOutputPorts(rtm, val) ((rtm)->Sizes.numOports = (val))
#endif

#ifndef rtmGetNumPeriodicContStates
# define rtmGetNumPeriodicContStates(rtm) ((rtm)->Sizes.numPeriodicContStates)
#endif

#ifndef rtmSetNumPeriodicContStates
# define rtmSetNumPeriodicContStates(rtm, val) ((rtm)->Sizes.numPeriodicContStates = (val))
#endif

#ifndef rtmGetNumSFcnParams
# define rtmGetNumSFcnParams(rtm)      ((rtm)->Sizes.numSFcnPrms)
#endif

#ifndef rtmSetNumSFcnParams
# define rtmSetNumSFcnParams(rtm, val) ((rtm)->Sizes.numSFcnPrms = (val))
#endif

#ifndef rtmGetNumSFunctions
# define rtmGetNumSFunctions(rtm)      ((rtm)->Sizes.numSFcns)
#endif

#ifndef rtmSetNumSFunctions
# define rtmSetNumSFunctions(rtm, val) ((rtm)->Sizes.numSFcns = (val))
#endif

#ifndef rtmGetNumSampleTimes
# define rtmGetNumSampleTimes(rtm)     ((rtm)->Sizes.numSampTimes)
#endif

#ifndef rtmSetNumSampleTimes
# define rtmSetNumSampleTimes(rtm, val) ((rtm)->Sizes.numSampTimes = (val))
#endif

#ifndef rtmGetNumU
# define rtmGetNumU(rtm)               ((rtm)->Sizes.numU)
#endif

#ifndef rtmSetNumU
# define rtmSetNumU(rtm, val)          ((rtm)->Sizes.numU = (val))
#endif

#ifndef rtmGetNumY
# define rtmGetNumY(rtm)               ((rtm)->Sizes.numY)
#endif

#ifndef rtmSetNumY
# define rtmSetNumY(rtm, val)          ((rtm)->Sizes.numY = (val))
#endif

#ifndef rtmGetOdeF
# define rtmGetOdeF(rtm)               ()
#endif

#ifndef rtmSetOdeF
# define rtmSetOdeF(rtm, val)          ()
#endif

#ifndef rtmGetOdeY
# define rtmGetOdeY(rtm)               ()
#endif

#ifndef rtmSetOdeY
# define rtmSetOdeY(rtm, val)          ()
#endif

#ifndef rtmGetOffsetTimeArray
# define rtmGetOffsetTimeArray(rtm)    ((rtm)->Timing.offsetTimesArray)
#endif

#ifndef rtmSetOffsetTimeArray
# define rtmSetOffsetTimeArray(rtm, val) ((rtm)->Timing.offsetTimesArray = (val))
#endif

#ifndef rtmGetOffsetTimePtr
# define rtmGetOffsetTimePtr(rtm)      ((rtm)->Timing.offsetTimes)
#endif

#ifndef rtmSetOffsetTimePtr
# define rtmSetOffsetTimePtr(rtm, val) ((rtm)->Timing.offsetTimes = (val))
#endif

#ifndef rtmGetOptions
# define rtmGetOptions(rtm)            ((rtm)->Sizes.options)
#endif

#ifndef rtmSetOptions
# define rtmSetOptions(rtm, val)       ((rtm)->Sizes.options = (val))
#endif

#ifndef rtmGetParamIsMalloced
# define rtmGetParamIsMalloced(rtm)    ()
#endif

#ifndef rtmSetParamIsMalloced
# define rtmSetParamIsMalloced(rtm, val) ()
#endif

#ifndef rtmGetPath
# define rtmGetPath(rtm)               ((rtm)->path)
#endif

#ifndef rtmSetPath
# define rtmSetPath(rtm, val)          ((rtm)->path = (val))
#endif

#ifndef rtmGetPerTaskSampleHits
# define rtmGetPerTaskSampleHits(rtm)  ()
#endif

#ifndef rtmSetPerTaskSampleHits
# define rtmSetPerTaskSampleHits(rtm, val) ()
#endif

#ifndef rtmGetPerTaskSampleHitsArray
# define rtmGetPerTaskSampleHitsArray(rtm) ((rtm)->Timing.perTaskSampleHitsArray)
#endif

#ifndef rtmSetPerTaskSampleHitsArray
# define rtmSetPerTaskSampleHitsArray(rtm, val) ((rtm)->Timing.perTaskSampleHitsArray = (val))
#endif

#ifndef rtmGetPerTaskSampleHitsPtr
# define rtmGetPerTaskSampleHitsPtr(rtm) ((rtm)->Timing.perTaskSampleHits)
#endif

#ifndef rtmSetPerTaskSampleHitsPtr
# define rtmSetPerTaskSampleHitsPtr(rtm, val) ((rtm)->Timing.perTaskSampleHits = (val))
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

#ifndef rtmGetPrevZCSigState
# define rtmGetPrevZCSigState(rtm)     ((rtm)->ModelData.prevZCSigState)
#endif

#ifndef rtmSetPrevZCSigState
# define rtmSetPrevZCSigState(rtm, val) ((rtm)->ModelData.prevZCSigState = (val))
#endif

#ifndef rtmGetRTWExtModeInfo
# define rtmGetRTWExtModeInfo(rtm)     ((rtm)->extModeInfo)
#endif

#ifndef rtmSetRTWExtModeInfo
# define rtmSetRTWExtModeInfo(rtm, val) ((rtm)->extModeInfo = (val))
#endif

#ifndef rtmGetRTWGeneratedSFcn
# define rtmGetRTWGeneratedSFcn(rtm)   ((rtm)->Sizes.rtwGenSfcn)
#endif

#ifndef rtmSetRTWGeneratedSFcn
# define rtmSetRTWGeneratedSFcn(rtm, val) ((rtm)->Sizes.rtwGenSfcn = (val))
#endif

#ifndef rtmGetRTWLogInfo
# define rtmGetRTWLogInfo(rtm)         ((rtm)->rtwLogInfo)
#endif

#ifndef rtmSetRTWLogInfo
# define rtmSetRTWLogInfo(rtm, val)    ((rtm)->rtwLogInfo = (val))
#endif

#ifndef rtmGetRTWRTModelMethodsInfo
# define rtmGetRTWRTModelMethodsInfo(rtm) ()
#endif

#ifndef rtmSetRTWRTModelMethodsInfo
# define rtmSetRTWRTModelMethodsInfo(rtm, val) ()
#endif

#ifndef rtmGetRTWSfcnInfo
# define rtmGetRTWSfcnInfo(rtm)        ((rtm)->sfcnInfo)
#endif

#ifndef rtmSetRTWSfcnInfo
# define rtmSetRTWSfcnInfo(rtm, val)   ((rtm)->sfcnInfo = (val))
#endif

#ifndef rtmGetRTWSolverInfo
# define rtmGetRTWSolverInfo(rtm)      ((rtm)->solverInfo)
#endif

#ifndef rtmSetRTWSolverInfo
# define rtmSetRTWSolverInfo(rtm, val) ((rtm)->solverInfo = (val))
#endif

#ifndef rtmGetRTWSolverInfoPtr
# define rtmGetRTWSolverInfoPtr(rtm)   ((rtm)->solverInfoPtr)
#endif

#ifndef rtmSetRTWSolverInfoPtr
# define rtmSetRTWSolverInfoPtr(rtm, val) ((rtm)->solverInfoPtr = (val))
#endif

#ifndef rtmGetReservedForXPC
# define rtmGetReservedForXPC(rtm)     ((rtm)->SpecialInfo.xpcData)
#endif

#ifndef rtmSetReservedForXPC
# define rtmSetReservedForXPC(rtm, val) ((rtm)->SpecialInfo.xpcData = (val))
#endif

#ifndef rtmGetRootDWork
# define rtmGetRootDWork(rtm)          ((rtm)->ModelData.dwork)
#endif

#ifndef rtmSetRootDWork
# define rtmSetRootDWork(rtm, val)     ((rtm)->ModelData.dwork = (val))
#endif

#ifndef rtmGetSFunctions
# define rtmGetSFunctions(rtm)         ((rtm)->childSfunctions)
#endif

#ifndef rtmSetSFunctions
# define rtmSetSFunctions(rtm, val)    ((rtm)->childSfunctions = (val))
#endif

#ifndef rtmGetSampleHitArray
# define rtmGetSampleHitArray(rtm)     ((rtm)->Timing.sampleHitArray)
#endif

#ifndef rtmSetSampleHitArray
# define rtmSetSampleHitArray(rtm, val) ((rtm)->Timing.sampleHitArray = (val))
#endif

#ifndef rtmGetSampleHitPtr
# define rtmGetSampleHitPtr(rtm)       ((rtm)->Timing.sampleHits)
#endif

#ifndef rtmSetSampleHitPtr
# define rtmSetSampleHitPtr(rtm, val)  ((rtm)->Timing.sampleHits = (val))
#endif

#ifndef rtmGetSampleTimeArray
# define rtmGetSampleTimeArray(rtm)    ((rtm)->Timing.sampleTimesArray)
#endif

#ifndef rtmSetSampleTimeArray
# define rtmSetSampleTimeArray(rtm, val) ((rtm)->Timing.sampleTimesArray = (val))
#endif

#ifndef rtmGetSampleTimePtr
# define rtmGetSampleTimePtr(rtm)      ((rtm)->Timing.sampleTimes)
#endif

#ifndef rtmSetSampleTimePtr
# define rtmSetSampleTimePtr(rtm, val) ((rtm)->Timing.sampleTimes = (val))
#endif

#ifndef rtmGetSampleTimeTaskIDArray
# define rtmGetSampleTimeTaskIDArray(rtm) ((rtm)->Timing.sampleTimeTaskIDArray)
#endif

#ifndef rtmSetSampleTimeTaskIDArray
# define rtmSetSampleTimeTaskIDArray(rtm, val) ((rtm)->Timing.sampleTimeTaskIDArray = (val))
#endif

#ifndef rtmGetSampleTimeTaskIDPtr
# define rtmGetSampleTimeTaskIDPtr(rtm) ((rtm)->Timing.sampleTimeTaskIDPtr)
#endif

#ifndef rtmSetSampleTimeTaskIDPtr
# define rtmSetSampleTimeTaskIDPtr(rtm, val) ((rtm)->Timing.sampleTimeTaskIDPtr = (val))
#endif

#ifndef rtmGetSimMode
# define rtmGetSimMode(rtm)            ((rtm)->simMode)
#endif

#ifndef rtmSetSimMode
# define rtmSetSimMode(rtm, val)       ((rtm)->simMode = (val))
#endif

#ifndef rtmGetSimTimeStep
# define rtmGetSimTimeStep(rtm)        ((rtm)->Timing.simTimeStep)
#endif

#ifndef rtmSetSimTimeStep
# define rtmSetSimTimeStep(rtm, val)   ((rtm)->Timing.simTimeStep = (val))
#endif

#ifndef rtmGetStartTime
# define rtmGetStartTime(rtm)          ((rtm)->Timing.tStart)
#endif

#ifndef rtmSetStartTime
# define rtmSetStartTime(rtm, val)     ((rtm)->Timing.tStart = (val))
#endif

#ifndef rtmGetStepSize
# define rtmGetStepSize(rtm)           ((rtm)->Timing.stepSize)
#endif

#ifndef rtmSetStepSize
# define rtmSetStepSize(rtm, val)      ((rtm)->Timing.stepSize = (val))
#endif

#ifndef rtmGetStopRequestedFlag
# define rtmGetStopRequestedFlag(rtm)  ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequestedFlag
# define rtmSetStopRequestedFlag(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetTaskCounters
# define rtmGetTaskCounters(rtm)       ()
#endif

#ifndef rtmSetTaskCounters
# define rtmSetTaskCounters(rtm, val)  ()
#endif

#ifndef rtmGetTaskTimeArray
# define rtmGetTaskTimeArray(rtm)      ((rtm)->Timing.tArray)
#endif

#ifndef rtmSetTaskTimeArray
# define rtmSetTaskTimeArray(rtm, val) ((rtm)->Timing.tArray = (val))
#endif

#ifndef rtmGetTimePtr
# define rtmGetTimePtr(rtm)            ((rtm)->Timing.t)
#endif

#ifndef rtmSetTimePtr
# define rtmSetTimePtr(rtm, val)       ((rtm)->Timing.t = (val))
#endif

#ifndef rtmGetTimingData
# define rtmGetTimingData(rtm)         ((rtm)->Timing.timingData)
#endif

#ifndef rtmSetTimingData
# define rtmSetTimingData(rtm, val)    ((rtm)->Timing.timingData = (val))
#endif

#ifndef rtmGetU
# define rtmGetU(rtm)                  ((rtm)->ModelData.inputs)
#endif

#ifndef rtmSetU
# define rtmSetU(rtm, val)             ((rtm)->ModelData.inputs = (val))
#endif

#ifndef rtmGetVarNextHitTimesListPtr
# define rtmGetVarNextHitTimesListPtr(rtm) ((rtm)->Timing.varNextHitTimesList)
#endif

#ifndef rtmSetVarNextHitTimesListPtr
# define rtmSetVarNextHitTimesListPtr(rtm, val) ((rtm)->Timing.varNextHitTimesList = (val))
#endif

#ifndef rtmGetY
# define rtmGetY(rtm)                  ((rtm)->ModelData.outputs)
#endif

#ifndef rtmSetY
# define rtmSetY(rtm, val)             ((rtm)->ModelData.outputs = (val))
#endif

#ifndef rtmGetZCCacheNeedsReset
# define rtmGetZCCacheNeedsReset(rtm)  ((rtm)->ModelData.zCCacheNeedsReset)
#endif

#ifndef rtmSetZCCacheNeedsReset
# define rtmSetZCCacheNeedsReset(rtm, val) ((rtm)->ModelData.zCCacheNeedsReset = (val))
#endif

#ifndef rtmGetZCSignalValues
# define rtmGetZCSignalValues(rtm)     ((rtm)->ModelData.zcSignalValues)
#endif

#ifndef rtmSetZCSignalValues
# define rtmSetZCSignalValues(rtm, val) ((rtm)->ModelData.zcSignalValues = (val))
#endif

#ifndef rtmGet_TimeOfLastOutput
# define rtmGet_TimeOfLastOutput(rtm)  ((rtm)->Timing.timeOfLastOutput)
#endif

#ifndef rtmSet_TimeOfLastOutput
# define rtmSet_TimeOfLastOutput(rtm, val) ((rtm)->Timing.timeOfLastOutput = (val))
#endif

#ifndef rtmGetdX
# define rtmGetdX(rtm)                 ((rtm)->ModelData.derivs)
#endif

#ifndef rtmSetdX
# define rtmSetdX(rtm, val)            ((rtm)->ModelData.derivs = (val))
#endif

#ifndef rtmGetChecksumVal
# define rtmGetChecksumVal(rtm, idx)   ((rtm)->Sizes.checksums[idx])
#endif

#ifndef rtmSetChecksumVal
# define rtmSetChecksumVal(rtm, idx, val) ((rtm)->Sizes.checksums[idx] = (val))
#endif

#ifndef rtmGetDWork
# define rtmGetDWork(rtm, idx)         ((rtm)->ModelData.dwork[idx])
#endif

#ifndef rtmSetDWork
# define rtmSetDWork(rtm, idx, val)    ((rtm)->ModelData.dwork[idx] = (val))
#endif

#ifndef rtmGetOffsetTime
# define rtmGetOffsetTime(rtm, idx)    ((rtm)->Timing.offsetTimes[idx])
#endif

#ifndef rtmSetOffsetTime
# define rtmSetOffsetTime(rtm, idx, val) ((rtm)->Timing.offsetTimes[idx] = (val))
#endif

#ifndef rtmGetSFunction
# define rtmGetSFunction(rtm, idx)     ((rtm)->childSfunctions[idx])
#endif

#ifndef rtmSetSFunction
# define rtmSetSFunction(rtm, idx, val) ((rtm)->childSfunctions[idx] = (val))
#endif

#ifndef rtmGetSampleTime
# define rtmGetSampleTime(rtm, idx)    ((rtm)->Timing.sampleTimes[idx])
#endif

#ifndef rtmSetSampleTime
# define rtmSetSampleTime(rtm, idx, val) ((rtm)->Timing.sampleTimes[idx] = (val))
#endif

#ifndef rtmGetSampleTimeTaskID
# define rtmGetSampleTimeTaskID(rtm, idx) ((rtm)->Timing.sampleTimeTaskIDPtr[idx])
#endif

#ifndef rtmSetSampleTimeTaskID
# define rtmSetSampleTimeTaskID(rtm, idx, val) ((rtm)->Timing.sampleTimeTaskIDPtr[idx] = (val))
#endif

#ifndef rtmGetVarNextHitTimeList
# define rtmGetVarNextHitTimeList(rtm, idx) ((rtm)->Timing.varNextHitTimesList[idx])
#endif

#ifndef rtmSetVarNextHitTimeList
# define rtmSetVarNextHitTimeList(rtm, idx, val) ((rtm)->Timing.varNextHitTimesList[idx] = (val))
#endif

#ifndef rtmIsContinuousTask
# define rtmIsContinuousTask(rtm, tid) ((tid) == 0)
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmIsMajorTimeStep
# define rtmIsMajorTimeStep(rtm)       (((rtm)->Timing.simTimeStep) == MAJOR_TIME_STEP)
#endif

#ifndef rtmIsMinorTimeStep
# define rtmIsMinorTimeStep(rtm)       (((rtm)->Timing.simTimeStep) == MINOR_TIME_STEP)
#endif

#ifndef rtmIsSampleHit
# define rtmIsSampleHit(rtm, sti, tid) ((rtmIsMajorTimeStep((rtm)) && (rtm)->Timing.sampleHits[(rtm)->Timing.sampleTimeTaskIDPtr[sti]]))
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

#ifndef rtmSetT
# define rtmSetT(rtm, val)                                       /* Do Nothing */
#endif

#ifndef rtmGetTFinal
# define rtmGetTFinal(rtm)             ((rtm)->Timing.tFinal)
#endif

#ifndef rtmSetTFinal
# define rtmSetTFinal(rtm, val)        ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmGetTPtr
# define rtmGetTPtr(rtm)               ((rtm)->Timing.t)
#endif

#ifndef rtmSetTPtr
# define rtmSetTPtr(rtm, val)          ((rtm)->Timing.t = (val))
#endif

#ifndef rtmGetTStart
# define rtmGetTStart(rtm)             ((rtm)->Timing.tStart)
#endif

#ifndef rtmSetTStart
# define rtmSetTStart(rtm, val)        ((rtm)->Timing.tStart = (val))
#endif

#ifndef rtmGetTaskTime
# define rtmGetTaskTime(rtm, sti)      (rtmGetTPtr((rtm))[(rtm)->Timing.sampleTimeTaskIDPtr[sti]])
#endif

#ifndef rtmSetTaskTime
# define rtmSetTaskTime(rtm, sti, val) (rtmGetTPtr((rtm))[sti] = (val))
#endif

#ifndef rtmGetTimeOfLastOutput
# define rtmGetTimeOfLastOutput(rtm)   ((rtm)->Timing.timeOfLastOutput)
#endif

#ifdef rtmGetRTWSolverInfo
#undef rtmGetRTWSolverInfo
#endif

#define rtmGetRTWSolverInfo(rtm)       &((rtm)->solverInfo)

/* Definition for use in the target main file */
#define b747cl_rtModel                 RT_MODEL_b747cl_T

/* Block signals (auto storage) */
typedef struct {
  real_T DiscreteTimeIntegrator[12];   /* '<S9>/Discrete-Time Integrator' */
  real_T Output[40];                   /* '<S3>/Output' */
  real_T u[12];                        /* '<S9>/ *1' */
  real_T u_a[12];                      /* '<S2>/ *1' */
  real_T g;                            /* '<S18>/g' */
  real_T T;                            /* '<S18>/T' */
  real_T ps;                           /* '<S18>/ps' */
  real_T rho;                          /* '<S18>/rho' */
  real_T mu;                           /* '<S18>/mu' */
  real_T a;                            /* '<S15>/a' */
  real_T M;                            /* '<S15>/M' */
  real_T qdyn;                         /* '<S15>/qdyn' */
  real_T cosalpha;                     /* '<S23>/cos alpha' */
  real_T Fcn;                          /* '<S19>/Fcn' */
  real_T Fcn1;                         /* '<S19>/Fcn1' */
  real_T Fcn2;                         /* '<S19>/Fcn2' */
  real_T Fcn3;                         /* '<S19>/Fcn3' */
  real_T pb2V;                         /* '<S20>/pb//2V' */
  real_T qc2V;                         /* '<S20>/qc//2V' */
  real_T rb2V;                         /* '<S20>/rb//2V' */
  real_T delay;                        /* '<S4>/ delay' */
  real_T DiscreteStateSpace;           /* '<S36>/Discrete State-Space' */
  real_T RateLimiter1;                 /* '<S4>/Rate Limiter1' */
  real_T delay_h;                      /* '<S4>/delay' */
  real_T DiscreteStateSpace_f;         /* '<S37>/Discrete State-Space' */
  real_T RateLimiter2;                 /* '<S4>/Rate Limiter2' */
  real_T delay_m;                      /* '<S4>/delay ' */
  real_T DiscreteStateSpace_a;         /* '<S38>/Discrete State-Space' */
  real_T RateLimiter3;                 /* '<S4>/Rate Limiter3' */
  real_T Product;                      /* '<S19>/Product' */
  real_T sinalpha;                     /* '<S23>/sin alpha' */
  real_T delay_o;                      /* '<S4>/ delay ' */
  real_T Saturation3;                  /* '<S4>/Saturation3' */
  real_T Xgr;                          /* '<S12>/Xgr' */
  real_T Ygr;                          /* '<S12>/Ygr' */
  real_T Zgr;                          /* '<S12>/Zgr' */
  real_T gain[3];                      /* '<S11>/gain' */
  real_T cosalpha_k;                   /* '<S13>/cos alpha' */
  real_T sinalpha_f;                   /* '<S13>/sin alpha' */
  real_T cosbeta;                      /* '<S13>/cos beta' */
  real_T sinbeta;                      /* '<S13>/sin beta' */
  real_T tanbeta;                      /* '<S13>/tan beta' */
  real_T sinpsi;                       /* '<S13>/sin psi' */
  real_T cospsi;                       /* '<S13>/cos psi' */
  real_T sintheta;                     /* '<S13>/sin theta' */
  real_T costheta;                     /* '<S13>/cos theta' */
  real_T sinphi;                       /* '<S13>/sin phi' */
  real_T cosphi;                       /* '<S13>/cos phi' */
  real_T Fcn_a;                        /* '<S29>/Fcn' */
  real_T Fcn1_m;                       /* '<S29>/Fcn1' */
  real_T Fcn2_o;                       /* '<S29>/Fcn2' */
  real_T Fcn3_e;                       /* '<S29>/Fcn3' */
  real_T Fcn4;                         /* '<S29>/Fcn4' */
  real_T Fcn5;                         /* '<S29>/Fcn5' */
  real_T Sum[3];                       /* '<S9>/Sum' */
  real_T xfix[12];                     /* '<S9>/xfix' */
  real_T Sum5;                         /* '<S1>/Sum5' */
  real_T wind2;                        /* '<S1>/wind2' */
  real_T wind4;                        /* '<S1>/wind4' */
  real_T wind5;                        /* '<S1>/wind5' */
  real_T Switch[4];                    /* '<S1>/Switch' */
  real_T Clock;                        /* '<Root>/Clock' */
} B_b747cl_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T DiscreteTimeIntegrator_DSTATE[12];/* '<S9>/Discrete-Time Integrator' */
  real_T DiscreteStateSpace_DSTATE;    /* '<S36>/Discrete State-Space' */
  real_T DiscreteStateSpace_DSTATE_g;  /* '<S37>/Discrete State-Space' */
  real_T DiscreteStateSpace_DSTATE_m;  /* '<S38>/Discrete State-Space' */
  real_T DiscreteStateSpace_DSTATE_d;  /* '<S35>/Discrete State-Space' */
  real_T NextOutput[40];               /* '<S3>/White Noise' */
  real_T PrevY;                        /* '<S4>/Rate Limiter1' */
  real_T LastMajorTime;                /* '<S4>/Rate Limiter1' */
  real_T PrevY_o;                      /* '<S4>/Rate Limiter2' */
  real_T LastMajorTime_l;              /* '<S4>/Rate Limiter2' */
  real_T PrevY_e;                      /* '<S4>/Rate Limiter3' */
  real_T LastMajorTime_e;              /* '<S4>/Rate Limiter3' */
  struct {
    real_T modelTStart;
    real_T TUbufferArea[2048];
  } delay_RWORK;                       /* '<S4>/ delay' */

  struct {
    real_T modelTStart;
    real_T TUbufferArea[2048];
  } delay_RWORK_g;                     /* '<S4>/delay' */

  struct {
    real_T modelTStart;
    real_T TUbufferArea[2048];
  } delay_RWORK_o;                     /* '<S4>/delay ' */

  struct {
    real_T modelTStart;
    real_T TUbufferArea[2048];
  } delay_RWORK_p;                     /* '<S4>/ delay ' */

  struct {
    void *LoggedData;
  } AircraftStates_PWORK;              /* '<Root>/Aircraft States' */

  struct {
    void *LoggedData;
  } Airspeed_PWORK;                    /* '<Root>/Airspeed' */

  struct {
    void *LoggedData;
  } Alpha_PWORK;                       /* '<Root>/Alpha' */

  struct {
    void *LoggedData;
  } Altitude_PWORK;                    /* '<Root>/Altitude' */

  struct {
    void *LoggedData;
  } Beta_PWORK;                        /* '<Root>/Beta' */

  struct {
    void *LoggedData;
  } Phi_PWORK;                         /* '<Root>/Phi' */

  struct {
    void *LoggedData;
  } Theta_PWORK;                       /* '<Root>/Theta' */

  struct {
    void *LoggedData;
  } airspeed_PWORK;                    /* '<Root>/airspeed' */

  struct {
    void *LoggedData;
  } alititude_PWORK;                   /* '<Root>/alititude' */

  struct {
    void *LoggedData;
  } p_PWORK;                           /* '<Root>/p' */

  struct {
    void *LoggedData;
  } psi_PWORK;                         /* '<Root>/psi' */

  struct {
    void *LoggedData;
  } q_PWORK;                           /* '<Root>/q' */

  struct {
    void *LoggedData;
  } r_PWORK;                           /* '<Root>/r' */

  struct {
    void *LoggedData;
  } theta_PWORK;                       /* '<Root>/theta' */

  struct {
    void *LoggedData;
  } x_e_PWORK;                         /* '<Root>/x_e' */

  struct {
    void *LoggedData;
  } y_e_PWORK;                         /* '<Root>/y_e' */

  struct {
    void *TUbufferPtrs[2];
  } delay_PWORK;                       /* '<S4>/ delay' */

  struct {
    void *TUbufferPtrs[2];
  } delay_PWORK_f;                     /* '<S4>/delay' */

  struct {
    void *TUbufferPtrs[2];
  } delay_PWORK_e;                     /* '<S4>/delay ' */

  struct {
    void *TUbufferPtrs[2];
  } delay_PWORK_a;                     /* '<S4>/ delay ' */

  struct {
    void *LoggedData;
  } ToWorkspace4_PWORK;                /* '<Root>/To Workspace4' */

  uint32_T RandSeed[40];               /* '<S3>/White Noise' */
  struct {
    int_T Tail;
    int_T Head;
    int_T Last;
    int_T CircularBufSize;
  } delay_IWORK;                       /* '<S4>/ delay' */

  struct {
    int_T Tail;
    int_T Head;
    int_T Last;
    int_T CircularBufSize;
  } delay_IWORK_j;                     /* '<S4>/delay' */

  struct {
    int_T Tail;
    int_T Head;
    int_T Last;
    int_T CircularBufSize;
  } delay_IWORK_g;                     /* '<S4>/delay ' */

  struct {
    int_T Tail;
    int_T Head;
    int_T Last;
    int_T CircularBufSize;
  } delay_IWORK_i;                     /* '<S4>/ delay ' */
} DW_b747cl_T;

/* Backward compatible GRT Identifiers */
#define rtB                            b747cl_B
#define BlockIO                        B_b747cl_T
#define rtP                            b747cl_P
#define Parameters                     P_b747cl_T
#define rtDWork                        b747cl_DW
#define D_Work                         DW_b747cl_T

/* Parameters (auto storage) */
struct P_b747cl_T_ {
  real_T AM_M[114];                    /* Mask Parameter: AM_M
                                        * Referenced by: '<S22>/Matrix Gain'
                                        */
  real_T GM2_M[27];                    /* Mask Parameter: GM2_M
                                        * Referenced by: '<S31>/Matrix Gain'
                                        */
  real_T Klg_M[10];                    /* Mask Parameter: Klg_M
                                        * Referenced by: '<S5>/Matrix Gain'
                                        */
  real_T Klt_M[8];                     /* Mask Parameter: Klt_M
                                        * Referenced by: '<S6>/Matrix Gain'
                                        */
  real_T fmdims_S;                     /* Mask Parameter: fmdims_S
                                        * Referenced by: '<S21>/S'
                                        */
  real_T fmdims_b;                     /* Mask Parameter: fmdims_b
                                        * Referenced by:
                                        *   '<S21>/b'
                                        *   '<S21>/b '
                                        */
  real_T fmdims_cbar;                  /* Mask Parameter: fmdims_cbar
                                        * Referenced by: '<S21>/cbar'
                                        */
  real_T ad3_cbar;                     /* Mask Parameter: ad3_cbar
                                        * Referenced by: '<S17>/cbar'
                                        */
  real_T fw_m;                         /* Mask Parameter: fw_m
                                        * Referenced by: '<S11>/gain'
                                        */
  real_T DTB747_x0[12];                /* Mask Parameter: DTB747_x0
                                        * Referenced by: '<S9>/Discrete-Time Integrator'
                                        */
  real_T IIR2_x0;                      /* Mask Parameter: IIR2_x0
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  real_T IIR3_x0;                      /* Mask Parameter: IIR3_x0
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  real_T IIR4_x0;                      /* Mask Parameter: IIR4_x0
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  real_T IIR1_x0;                      /* Mask Parameter: IIR1_x0
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  real_T DiscreteTimeIntegrator_gainval;/* Computed Parameter: DiscreteTimeIntegrator_gainval
                                         * Referenced by: '<S9>/Discrete-Time Integrator'
                                         */
  real_T y0125812_Value[5];            /* Expression: [150.148  0.0464959 0 0.0464959 61]
                                        * Referenced by: '<S1>/y0([1 2 5 8 12])'
                                        */
  real_T WhiteNoise_Mean;              /* Expression: 0
                                        * Referenced by: '<S3>/White Noise'
                                        */
  real_T WhiteNoise_StdDev;            /* Computed Parameter: WhiteNoise_StdDev
                                        * Referenced by: '<S3>/White Noise'
                                        */
  real_T WhiteNoise_Seed[40];          /* Expression: seed
                                        * Referenced by: '<S3>/White Noise'
                                        */
  real_T Output_Gain[40];              /* Expression: [sqrt(Cov)]/[sqrt(Ts)]
                                        * Referenced by: '<S3>/Output'
                                        */
  real_T u_Gain;                       /* Expression: 1
                                        * Referenced by: '<S9>/ *1'
                                        */
  real_T u_Gain_o;                     /* Expression: 1
                                        * Referenced by: '<S2>/ *1'
                                        */
  real_T const_Value;                  /* Expression: 1
                                        * Referenced by: '<S19>/const'
                                        */
  real_T delay_Delay;                  /* Expression: 0.03
                                        * Referenced by: '<S4>/ delay'
                                        */
  real_T delay_InitOutput;             /* Expression: -1.71374e-004
                                        * Referenced by: '<S4>/ delay'
                                        */
  real_T DiscreteStateSpace_A;         /* Computed Parameter: DiscreteStateSpace_A
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_B;         /* Computed Parameter: DiscreteStateSpace_B
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_C;         /* Computed Parameter: DiscreteStateSpace_C
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_D;         /* Computed Parameter: DiscreteStateSpace_D
                                        * Referenced by: '<S36>/Discrete State-Space'
                                        */
  real_T RateLimiter1_RisingLim;       /* Expression: 100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter1'
                                        */
  real_T RateLimiter1_FallingLim;      /* Expression: -100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter1'
                                        */
  real_T Saturation4_UpperSat;         /* Expression: 20*pi/180
                                        * Referenced by: '<S4>/Saturation4'
                                        */
  real_T Saturation4_LowerSat;         /* Expression: -20*pi/180
                                        * Referenced by: '<S4>/Saturation4'
                                        */
  real_T stabs_Value;                  /* Expression: -0.0171382
                                        * Referenced by: '<Root>/stabs'
                                        */
  real_T delay_Delay_m;                /* Expression: 0.03
                                        * Referenced by: '<S4>/delay'
                                        */
  real_T delay_InitOutput_b;           /* Expression: 0
                                        * Referenced by: '<S4>/delay'
                                        */
  real_T DiscreteStateSpace_A_d;       /* Computed Parameter: DiscreteStateSpace_A_d
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_B_h;       /* Computed Parameter: DiscreteStateSpace_B_h
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_C_l;       /* Computed Parameter: DiscreteStateSpace_C_l
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_D_m;       /* Computed Parameter: DiscreteStateSpace_D_m
                                        * Referenced by: '<S37>/Discrete State-Space'
                                        */
  real_T RateLimiter2_RisingLim;       /* Expression: 100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter2'
                                        */
  real_T RateLimiter2_FallingLim;      /* Expression: -100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter2'
                                        */
  real_T Saturation1_UpperSat;         /* Expression: 20*pi/180
                                        * Referenced by: '<S4>/Saturation1'
                                        */
  real_T Saturation1_LowerSat;         /* Expression: -20*pi/180
                                        * Referenced by: '<S4>/Saturation1'
                                        */
  real_T delay_Delay_c;                /* Expression: 0.03
                                        * Referenced by: '<S4>/delay '
                                        */
  real_T delay_InitOutput_ba;          /* Expression: 0
                                        * Referenced by: '<S4>/delay '
                                        */
  real_T DiscreteStateSpace_A_b;       /* Computed Parameter: DiscreteStateSpace_A_b
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_B_i;       /* Computed Parameter: DiscreteStateSpace_B_i
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_C_g;       /* Computed Parameter: DiscreteStateSpace_C_g
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_D_a;       /* Computed Parameter: DiscreteStateSpace_D_a
                                        * Referenced by: '<S38>/Discrete State-Space'
                                        */
  real_T RateLimiter3_RisingLim;       /* Expression: 100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter3'
                                        */
  real_T RateLimiter3_FallingLim;      /* Expression: -100*pi/180
                                        * Referenced by: '<S4>/Rate Limiter3'
                                        */
  real_T Saturation2_UpperSat;         /* Expression: 20*pi/180
                                        * Referenced by: '<S4>/Saturation2'
                                        */
  real_T Saturation2_LowerSat;         /* Expression: -20*pi/180
                                        * Referenced by: '<S4>/Saturation2'
                                        */
  real_T const1_Value;                 /* Expression: 0
                                        * Referenced by: '<S19>/const1'
                                        */
  real_T MatrixGain_X0;                /* Expression: 0
                                        * Referenced by: '<S22>/Matrix Gain'
                                        */
  real_T delay_Delay_k;                /* Expression: 0.03
                                        * Referenced by: '<S4>/ delay '
                                        */
  real_T delay_InitOutput_a;           /* Expression: 0.490154
                                        * Referenced by: '<S4>/ delay '
                                        */
  real_T DiscreteStateSpace_A_n;       /* Computed Parameter: DiscreteStateSpace_A_n
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_B_e;       /* Computed Parameter: DiscreteStateSpace_B_e
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_C_g5;      /* Computed Parameter: DiscreteStateSpace_C_g5
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  real_T DiscreteStateSpace_D_o;       /* Computed Parameter: DiscreteStateSpace_D_o
                                        * Referenced by: '<S35>/Discrete State-Space'
                                        */
  real_T ForceN_Gain;                  /* Expression: 368160
                                        * Referenced by: '<S4>/Force [N]'
                                        */
  real_T Saturation3_UpperSat;         /* Expression: 245440
                                        * Referenced by: '<S4>/Saturation3'
                                        */
  real_T Saturation3_LowerSat;         /* Expression: 0
                                        * Referenced by: '<S4>/Saturation3'
                                        */
  real_T Wind_Value[6];                /* Expression: zeros(6,1)
                                        * Referenced by: '<Root>/Wind'
                                        */
  real_T MatrixGain_X0_l;              /* Expression: 0
                                        * Referenced by: '<S31>/Matrix Gain'
                                        */
  real_T u_Gain_m;                     /* Expression: 1
                                        * Referenced by: '<S9>/+1'
                                        */
  real_T xfix_Gain;                    /* Expression: xfix
                                        * Referenced by: '<S9>/xfix'
                                        */
  real_T u_Gain_b;                     /* Expression: 1
                                        * Referenced by: '<S2>/+1'
                                        */
  real_T MatrixGain_X0_f;              /* Expression: 0
                                        * Referenced by: '<S5>/Matrix Gain'
                                        */
  real_T y03469_Value[4];              /* Expression: [0.0 0.0 0.0 0.0]
                                        * Referenced by: '<S1>/y0([3 4 6 9])'
                                        */
  real_T MatrixGain_X0_a;              /* Expression: 0
                                        * Referenced by: '<S6>/Matrix Gain'
                                        */
  real_T c4_Value[2];                  /* Expression: [0.490154 -1.71374e-004]
                                        * Referenced by: '<S1>/c4'
                                        */
  real_T c1_Value[2];                  /* Expression: [0 0]
                                        * Referenced by: '<S1>/c1'
                                        */
  real_T c5_Value;                     /* Expression: 2/3*0.490154
                                        * Referenced by: '<S1>/c5'
                                        */
  real_T The747PilotWasHere_Value;     /* Expression: 0.75
                                        * Referenced by: '<Root>/The 747 Pilot Was Here'
                                        */
  real_T wind6_Gain;                   /* Expression: -2/3*188211.4522/368160
                                        * Referenced by: '<S1>/wind6'
                                        */
  real_T wind2_Gain;                   /* Expression: -1/3
                                        * Referenced by: '<S1>/wind2'
                                        */
  real_T wind4_Gain;                   /* Expression: -1/3
                                        * Referenced by: '<S1>/wind4'
                                        */
  real_T wind5_Gain;                   /* Expression: -1/3
                                        * Referenced by: '<S1>/wind5'
                                        */
  uint8_T Switch_CurrentSetting;       /* Computed Parameter: Switch_CurrentSetting
                                        * Referenced by: '<S1>/Switch'
                                        */
};

/* Real-time Model Data Structure */
struct tag_RTM_b747cl_T {
  const char_T *path;
  const char_T *modelName;
  struct SimStruct_tag * *childSfunctions;
  const char_T *errorStatus;
  SS_SimMode simMode;
  RTWLogInfo *rtwLogInfo;
  RTWExtModeInfo *extModeInfo;
  RTWSolverInfo solverInfo;
  RTWSolverInfo *solverInfoPtr;
  void *sfcnInfo;

  /*
   * ModelData:
   * The following substructure contains information regarding
   * the data used in the model.
   */
  struct {
    void *blockIO;
    const void *constBlockIO;
    void *defaultParam;
    ZCSigState *prevZCSigState;
    real_T *contStates;
    int_T *periodicContStateIndices;
    real_T *periodicContStateRanges;
    real_T *derivs;
    void *zcSignalValues;
    void *inputs;
    void *outputs;
    boolean_T *contStateDisabled;
    boolean_T zCCacheNeedsReset;
    boolean_T derivCacheNeedsReset;
    boolean_T blkStateChange;
    void *dwork;
  } ModelData;

  /*
   * Sizes:
   * The following substructure contains sizes information
   * for many of the model attributes such as inputs, outputs,
   * dwork, sample times, etc.
   */
  struct {
    uint32_T checksums[4];
    uint32_T options;
    int_T numContStates;
    int_T numPeriodicContStates;
    int_T numU;
    int_T numY;
    int_T numSampTimes;
    int_T numBlocks;
    int_T numBlockIO;
    int_T numBlockPrms;
    int_T numDwork;
    int_T numSFcnPrms;
    int_T numSFcns;
    int_T numIports;
    int_T numOports;
    int_T numNonSampZCs;
    int_T sysDirFeedThru;
    int_T rtwGenSfcn;
  } Sizes;

  /*
   * SpecialInfo:
   * The following substructure contains special information
   * related to other components that are dependent on RTW.
   */
  struct {
    const void *mappingInfo;
    void *xpcData;
  } SpecialInfo;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    time_T stepSize;
    uint32_T clockTick0;
    uint32_T clockTickH0;
    time_T stepSize0;
    uint32_T clockTick1;
    uint32_T clockTickH1;
    time_T stepSize1;
    time_T tStart;
    time_T tFinal;
    time_T timeOfLastOutput;
    void *timingData;
    real_T *varNextHitTimesList;
    SimTimeStep simTimeStep;
    boolean_T stopRequestedFlag;
    time_T *sampleTimes;
    time_T *offsetTimes;
    int_T *sampleTimeTaskIDPtr;
    int_T *sampleHits;
    int_T *perTaskSampleHits;
    time_T *t;
    time_T sampleTimesArray[2];
    time_T offsetTimesArray[2];
    int_T sampleTimeTaskIDArray[2];
    int_T sampleHitArray[2];
    int_T perTaskSampleHitsArray[4];
    time_T tArray[2];
  } Timing;
};

/* Block parameters (auto storage) */
extern P_b747cl_T b747cl_P;

/* Block signals (auto storage) */
extern B_b747cl_T b747cl_B;

/* Block states (auto storage) */
extern DW_b747cl_T b747cl_DW;

/*====================*
 * External functions *
 *====================*/
extern b747cl_rtModel *b747cl(void);
extern void MdlInitializeSizes(void);
extern void MdlInitializeSampleTimes(void);
extern void MdlInitialize(void);
extern void MdlStart(void);
extern void MdlOutputs(int_T tid);
extern void MdlUpdate(int_T tid);
extern void MdlTerminate(void);

/* Real-time Model object */
extern RT_MODEL_b747cl_T *const b747cl_M;

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
 * '<Root>' : 'b747cl'
 * '<S1>'   : 'b747cl/Autopilot'
 * '<S2>'   : 'b747cl/DT-B747'
 * '<S3>'   : 'b747cl/Noise'
 * '<S4>'   : 'b747cl/elevator'
 * '<S5>'   : 'b747cl/Autopilot/Klg '
 * '<S6>'   : 'b747cl/Autopilot/Klt '
 * '<S7>'   : 'b747cl/DT-B747/ad'
 * '<S8>'   : 'b747cl/DT-B747/ae'
 * '<S9>'   : 'b747cl/DT-B747/eqm'
 * '<S10>'  : 'b747cl/DT-B747/fms'
 * '<S11>'  : 'b747cl/DT-B747/fw'
 * '<S12>'  : 'b747cl/DT-B747/gr'
 * '<S13>'  : 'b747cl/DT-B747/hlp'
 * '<S14>'  : 'b747cl/DT-B747/out'
 * '<S15>'  : 'b747cl/DT-B747/ad/ad1'
 * '<S16>'  : 'b747cl/DT-B747/ad/ad2'
 * '<S17>'  : 'b747cl/DT-B747/ad/ad3'
 * '<S18>'  : 'b747cl/DT-B747/ad/atm'
 * '<S19>'  : 'b747cl/DT-B747/ae/aero'
 * '<S20>'  : 'b747cl/DT-B747/ae/dl'
 * '<S21>'  : 'b747cl/DT-B747/ae/fmdims'
 * '<S22>'  : 'b747cl/DT-B747/ae/aero/AM '
 * '<S23>'  : 'b747cl/DT-B747/ae/aero/Stability 2 Body Axis'
 * '<S24>'  : 'b747cl/DT-B747/eqm/ODE'
 * '<S25>'  : 'b747cl/DT-B747/eqm/uvw'
 * '<S26>'  : 'b747cl/DT-B747/eqm/xdotcorr'
 * '<S27>'  : 'b747cl/DT-B747/eqm/ODE/Eulerdot'
 * '<S28>'  : 'b747cl/DT-B747/eqm/ODE/Vabdot'
 * '<S29>'  : 'b747cl/DT-B747/eqm/ODE/pqrdot'
 * '<S30>'  : 'b747cl/DT-B747/eqm/ODE/xyHdot'
 * '<S31>'  : 'b747cl/DT-B747/eqm/ODE/pqrdot/GM2 '
 * '<S32>'  : 'b747cl/DT-B747/out/accel'
 * '<S33>'  : 'b747cl/DT-B747/out/fp'
 * '<S34>'  : 'b747cl/DT-B747/out/uvwdot'
 * '<S35>'  : 'b747cl/elevator/IIR1'
 * '<S36>'  : 'b747cl/elevator/IIR2'
 * '<S37>'  : 'b747cl/elevator/IIR3'
 * '<S38>'  : 'b747cl/elevator/IIR4'
 */
#endif                                 /* RTW_HEADER_b747cl_h_ */
