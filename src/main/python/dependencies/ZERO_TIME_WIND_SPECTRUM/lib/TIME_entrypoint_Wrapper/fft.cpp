//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// fft.cpp
//
// Code generation for function 'fft'
//

// Include files
#include "fft.h"
#include "FFTImplementationCallback.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace coder {
void fft(const array<double, 2U> &x, double varargin_1, array<creal_T, 2U> &y)
{
  array<double, 2U> costab;
  array<double, 2U> sintab;
  array<double, 2U> sintabinv;
  int nRows;
  if ((x.size(1) == 0) || (static_cast<int>(varargin_1) == 0)) {
    int N2blue;
    y.set_size(static_cast<int>(varargin_1), x.size(1));
    N2blue = static_cast<int>(varargin_1) * x.size(1);
    for (nRows = 0; nRows < N2blue; nRows++) {
      y[nRows].re = 0.0;
      y[nRows].im = 0.0;
    }
  } else {
    int N2blue;
    boolean_T useRadix2;
    useRadix2 = ((static_cast<int>(varargin_1) > 0) &&
                 ((static_cast<int>(varargin_1) &
                   (static_cast<int>(varargin_1) - 1)) == 0));
    N2blue = internal::fft::FFTImplementationCallback::get_algo_sizes(
        static_cast<int>(varargin_1), useRadix2, nRows);
    internal::fft::FFTImplementationCallback::generate_twiddle_tables(
        nRows, useRadix2, costab, sintab, sintabinv);
    if (useRadix2) {
      internal::fft::FFTImplementationCallback::r2br_r2dit_trig(
          x, static_cast<int>(varargin_1), costab, sintab, y);
    } else {
      internal::fft::FFTImplementationCallback::dobluesteinfft(
          x, N2blue, static_cast<int>(varargin_1), costab, sintab, sintabinv,
          y);
    }
  }
}

void fft(const array<double, 1U> &x, array<creal_T, 1U> &y)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> wwc;
  array<double, 2U> costab;
  array<double, 2U> sintab;
  array<double, 2U> sintabinv;
  int minNrowsNx;
  int nfft;
  nfft = x.size(0);
  if (x.size(0) == 0) {
    y.set_size(0);
  } else {
    int N2blue;
    boolean_T useRadix2;
    useRadix2 = ((x.size(0) & (x.size(0) - 1)) == 0);
    N2blue = internal::fft::FFTImplementationCallback::get_algo_sizes(
        x.size(0), useRadix2, minNrowsNx);
    internal::fft::FFTImplementationCallback::generate_twiddle_tables(
        minNrowsNx, useRadix2, costab, sintab, sintabinv);
    if (useRadix2) {
      y.set_size(x.size(0));
      if (x.size(0) != 1) {
        internal::fft::FFTImplementationCallback::doHalfLengthRadix2(
            x, y, x.size(0), costab, sintab);
      } else {
        y[0].re = x[0];
        y[0].im = 0.0;
      }
    } else {
      double nt_im;
      int i;
      int nInt2m1;
      if ((x.size(0) != 1) && ((x.size(0) & 1) == 0)) {
        int nInt2;
        int rt;
        minNrowsNx =
            static_cast<int>(static_cast<unsigned int>(x.size(0)) >> 1);
        nInt2m1 = (minNrowsNx + minNrowsNx) - 1;
        wwc.set_size(nInt2m1);
        rt = 0;
        wwc[minNrowsNx - 1].re = 1.0;
        wwc[minNrowsNx - 1].im = 0.0;
        nInt2 = minNrowsNx << 1;
        for (int k{0}; k <= minNrowsNx - 2; k++) {
          int b_y;
          b_y = ((k + 1) << 1) - 1;
          if (nInt2 - rt <= b_y) {
            rt += b_y - nInt2;
          } else {
            rt += b_y;
          }
          nt_im = -3.1415926535897931 * static_cast<double>(rt) /
                  static_cast<double>(minNrowsNx);
          i = (minNrowsNx - k) - 2;
          wwc[i].re = std::cos(nt_im);
          wwc[i].im = -std::sin(nt_im);
        }
        i = nInt2m1 - 1;
        for (int k{i}; k >= minNrowsNx; k--) {
          wwc[k] = wwc[(nInt2m1 - k) - 1];
        }
      } else {
        int nInt2;
        int rt;
        nInt2m1 = (x.size(0) + x.size(0)) - 1;
        wwc.set_size(nInt2m1);
        rt = 0;
        wwc[x.size(0) - 1].re = 1.0;
        wwc[x.size(0) - 1].im = 0.0;
        nInt2 = x.size(0) << 1;
        i = x.size(0);
        for (int k{0}; k <= i - 2; k++) {
          int b_y;
          b_y = ((k + 1) << 1) - 1;
          if (nInt2 - rt <= b_y) {
            rt += b_y - nInt2;
          } else {
            rt += b_y;
          }
          nt_im = -3.1415926535897931 * static_cast<double>(rt) /
                  static_cast<double>(nfft);
          wwc[(x.size(0) - k) - 2].re = std::cos(nt_im);
          wwc[(x.size(0) - k) - 2].im = -std::sin(nt_im);
        }
        i = nInt2m1 - 1;
        for (int k{i}; k >= nfft; k--) {
          wwc[k] = wwc[(nInt2m1 - k) - 1];
        }
      }
      y.set_size(x.size(0));
      if ((N2blue != 1) && ((x.size(0) & 1) == 0)) {
        internal::fft::FFTImplementationCallback::doHalfLengthBluestein(
            x, y, x.size(0), x.size(0), N2blue, wwc, costab, sintab, costab,
            sintabinv);
      } else {
        double b_re_tmp;
        double c_re_tmp;
        double re_tmp;
        minNrowsNx = x.size(0);
        for (int k{0}; k < minNrowsNx; k++) {
          y[k].re = wwc[(nfft + k) - 1].re * x[k];
          y[k].im = wwc[(nfft + k) - 1].im * -x[k];
        }
        i = x.size(0) + 1;
        for (int k{i}; k <= nfft; k++) {
          y[k - 1].re = 0.0;
          y[k - 1].im = 0.0;
        }
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            y, N2blue, costab, sintab, fv);
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            wwc, N2blue, costab, sintab, b_fv);
        b_fv.set_size(fv.size(0));
        minNrowsNx = fv.size(0);
        for (i = 0; i < minNrowsNx; i++) {
          nt_im = fv[i].re;
          re_tmp = b_fv[i].im;
          b_re_tmp = fv[i].im;
          c_re_tmp = b_fv[i].re;
          b_fv[i].re = nt_im * c_re_tmp - b_re_tmp * re_tmp;
          b_fv[i].im = nt_im * re_tmp + b_re_tmp * c_re_tmp;
        }
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            b_fv, N2blue, costab, sintabinv, fv);
        if (fv.size(0) > 1) {
          nt_im = 1.0 / static_cast<double>(fv.size(0));
          minNrowsNx = fv.size(0);
          for (i = 0; i < minNrowsNx; i++) {
            fv[i].re = nt_im * fv[i].re;
            fv[i].im = nt_im * fv[i].im;
          }
        }
        i = x.size(0);
        minNrowsNx = wwc.size(0);
        for (int k{i}; k <= minNrowsNx; k++) {
          nt_im = wwc[k - 1].re;
          re_tmp = fv[k - 1].im;
          b_re_tmp = wwc[k - 1].im;
          c_re_tmp = fv[k - 1].re;
          nInt2m1 = k - i;
          y[nInt2m1].re = nt_im * c_re_tmp + b_re_tmp * re_tmp;
          y[nInt2m1].im = nt_im * re_tmp - b_re_tmp * c_re_tmp;
        }
      }
    }
  }
}

} // namespace coder

// End of code generation (fft.cpp)
