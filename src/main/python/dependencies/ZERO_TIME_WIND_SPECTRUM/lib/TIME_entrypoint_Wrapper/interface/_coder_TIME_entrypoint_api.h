//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// _coder_TIME_entrypoint_api.h
//
// Code generation for function 'TIME_entrypoint'
//

#ifndef _CODER_TIME_ENTRYPOINT_API_H
#define _CODER_TIME_ENTRYPOINT_API_H

// Include files
#include "coder_array_mex.h"
#include "emlrt.h"
#include "mex.h"
#include "tmwtypes.h"
#include <algorithm>
#include <cstring>

// Variable Declarations
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

// Function Declarations
void TIME_entrypoint(coder::array<real_T, 1U> *wav, real_T fs, real_T N,
                     real_T M, coder::array<real_T, 2U> *ZTW_SPEC,
                     coder::array<real_T, 2U> *ZTW_HNGD_SPEC);

void TIME_entrypoint_api(const mxArray *const prhs[4], int32_T nlhs,
                         const mxArray *plhs[2]);

void TIME_entrypoint_atexit();

void TIME_entrypoint_initialize();

void TIME_entrypoint_terminate();

void TIME_entrypoint_xil_shutdown();

void TIME_entrypoint_xil_terminate();

#endif
// End of code generation (_coder_TIME_entrypoint_api.h)
