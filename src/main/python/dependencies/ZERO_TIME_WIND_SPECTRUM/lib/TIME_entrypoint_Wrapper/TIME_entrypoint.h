//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// TIME_entrypoint.h
//
// Code generation for function 'TIME_entrypoint'
//

#ifndef TIME_ENTRYPOINT_H
#define TIME_ENTRYPOINT_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Use conditional compilation for extern "C"
#ifdef __cplusplus
extern "C" {
#endif

// Function Declarations
void TIME_entrypoint(const coder::array<double, 1U> &wav, double fs,
                            double N, double M,
                            coder::array<double, 2U> &ZTW_SPEC,
                            coder::array<double, 2U> &ZTW_HNGD_SPEC);

// End conditional compilation for extern "C"
#ifdef __cplusplus
}

#endif

#endif
// End of code generation (TIME_entrypoint.h)
