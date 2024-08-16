//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// hilbert.cpp
//
// Code generation for function 'hilbert'
//

// Include files
#include "hilbert.h"
#include "FFTImplementationCallback.h"
#include "TIME_entrypoint_data.h"
#include "fft.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace coder {
static void HilbertColumnwise(const array<double, 2U> &xr,
                              array<creal_T, 2U> &x);

}

// Function Definitions
namespace coder {
static void HilbertColumnwise(const array<double, 2U> &xr,
                              array<creal_T, 2U> &x)
{
  array<creal_T, 2U> b_x;
  array<double, 2U> costab;
  array<double, 2U> sintab;
  array<double, 2U> sintabinv;
  int firstIndexToZero;
  int halfn;
  int lastIndexToDouble;
  int ncols;
  int nrows;
  boolean_T useRadix2;
  if ((xr.size(0) == 0) || (xr.size(1) == 0)) {
    x.set_size(xr.size(0), xr.size(1));
    halfn = xr.size(0) * xr.size(1);
    for (nrows = 0; nrows < halfn; nrows++) {
      x[nrows].re = 0.0;
      x[nrows].im = 0.0;
    }
  } else {
    useRadix2 = ((xr.size(0) & (xr.size(0) - 1)) == 0);
    firstIndexToZero = internal::fft::FFTImplementationCallback::get_algo_sizes(
        xr.size(0), useRadix2, halfn);
    internal::fft::FFTImplementationCallback::generate_twiddle_tables(
        halfn, useRadix2, costab, sintab, sintabinv);
    if (useRadix2) {
      internal::fft::FFTImplementationCallback::b_r2br_r2dit_trig(
          xr, xr.size(0), costab, sintab, x);
    } else {
      internal::fft::FFTImplementationCallback::b_dobluesteinfft(
          xr, firstIndexToZero, xr.size(0), costab, sintab, sintabinv, x);
    }
  }
  nrows = xr.size(0);
  ncols = x.size(1);
  halfn = xr.size(0) >> 1;
  if ((xr.size(0) & 1) == 0) {
    lastIndexToDouble = halfn;
  } else {
    lastIndexToDouble = halfn + 1;
  }
  firstIndexToZero = halfn + 2;
  for (halfn = 0; halfn < ncols; halfn++) {
    for (int i{2}; i <= lastIndexToDouble; i++) {
      x[(i + x.size(0) * halfn) - 1].re =
          2.0 * x[(i + x.size(0) * halfn) - 1].re;
      x[(i + x.size(0) * halfn) - 1].im =
          2.0 * x[(i + x.size(0) * halfn) - 1].im;
    }
    for (int i{firstIndexToZero}; i <= nrows; i++) {
      x[(i + x.size(0) * halfn) - 1].re = 0.0;
      x[(i + x.size(0) * halfn) - 1].im = 0.0;
    }
  }
  if ((x.size(0) == 0) || (x.size(1) == 0)) {
    unsigned int ySize_idx_0;
    unsigned int ySize_idx_1;
    ySize_idx_1 = static_cast<unsigned int>(x.size(1));
    ySize_idx_0 = static_cast<unsigned int>(x.size(0));
    x.set_size(static_cast<int>(ySize_idx_0), static_cast<int>(ySize_idx_1));
    halfn = static_cast<int>(ySize_idx_0) * static_cast<int>(ySize_idx_1);
    for (nrows = 0; nrows < halfn; nrows++) {
      x[nrows].re = 0.0;
      x[nrows].im = 0.0;
    }
  } else {
    useRadix2 = ((x.size(0) & (x.size(0) - 1)) == 0);
    firstIndexToZero = internal::fft::FFTImplementationCallback::get_algo_sizes(
        x.size(0), useRadix2, halfn);
    internal::fft::FFTImplementationCallback::c_generate_twiddle_tables(
        halfn, useRadix2, costab, sintab, sintabinv);
    if (useRadix2) {
      b_x.set_size(x.size(0), x.size(1));
      halfn = x.size(0) * x.size(1) - 1;
      for (nrows = 0; nrows <= halfn; nrows++) {
        b_x[nrows] = x[nrows];
      }
      internal::fft::FFTImplementationCallback::r2br_r2dit_trig(
          b_x, x.size(0), costab, sintab, x);
    } else {
      b_x.set_size(x.size(0), x.size(1));
      halfn = x.size(0) * x.size(1) - 1;
      for (nrows = 0; nrows <= halfn; nrows++) {
        b_x[nrows] = x[nrows];
      }
      internal::fft::FFTImplementationCallback::dobluesteinfft(
          b_x, firstIndexToZero, x.size(0), costab, sintab, sintabinv, x);
    }
  }
}

void hilbert(const array<double, 2U> &xr, array<creal_T, 2U> &x)
{
  array<creal_T, 2U> b_x;
  array<creal_T, 2U> r;
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> c_x;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> wwc;
  array<double, 2U> c_xr;
  array<double, 2U> sintab;
  array<double, 2U> sintabinv;
  array<double, 1U> b_xr;
  int dim;
  int k;
  boolean_T exitg1;
  dim = 1;
  k = 0;
  exitg1 = false;
  while ((!exitg1) && (k < 2)) {
    if (xr.size(k) != 1) {
      dim = k + 1;
      exitg1 = true;
    } else {
      k++;
    }
  }
  if ((xr.size(0) == 1) || (xr.size(1) == 1)) {
    int i;
    int lastIndexToDouble;
    int nInt2;
    int nfft;
    int xr_tmp;
    xr_tmp = xr.size(0) * xr.size(1);
    b_xr = xr.reshape(xr_tmp);
    fft(b_xr, c_x);
    dim = xr_tmp >> 1;
    if ((xr_tmp & 1) == 0) {
      lastIndexToDouble = dim;
    } else {
      lastIndexToDouble = dim + 1;
    }
    dim += 2;
    for (nInt2 = 2; nInt2 <= lastIndexToDouble; nInt2++) {
      c_x[nInt2 - 1].re = 2.0 * c_x[nInt2 - 1].re;
      c_x[nInt2 - 1].im = 2.0 * c_x[nInt2 - 1].im;
    }
    for (nInt2 = dim; nInt2 <= xr_tmp; nInt2++) {
      c_x[nInt2 - 1].re = 0.0;
      c_x[nInt2 - 1].im = 0.0;
    }
    nfft = c_x.size(0);
    if (c_x.size(0) == 0) {
      c_x.set_size(0);
    } else {
      int N2blue;
      boolean_T useRadix2;
      useRadix2 = ((c_x.size(0) & (c_x.size(0) - 1)) == 0);
      N2blue = internal::fft::FFTImplementationCallback::get_algo_sizes(
          c_x.size(0), useRadix2, dim);
      internal::fft::FFTImplementationCallback::c_generate_twiddle_tables(
          dim, useRadix2, c_xr, sintab, sintabinv);
      if (useRadix2) {
        wwc.set_size(c_x.size(0));
        dim = c_x.size(0) - 1;
        for (i = 0; i <= dim; i++) {
          wwc[i] = c_x[i];
        }
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            wwc, c_x.size(0), c_xr, sintab, c_x);
        if (c_x.size(0) > 1) {
          double nt_re;
          nt_re = 1.0 / static_cast<double>(c_x.size(0));
          dim = c_x.size(0);
          for (i = 0; i < dim; i++) {
            c_x[i].re = nt_re * c_x[i].re;
            c_x[i].im = nt_re * c_x[i].im;
          }
        }
      } else {
        double b_re_tmp;
        double nt_im;
        double nt_re;
        double re_tmp;
        dim = (c_x.size(0) + c_x.size(0)) - 1;
        wwc.set_size(dim);
        lastIndexToDouble = 0;
        wwc[c_x.size(0) - 1].re = 1.0;
        wwc[c_x.size(0) - 1].im = 0.0;
        nInt2 = c_x.size(0) << 1;
        i = c_x.size(0);
        for (k = 0; k <= i - 2; k++) {
          int y;
          y = ((k + 1) << 1) - 1;
          if (nInt2 - lastIndexToDouble <= y) {
            lastIndexToDouble += y - nInt2;
          } else {
            lastIndexToDouble += y;
          }
          nt_im = 3.1415926535897931 * static_cast<double>(lastIndexToDouble) /
                  static_cast<double>(nfft);
          wwc[(c_x.size(0) - k) - 2].re = std::cos(nt_im);
          wwc[(c_x.size(0) - k) - 2].im = -std::sin(nt_im);
        }
        i = dim - 1;
        for (k = i; k >= nfft; k--) {
          wwc[k] = wwc[(dim - k) - 1];
        }
        fv.set_size(c_x.size(0));
        dim = c_x.size(0);
        for (i = 0; i < dim; i++) {
          fv[i] = c_x[i];
        }
        dim = c_x.size(0);
        lastIndexToDouble = c_x.size(0);
        c_x.set_size(lastIndexToDouble);
        if (nfft > dim) {
          c_x.set_size(lastIndexToDouble);
          for (i = 0; i < lastIndexToDouble; i++) {
            c_x[i].re = 0.0;
            c_x[i].im = 0.0;
          }
        }
        dim = fv.size(0);
        if (nfft <= dim) {
          dim = nfft;
        }
        for (k = 0; k < dim; k++) {
          lastIndexToDouble = (nfft + k) - 1;
          nt_re = wwc[lastIndexToDouble].re;
          nt_im = wwc[lastIndexToDouble].im;
          re_tmp = fv[k].im;
          b_re_tmp = fv[k].re;
          c_x[k].re = nt_re * b_re_tmp + nt_im * re_tmp;
          c_x[k].im = nt_re * re_tmp - nt_im * b_re_tmp;
        }
        i = dim + 1;
        for (k = i; k <= nfft; k++) {
          c_x[k - 1].re = 0.0;
          c_x[k - 1].im = 0.0;
        }
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            c_x, N2blue, c_xr, sintab, fv);
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            wwc, N2blue, c_xr, sintab, b_fv);
        b_fv.set_size(fv.size(0));
        dim = fv.size(0);
        for (i = 0; i < dim; i++) {
          nt_re = fv[i].re;
          nt_im = b_fv[i].im;
          re_tmp = fv[i].im;
          b_re_tmp = b_fv[i].re;
          b_fv[i].re = nt_re * b_re_tmp - re_tmp * nt_im;
          b_fv[i].im = nt_re * nt_im + re_tmp * b_re_tmp;
        }
        internal::fft::FFTImplementationCallback::r2br_r2dit_trig_impl(
            b_fv, N2blue, c_xr, sintabinv, fv);
        if (fv.size(0) > 1) {
          nt_re = 1.0 / static_cast<double>(fv.size(0));
          dim = fv.size(0);
          for (i = 0; i < dim; i++) {
            fv[i].re = nt_re * fv[i].re;
            fv[i].im = nt_re * fv[i].im;
          }
        }
        i = wwc.size(0);
        for (k = nfft; k <= i; k++) {
          double ar;
          nt_re = wwc[k - 1].re;
          nt_im = fv[k - 1].im;
          re_tmp = wwc[k - 1].im;
          b_re_tmp = fv[k - 1].re;
          ar = nt_re * b_re_tmp + re_tmp * nt_im;
          nt_re = nt_re * nt_im - re_tmp * b_re_tmp;
          if (nt_re == 0.0) {
            dim = k - nfft;
            c_x[dim].re = ar / static_cast<double>(nfft);
            c_x[dim].im = 0.0;
          } else if (ar == 0.0) {
            dim = k - nfft;
            c_x[dim].re = 0.0;
            c_x[dim].im = nt_re / static_cast<double>(nfft);
          } else {
            dim = k - nfft;
            c_x[dim].re = ar / static_cast<double>(nfft);
            c_x[dim].im = nt_re / static_cast<double>(nfft);
          }
        }
      }
    }
    x.set_size(xr.size(0), xr.size(1));
    for (i = 0; i < xr_tmp; i++) {
      x[i] = c_x[i];
    }
  } else if (dim == 1) {
    HilbertColumnwise(xr, r);
    b_x.set_size(r.size(0), r.size(1));
    dim = r.size(0) * r.size(1);
    for (int i{0}; i < dim; i++) {
      b_x[i] = r[i];
    }
    x.set_size(b_x.size(0), b_x.size(1));
    for (int i{0}; i < dim; i++) {
      x[i] = b_x[i];
    }
  } else {
    dim = xr.size(1);
    c_xr = xr.reshape(dim, 1);
    HilbertColumnwise(c_xr, r);
    b_x.set_size(r.size(0), r.size(1));
    dim = r.size(0) * r.size(1);
    for (int i{0}; i < dim; i++) {
      b_x[i] = r[i];
    }
    x.set_size(b_x.size(0), 1);
    dim = b_x.size(0);
    for (int i{0}; i < dim; i++) {
      x[i] = b_x[i];
    }
  }
}

} // namespace coder

// End of code generation (hilbert.cpp)
