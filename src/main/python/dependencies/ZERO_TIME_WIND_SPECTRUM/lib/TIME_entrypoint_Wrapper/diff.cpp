//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// diff.cpp
//
// Code generation for function 'diff'
//

// Include files
#include "diff.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace coder {
void diff(const array<creal_T, 2U> &x, array<creal_T, 2U> &y)
{
  int dimSize;
  dimSize = x.size(0);
  if (x.size(0) == 0) {
    y.set_size(0, x.size(1));
  } else {
    int u0;
    u0 = x.size(0) - 1;
    if (u0 > 1) {
      u0 = 1;
    }
    if (u0 < 1) {
      y.set_size(0, x.size(1));
    } else {
      y.set_size(x.size(0) - 1, x.size(1));
      if ((x.size(0) - 1 != 0) && (x.size(1) != 0)) {
        int iyStart;
        int nHigh;
        nHigh = x.size(1);
        iyStart = 0;
        for (int r{0}; r < nHigh; r++) {
          creal_T work_data;
          int ixLead_tmp;
          ixLead_tmp = r * dimSize;
          work_data = x[ixLead_tmp];
          for (int m{2}; m <= dimSize; m++) {
            double im;
            double re;
            double tmp1_im;
            double tmp1_re;
            u0 = (ixLead_tmp + m) - 1;
            tmp1_re = x[u0].re;
            tmp1_im = x[u0].im;
            re = tmp1_re;
            im = tmp1_im;
            tmp1_re -= work_data.re;
            tmp1_im -= work_data.im;
            work_data.re = re;
            work_data.im = im;
            u0 = (iyStart + m) - 2;
            y[u0].re = tmp1_re;
            y[u0].im = tmp1_im;
          }
          iyStart = (iyStart + dimSize) - 1;
        }
      }
    }
  }
}

void diff(const array<double, 2U> &x, array<double, 2U> &y)
{
  int dimSize;
  dimSize = x.size(0);
  if (x.size(0) == 0) {
    y.set_size(0, x.size(1));
  } else {
    int nHigh;
    nHigh = x.size(0) - 1;
    if (nHigh > 1) {
      nHigh = 1;
    }
    if (nHigh < 1) {
      y.set_size(0, x.size(1));
    } else {
      y.set_size(x.size(0) - 1, x.size(1));
      if ((x.size(0) - 1 != 0) && (x.size(1) != 0)) {
        int iyStart;
        nHigh = x.size(1);
        iyStart = 0;
        for (int r{0}; r < nHigh; r++) {
          double work_data;
          int ixLead_tmp;
          ixLead_tmp = r * dimSize;
          work_data = x[ixLead_tmp];
          for (int m{2}; m <= dimSize; m++) {
            double d;
            double tmp1;
            tmp1 = x[(ixLead_tmp + m) - 1];
            d = tmp1;
            tmp1 -= work_data;
            work_data = d;
            y[(iyStart + m) - 2] = tmp1;
          }
          iyStart = (iyStart + dimSize) - 1;
        }
      }
    }
  }
}

} // namespace coder

// End of code generation (diff.cpp)
