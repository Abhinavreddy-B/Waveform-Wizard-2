//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// ZERO_TIME_WIND_SPECTRUM.cpp
//
// Code generation for function 'ZERO_TIME_WIND_SPECTRUM'
//

// Include files
#include "ZERO_TIME_WIND_SPECTRUM.h"
#include "diff.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
void binary_expand_op(coder::array<double, 2U> &in1,
                      const coder::array<creal_T, 2U> &in2,
                      const coder::array<creal_T, 2U> &in3)
{
  coder::array<double, 2U> b_in2;
  int aux_0_1;
  int aux_1_1;
  int aux_2_1_tmp;
  int aux_3_1_tmp;
  int b_loop_ub;
  int loop_ub;
  int stride_0_0_tmp;
  int stride_0_1_tmp;
  int stride_1_0_tmp;
  int stride_1_1_tmp;
  if (in3.size(0) == 1) {
    loop_ub = in2.size(0);
  } else {
    loop_ub = in3.size(0);
  }
  if (in3.size(1) == 1) {
    b_loop_ub = in2.size(1);
  } else {
    b_loop_ub = in3.size(1);
  }
  b_in2.set_size(loop_ub, b_loop_ub);
  stride_0_0_tmp = (in2.size(0) != 1);
  stride_0_1_tmp = (in2.size(1) != 1);
  stride_1_0_tmp = (in3.size(0) != 1);
  stride_1_1_tmp = (in3.size(1) != 1);
  aux_0_1 = 0;
  aux_1_1 = 0;
  aux_2_1_tmp = 0;
  aux_3_1_tmp = 0;
  for (int i{0}; i < b_loop_ub; i++) {
    for (int i1{0}; i1 < loop_ub; i1++) {
      b_in2[i1 + b_in2.size(0) * i] =
          in2[i1 * stride_0_0_tmp + in2.size(0) * aux_0_1].re *
              in3[i1 * stride_1_0_tmp + in3.size(0) * aux_1_1].re +
          in2[i1 * stride_0_0_tmp + in2.size(0) * aux_2_1_tmp].im *
              in3[i1 * stride_1_0_tmp + in3.size(0) * aux_3_1_tmp].im;
    }
    aux_3_1_tmp += stride_1_1_tmp;
    aux_2_1_tmp += stride_0_1_tmp;
    aux_1_1 = aux_3_1_tmp;
    aux_0_1 = aux_2_1_tmp;
  }
  coder::diff(b_in2, in1);
}

// End of code generation (ZERO_TIME_WIND_SPECTRUM.cpp)
