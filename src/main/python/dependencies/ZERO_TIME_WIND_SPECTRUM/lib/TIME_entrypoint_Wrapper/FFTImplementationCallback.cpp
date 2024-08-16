//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// FFTImplementationCallback.cpp
//
// Code generation for function 'FFTImplementationCallback'
//

// Include files
#include "FFTImplementationCallback.h"
#include "TIME_entrypoint_data.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "omp.h"
#include <cmath>

// Function Definitions
namespace coder {
namespace internal {
namespace fft {
void FFTImplementationCallback::b_doHalfLengthRadix2(
    const array<double, 2U> &x, int xoffInit, array<creal_T, 1U> &y,
    int unsigned_nRows, const array<double, 2U> &costab,
    const array<double, 2U> &sintab)
{
  array<creal_T, 1U> reconVar1;
  array<creal_T, 1U> reconVar2;
  array<double, 2U> hcostab;
  array<double, 2U> hsintab;
  array<int, 2U> wrapIndex;
  array<int, 1U> bitrevIndex;
  double im;
  double re;
  double temp_im;
  double temp_re;
  double twid_im;
  double twid_re;
  int bitrevIndex_tmp;
  int hszCostab;
  int i;
  int iDelta;
  int iDelta2;
  int iheight;
  int j;
  int k;
  int nRows;
  int nRowsD2;
  int nRowsM2;
  nRows = unsigned_nRows / 2;
  nRowsM2 = nRows - 2;
  nRowsD2 = nRows / 2;
  k = nRowsD2 / 2;
  hszCostab = static_cast<int>(static_cast<unsigned int>(costab.size(1)) >> 1);
  hcostab.set_size(1, hszCostab);
  hsintab.set_size(1, hszCostab);
  for (i = 0; i < hszCostab; i++) {
    j = ((i + 1) << 1) - 2;
    hcostab[i] = costab[j];
    hsintab[i] = sintab[j];
  }
  reconVar1.set_size(nRows);
  reconVar2.set_size(nRows);
  wrapIndex.set_size(1, nRows);
  for (i = 0; i < nRows; i++) {
    re = sintab[i];
    im = costab[i];
    reconVar1[i].re = re + 1.0;
    reconVar1[i].im = -im;
    reconVar2[i].re = 1.0 - re;
    reconVar2[i].im = im;
    if (i + 1 != 1) {
      wrapIndex[i] = (nRows - i) + 1;
    } else {
      wrapIndex[0] = 1;
    }
  }
  bitrevIndex_tmp = static_cast<int>(static_cast<double>(unsigned_nRows) / 2.0);
  hszCostab = y.size(0);
  if (hszCostab > nRows) {
    hszCostab = nRows;
  }
  FFTImplementationCallback::get_bitrevIndex(hszCostab - 1, bitrevIndex_tmp,
                                             bitrevIndex);
  if (unsigned_nRows >= 120) {
    hszCostab = 120;
  } else {
    hszCostab = unsigned_nRows;
  }
  hszCostab = static_cast<int>(static_cast<double>(hszCostab) / 2.0);
  for (i = 0; i < hszCostab; i++) {
    j = xoffInit + (i << 1);
    y[bitrevIndex[i] - 1].re = x[j];
    y[bitrevIndex[i] - 1].im = x[j + 1];
  }
  if (nRows > 1) {
    for (i = 0; i <= nRowsM2; i += 2) {
      re = y[i + 1].re;
      im = y[i + 1].im;
      temp_re = re;
      temp_im = im;
      twid_re = y[i].re;
      twid_im = y[i].im;
      re = twid_re - re;
      im = twid_im - im;
      y[i + 1].re = re;
      y[i + 1].im = im;
      twid_re += temp_re;
      twid_im += temp_im;
      y[i].re = twid_re;
      y[i].im = twid_im;
    }
  }
  iDelta = 2;
  iDelta2 = 4;
  iheight = ((k - 1) << 2) + 1;
  while (k > 0) {
    for (i = 0; i < iheight; i += iDelta2) {
      hszCostab = i + iDelta;
      temp_re = y[hszCostab].re;
      temp_im = y[hszCostab].im;
      y[hszCostab].re = y[i].re - temp_re;
      y[hszCostab].im = y[i].im - temp_im;
      y[i].re = y[i].re + temp_re;
      y[i].im = y[i].im + temp_im;
    }
    hszCostab = 1;
    for (j = k; j < nRowsD2; j += k) {
      twid_re = hcostab[j];
      twid_im = hsintab[j];
      i = hszCostab;
      nRows = hszCostab + iheight;
      while (i < nRows) {
        nRowsM2 = i + iDelta;
        re = y[nRowsM2].im;
        im = y[nRowsM2].re;
        temp_re = twid_re * im - twid_im * re;
        temp_im = twid_re * re + twid_im * im;
        y[nRowsM2].re = y[i].re - temp_re;
        y[nRowsM2].im = y[i].im - temp_im;
        y[i].re = y[i].re + temp_re;
        y[i].im = y[i].im + temp_im;
        i += iDelta2;
      }
      hszCostab++;
    }
    k /= 2;
    iDelta = iDelta2;
    iDelta2 += iDelta2;
    iheight -= iDelta;
  }
  FFTImplementationCallback::getback_radix2_fft(y, reconVar1, reconVar2,
                                                wrapIndex, bitrevIndex_tmp);
}

void FFTImplementationCallback::b_generate_twiddle_tables(
    int nRows, array<double, 2U> &costab, array<double, 2U> &sintab,
    array<double, 2U> &sintabinv)
{
  array<double, 2U> costab1q;
  double e;
  int i;
  int n;
  int nd2;
  e = 6.2831853071795862 / static_cast<double>(nRows);
  n = nRows / 2 / 2;
  costab1q.set_size(1, n + 1);
  costab1q[0] = 1.0;
  nd2 = n / 2 - 1;
  for (int k{0}; k <= nd2; k++) {
    costab1q[k + 1] = std::cos(e * (static_cast<double>(k) + 1.0));
  }
  i = nd2 + 2;
  nd2 = n - 1;
  for (int k{i}; k <= nd2; k++) {
    costab1q[k] = std::sin(e * static_cast<double>(n - k));
  }
  costab1q[n] = 0.0;
  n = costab1q.size(1) - 1;
  nd2 = (costab1q.size(1) - 1) << 1;
  costab.set_size(1, nd2 + 1);
  sintab.set_size(1, nd2 + 1);
  costab[0] = 1.0;
  sintab[0] = 0.0;
  sintabinv.set_size(1, nd2 + 1);
  for (int k{0}; k < n; k++) {
    sintabinv[k + 1] = costab1q[(n - k) - 1];
  }
  i = costab1q.size(1);
  for (int k{i}; k <= nd2; k++) {
    sintabinv[k] = costab1q[k - n];
  }
  for (int k{0}; k < n; k++) {
    costab[k + 1] = costab1q[k + 1];
    sintab[k + 1] = -costab1q[(n - k) - 1];
  }
  i = costab1q.size(1);
  for (int k{i}; k <= nd2; k++) {
    costab[k] = -costab1q[nd2 - k];
    sintab[k] = -costab1q[k - n];
  }
}

void FFTImplementationCallback::doHalfLengthBluestein(
    const array<double, 2U> &x, int xoffInit, array<creal_T, 1U> &y, int nRows,
    int nfft, const array<creal_T, 1U> &wwc, const array<double, 2U> &costab,
    const array<double, 2U> &sintab, const array<double, 2U> &costabinv,
    const array<double, 2U> &sintabinv)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> reconVar1;
  array<creal_T, 1U> reconVar2;
  array<creal_T, 1U> ytmp;
  array<double, 2U> a__1;
  array<double, 2U> costable;
  array<double, 2U> hcostabinv;
  array<double, 2U> hsintab;
  array<double, 2U> hsintabinv;
  array<double, 2U> sintable;
  array<int, 2U> wrapIndex;
  double a_im;
  double a_re;
  double b_im;
  double b_re;
  int a_re_tmp;
  int b_i;
  int hnRows;
  int i;
  hnRows = nRows / 2;
  ytmp.set_size(hnRows);
  if (hnRows > 120) {
    ytmp.set_size(hnRows);
    for (i = 0; i < hnRows; i++) {
      ytmp[i].re = 0.0;
      ytmp[i].im = 0.0;
    }
  }
  FFTImplementationCallback::b_generate_twiddle_tables(nRows << 1, costable,
                                                       sintable, a__1);
  FFTImplementationCallback::get_half_twiddle_tables(costab, sintab, costabinv,
                                                     sintabinv, a__1, hsintab,
                                                     hcostabinv, hsintabinv);
  reconVar1.set_size(hnRows);
  reconVar2.set_size(hnRows);
  wrapIndex.set_size(1, hnRows);
  for (b_i = 0; b_i < hnRows; b_i++) {
    i = b_i << 1;
    a_re = sintable[i];
    a_im = costable[i];
    reconVar1[b_i].re = a_re + 1.0;
    reconVar1[b_i].im = -a_im;
    reconVar2[b_i].re = 1.0 - a_re;
    reconVar2[b_i].im = a_im;
    if (b_i + 1 != 1) {
      wrapIndex[b_i] = (hnRows - b_i) + 1;
    } else {
      wrapIndex[0] = 1;
    }
  }
  if (nRows >= 120) {
    i = 120;
  } else {
    i = nRows;
  }
  i = static_cast<int>(static_cast<double>(i) / 2.0);
  for (b_i = 0; b_i < i; b_i++) {
    a_re_tmp = (hnRows + b_i) - 1;
    a_re = wwc[a_re_tmp].re;
    a_im = wwc[a_re_tmp].im;
    a_re_tmp = xoffInit + (b_i << 1);
    b_re = x[a_re_tmp];
    b_im = x[a_re_tmp + 1];
    ytmp[b_i].re = a_re * b_re + a_im * b_im;
    ytmp[b_i].im = a_re * b_im - a_im * b_re;
  }
  a_re_tmp = i + 1;
  if (i + 1 <= hnRows) {
    for (b_i = a_re_tmp; b_i <= hnRows; b_i++) {
      ytmp[b_i - 1].re = 0.0;
      ytmp[b_i - 1].im = 0.0;
    }
  }
  a_re_tmp = static_cast<int>(static_cast<double>(nfft) / 2.0);
  FFTImplementationCallback::r2br_r2dit_trig_impl(ytmp, a_re_tmp, a__1, hsintab,
                                                  fv);
  FFTImplementationCallback::r2br_r2dit_trig_impl(wwc, a_re_tmp, a__1, hsintab,
                                                  b_fv);
  b_fv.set_size(fv.size(0));
  b_i = fv.size(0);
  for (i = 0; i < b_i; i++) {
    a_re = fv[i].re;
    a_im = b_fv[i].im;
    b_re = fv[i].im;
    b_im = b_fv[i].re;
    b_fv[i].re = a_re * b_im - b_re * a_im;
    b_fv[i].im = a_re * a_im + b_re * b_im;
  }
  FFTImplementationCallback::r2br_r2dit_trig_impl(b_fv, a_re_tmp, hcostabinv,
                                                  hsintabinv, fv);
  if (fv.size(0) > 1) {
    a_re = 1.0 / static_cast<double>(fv.size(0));
    b_i = fv.size(0);
    for (i = 0; i < b_i; i++) {
      fv[i].re = a_re * fv[i].re;
      fv[i].im = a_re * fv[i].im;
    }
  }
  i = wwc.size(0);
  for (b_i = hnRows; b_i <= i; b_i++) {
    a_re = wwc[b_i - 1].re;
    a_im = fv[b_i - 1].im;
    b_re = wwc[b_i - 1].im;
    b_im = fv[b_i - 1].re;
    a_re_tmp = b_i - hnRows;
    ytmp[a_re_tmp].re = a_re * b_im + b_re * a_im;
    ytmp[a_re_tmp].im = a_re * a_im - b_re * b_im;
  }
  for (b_i = 0; b_i < hnRows; b_i++) {
    double b_ytmp_re_tmp;
    double c_ytmp_re_tmp;
    double ytmp_im_tmp;
    double ytmp_re_tmp;
    i = wrapIndex[b_i];
    a_re = ytmp[b_i].re;
    a_im = reconVar1[b_i].im;
    b_re = ytmp[b_i].im;
    b_im = reconVar1[b_i].re;
    ytmp_re_tmp = ytmp[i - 1].re;
    ytmp_im_tmp = -ytmp[i - 1].im;
    b_ytmp_re_tmp = reconVar2[b_i].im;
    c_ytmp_re_tmp = reconVar2[b_i].re;
    y[b_i].re =
        0.5 * ((a_re * b_im - b_re * a_im) +
               (ytmp_re_tmp * c_ytmp_re_tmp - ytmp_im_tmp * b_ytmp_re_tmp));
    y[b_i].im =
        0.5 * ((a_re * a_im + b_re * b_im) +
               (ytmp_re_tmp * b_ytmp_re_tmp + ytmp_im_tmp * c_ytmp_re_tmp));
    i = hnRows + b_i;
    y[i].re = 0.5 * ((a_re * c_ytmp_re_tmp - b_re * b_ytmp_re_tmp) +
                     (ytmp_re_tmp * b_im - ytmp_im_tmp * a_im));
    y[i].im = 0.5 * ((a_re * b_ytmp_re_tmp + b_re * c_ytmp_re_tmp) +
                     (ytmp_re_tmp * a_im + ytmp_im_tmp * b_im));
  }
}

void FFTImplementationCallback::doHalfLengthBluestein(
    const array<double, 2U> &x, int xoffInit, array<creal_T, 1U> &y, int nrowsx,
    int nRows, int nfft, const array<creal_T, 1U> &wwc,
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    const array<double, 2U> &costabinv, const array<double, 2U> &sintabinv)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> reconVar1;
  array<creal_T, 1U> reconVar2;
  array<creal_T, 1U> ytmp;
  array<double, 2U> a__1;
  array<double, 2U> costable;
  array<double, 2U> hcostabinv;
  array<double, 2U> hsintab;
  array<double, 2U> hsintabinv;
  array<double, 2U> sintable;
  array<int, 2U> wrapIndex;
  double a_im;
  double a_re;
  double b_im;
  double b_re;
  double z_tmp;
  int b_i;
  int b_re_tmp;
  int hnRows;
  int i;
  int u0;
  boolean_T nxeven;
  hnRows = nRows / 2;
  ytmp.set_size(hnRows);
  if (hnRows > nrowsx) {
    ytmp.set_size(hnRows);
    for (i = 0; i < hnRows; i++) {
      ytmp[i].re = 0.0;
      ytmp[i].im = 0.0;
    }
  }
  if ((x.size(0) & 1) == 0) {
    nxeven = true;
    u0 = x.size(0);
  } else if (x.size(0) >= nRows) {
    nxeven = true;
    u0 = nRows;
  } else {
    nxeven = false;
    u0 = x.size(0) - 1;
  }
  if (u0 > nRows) {
    u0 = nRows;
  }
  FFTImplementationCallback::b_generate_twiddle_tables(nRows << 1, costable,
                                                       sintable, a__1);
  FFTImplementationCallback::get_half_twiddle_tables(costab, sintab, costabinv,
                                                     sintabinv, a__1, hsintab,
                                                     hcostabinv, hsintabinv);
  reconVar1.set_size(hnRows);
  reconVar2.set_size(hnRows);
  wrapIndex.set_size(1, hnRows);
  for (b_i = 0; b_i < hnRows; b_i++) {
    i = b_i << 1;
    z_tmp = sintable[i];
    a_re = costable[i];
    reconVar1[b_i].re = z_tmp + 1.0;
    reconVar1[b_i].im = -a_re;
    reconVar2[b_i].re = 1.0 - z_tmp;
    reconVar2[b_i].im = a_re;
    if (b_i + 1 != 1) {
      wrapIndex[b_i] = (hnRows - b_i) + 1;
    } else {
      wrapIndex[0] = 1;
    }
  }
  z_tmp = static_cast<double>(u0) / 2.0;
  i = static_cast<int>(z_tmp);
  for (int k1{0}; k1 < i; k1++) {
    b_i = (hnRows + k1) - 1;
    a_re = wwc[b_i].re;
    a_im = wwc[b_i].im;
    b_re_tmp = xoffInit + (k1 << 1);
    b_re = x[b_re_tmp];
    b_im = x[b_re_tmp + 1];
    ytmp[k1].re = a_re * b_re + a_im * b_im;
    ytmp[k1].im = a_re * b_im - a_im * b_re;
  }
  if (!nxeven) {
    b_i = (hnRows + static_cast<int>(z_tmp)) - 1;
    a_re = wwc[b_i].re;
    a_im = wwc[b_i].im;
    if (static_cast<int>(z_tmp) - 1 < 0) {
      i = xoffInit;
    } else {
      i = xoffInit + (static_cast<int>(z_tmp) << 1);
    }
    b_re = x[i];
    ytmp[static_cast<int>(static_cast<double>(u0) / 2.0)].re =
        a_re * b_re + a_im * 0.0;
    ytmp[static_cast<int>(static_cast<double>(u0) / 2.0)].im =
        a_re * 0.0 - a_im * b_re;
    if (static_cast<int>(z_tmp) + 2 <= hnRows) {
      i = static_cast<int>(z_tmp) + 2;
      for (b_i = i; b_i <= hnRows; b_i++) {
        ytmp[b_i - 1].re = 0.0;
        ytmp[b_i - 1].im = 0.0;
      }
    }
  } else if (static_cast<int>(z_tmp) + 1 <= hnRows) {
    i = static_cast<int>(z_tmp) + 1;
    for (b_i = i; b_i <= hnRows; b_i++) {
      ytmp[b_i - 1].re = 0.0;
      ytmp[b_i - 1].im = 0.0;
    }
  }
  b_i = static_cast<int>(static_cast<double>(nfft) / 2.0);
  FFTImplementationCallback::r2br_r2dit_trig_impl(ytmp, b_i, a__1, hsintab, fv);
  FFTImplementationCallback::r2br_r2dit_trig_impl(wwc, b_i, a__1, hsintab,
                                                  b_fv);
  b_fv.set_size(fv.size(0));
  b_re_tmp = fv.size(0);
  for (i = 0; i < b_re_tmp; i++) {
    z_tmp = fv[i].re;
    a_re = b_fv[i].im;
    a_im = fv[i].im;
    b_re = b_fv[i].re;
    b_fv[i].re = z_tmp * b_re - a_im * a_re;
    b_fv[i].im = z_tmp * a_re + a_im * b_re;
  }
  FFTImplementationCallback::r2br_r2dit_trig_impl(b_fv, b_i, hcostabinv,
                                                  hsintabinv, fv);
  if (fv.size(0) > 1) {
    z_tmp = 1.0 / static_cast<double>(fv.size(0));
    b_re_tmp = fv.size(0);
    for (i = 0; i < b_re_tmp; i++) {
      fv[i].re = z_tmp * fv[i].re;
      fv[i].im = z_tmp * fv[i].im;
    }
  }
  i = wwc.size(0);
  for (b_re_tmp = hnRows; b_re_tmp <= i; b_re_tmp++) {
    z_tmp = wwc[b_re_tmp - 1].re;
    a_re = fv[b_re_tmp - 1].im;
    a_im = wwc[b_re_tmp - 1].im;
    b_re = fv[b_re_tmp - 1].re;
    b_i = b_re_tmp - hnRows;
    ytmp[b_i].re = z_tmp * b_re + a_im * a_re;
    ytmp[b_i].im = z_tmp * a_re - a_im * b_re;
  }
  for (b_i = 0; b_i < hnRows; b_i++) {
    double b_ytmp_re_tmp;
    double ytmp_im_tmp;
    double ytmp_re_tmp;
    i = wrapIndex[b_i];
    z_tmp = ytmp[b_i].re;
    a_re = reconVar1[b_i].im;
    a_im = ytmp[b_i].im;
    b_re = reconVar1[b_i].re;
    b_im = ytmp[i - 1].re;
    ytmp_im_tmp = -ytmp[i - 1].im;
    ytmp_re_tmp = reconVar2[b_i].im;
    b_ytmp_re_tmp = reconVar2[b_i].re;
    y[b_i].re = 0.5 * ((z_tmp * b_re - a_im * a_re) +
                       (b_im * b_ytmp_re_tmp - ytmp_im_tmp * ytmp_re_tmp));
    y[b_i].im = 0.5 * ((z_tmp * a_re + a_im * b_re) +
                       (b_im * ytmp_re_tmp + ytmp_im_tmp * b_ytmp_re_tmp));
    i = hnRows + b_i;
    y[i].re = 0.5 * ((z_tmp * b_ytmp_re_tmp - a_im * ytmp_re_tmp) +
                     (b_im * b_re - ytmp_im_tmp * a_re));
    y[i].im = 0.5 * ((z_tmp * ytmp_re_tmp + a_im * b_ytmp_re_tmp) +
                     (b_im * a_re + ytmp_im_tmp * b_re));
  }
}

void FFTImplementationCallback::doHalfLengthRadix2(
    const array<double, 2U> &x, int xoffInit, array<creal_T, 1U> &y,
    int unsigned_nRows, const array<double, 2U> &costab,
    const array<double, 2U> &sintab)
{
  array<creal_T, 1U> reconVar1;
  array<creal_T, 1U> reconVar2;
  array<double, 2U> hcostab;
  array<double, 2U> hsintab;
  array<int, 2U> wrapIndex;
  array<int, 1U> bitrevIndex;
  double im;
  double re;
  double temp_im;
  double temp_re;
  double twid_im;
  double twid_re;
  int bitrevIndex_tmp;
  int hszCostab;
  int i;
  int iDelta;
  int iDelta2;
  int iheight;
  int j;
  int k;
  int nRows;
  int nRowsD2;
  int nRowsM2;
  boolean_T nxeven;
  nRows = unsigned_nRows / 2;
  nRowsM2 = nRows - 2;
  nRowsD2 = nRows / 2;
  k = nRowsD2 / 2;
  hszCostab = static_cast<int>(static_cast<unsigned int>(costab.size(1)) >> 1);
  hcostab.set_size(1, hszCostab);
  hsintab.set_size(1, hszCostab);
  for (i = 0; i < hszCostab; i++) {
    j = ((i + 1) << 1) - 2;
    hcostab[i] = costab[j];
    hsintab[i] = sintab[j];
  }
  reconVar1.set_size(nRows);
  reconVar2.set_size(nRows);
  wrapIndex.set_size(1, nRows);
  for (i = 0; i < nRows; i++) {
    re = sintab[i];
    im = costab[i];
    reconVar1[i].re = re + 1.0;
    reconVar1[i].im = -im;
    reconVar2[i].re = 1.0 - re;
    reconVar2[i].im = im;
    if (i + 1 != 1) {
      wrapIndex[i] = (nRows - i) + 1;
    } else {
      wrapIndex[0] = 1;
    }
  }
  bitrevIndex_tmp = static_cast<int>(static_cast<double>(unsigned_nRows) / 2.0);
  hszCostab = y.size(0);
  if (hszCostab > nRows) {
    hszCostab = nRows;
  }
  FFTImplementationCallback::get_bitrevIndex(hszCostab - 1, bitrevIndex_tmp,
                                             bitrevIndex);
  if ((x.size(0) & 1) == 0) {
    nxeven = true;
    hszCostab = x.size(0);
  } else if (x.size(0) >= unsigned_nRows) {
    nxeven = true;
    hszCostab = unsigned_nRows;
  } else {
    nxeven = false;
    hszCostab = x.size(0) - 1;
  }
  if (hszCostab > unsigned_nRows) {
    hszCostab = unsigned_nRows;
  }
  re = static_cast<double>(hszCostab) / 2.0;
  hszCostab = static_cast<int>(re);
  for (i = 0; i < hszCostab; i++) {
    j = xoffInit + (i << 1);
    y[bitrevIndex[i] - 1].re = x[j];
    y[bitrevIndex[i] - 1].im = x[j + 1];
  }
  if (!nxeven) {
    hszCostab = bitrevIndex[static_cast<int>(re)] - 1;
    if (static_cast<int>(re) - 1 < 0) {
      j = xoffInit;
    } else {
      j = xoffInit + (static_cast<int>(re) << 1);
    }
    y[hszCostab].re = x[j];
    y[hszCostab].im = 0.0;
  }
  if (nRows > 1) {
    for (i = 0; i <= nRowsM2; i += 2) {
      re = y[i + 1].re;
      im = y[i + 1].im;
      temp_re = re;
      temp_im = im;
      twid_re = y[i].re;
      twid_im = y[i].im;
      re = twid_re - re;
      im = twid_im - im;
      y[i + 1].re = re;
      y[i + 1].im = im;
      twid_re += temp_re;
      twid_im += temp_im;
      y[i].re = twid_re;
      y[i].im = twid_im;
    }
  }
  iDelta = 2;
  iDelta2 = 4;
  iheight = ((k - 1) << 2) + 1;
  while (k > 0) {
    for (i = 0; i < iheight; i += iDelta2) {
      hszCostab = i + iDelta;
      temp_re = y[hszCostab].re;
      temp_im = y[hszCostab].im;
      y[hszCostab].re = y[i].re - temp_re;
      y[hszCostab].im = y[i].im - temp_im;
      y[i].re = y[i].re + temp_re;
      y[i].im = y[i].im + temp_im;
    }
    hszCostab = 1;
    for (j = k; j < nRowsD2; j += k) {
      twid_re = hcostab[j];
      twid_im = hsintab[j];
      i = hszCostab;
      nRows = hszCostab + iheight;
      while (i < nRows) {
        nRowsM2 = i + iDelta;
        re = y[nRowsM2].im;
        im = y[nRowsM2].re;
        temp_re = twid_re * im - twid_im * re;
        temp_im = twid_re * re + twid_im * im;
        y[nRowsM2].re = y[i].re - temp_re;
        y[nRowsM2].im = y[i].im - temp_im;
        y[i].re = y[i].re + temp_re;
        y[i].im = y[i].im + temp_im;
        i += iDelta2;
      }
      hszCostab++;
    }
    k /= 2;
    iDelta = iDelta2;
    iDelta2 += iDelta2;
    iheight -= iDelta;
  }
  FFTImplementationCallback::getback_radix2_fft(y, reconVar1, reconVar2,
                                                wrapIndex, bitrevIndex_tmp);
}

void FFTImplementationCallback::get_bitrevIndex(int nRowsM1, int nfftLen,
                                                array<int, 1U> &bitrevIndex)
{
  int iy;
  int ju;
  ju = 0;
  iy = 1;
  bitrevIndex.set_size(nfftLen);
  for (int b_j1{0}; b_j1 < nfftLen; b_j1++) {
    bitrevIndex[b_j1] = 0;
  }
  for (int b_j1{0}; b_j1 < nRowsM1; b_j1++) {
    boolean_T tst;
    bitrevIndex[b_j1] = iy;
    iy = nfftLen;
    tst = true;
    while (tst) {
      iy >>= 1;
      ju ^= iy;
      tst = ((ju & iy) == 0);
    }
    iy = ju + 1;
  }
  bitrevIndex[nRowsM1] = iy;
}

void FFTImplementationCallback::get_half_twiddle_tables(
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    const array<double, 2U> &costabinv, const array<double, 2U> &sintabinv,
    array<double, 2U> &hcostab, array<double, 2U> &hsintab,
    array<double, 2U> &hcostabinv, array<double, 2U> &hsintabinv)
{
  int hszCostab;
  hszCostab = static_cast<int>(static_cast<unsigned int>(costab.size(1)) >> 1);
  hcostab.set_size(1, hszCostab);
  hsintab.set_size(1, hszCostab);
  hcostabinv.set_size(1, hszCostab);
  hsintabinv.set_size(1, hszCostab);
  for (int i{0}; i < hszCostab; i++) {
    int hcostab_tmp;
    hcostab_tmp = ((i + 1) << 1) - 2;
    hcostab[i] = costab[hcostab_tmp];
    hsintab[i] = sintab[hcostab_tmp];
    hcostabinv[i] = costabinv[hcostab_tmp];
    hsintabinv[i] = sintabinv[hcostab_tmp];
  }
}

void FFTImplementationCallback::getback_radix2_fft(
    array<creal_T, 1U> &y, const array<creal_T, 1U> &reconVar1,
    const array<creal_T, 1U> &reconVar2, const array<int, 2U> &wrapIndex,
    int hnRows)
{
  double b_temp1_re_tmp;
  double b_temp2_re_tmp;
  double b_y_re_tmp;
  double c_y_re_tmp;
  double d_y_re_tmp;
  double temp1_im_tmp;
  double temp1_re_tmp;
  double temp1_re_tmp_tmp;
  double y_im_tmp;
  double y_re_tmp;
  int b_i;
  int iterVar;
  iterVar = hnRows / 2;
  temp1_re_tmp_tmp = y[0].re;
  temp1_im_tmp = y[0].im;
  y_re_tmp = temp1_re_tmp_tmp * reconVar1[0].re;
  y_im_tmp = temp1_re_tmp_tmp * reconVar1[0].im;
  b_y_re_tmp = temp1_re_tmp_tmp * reconVar2[0].re;
  temp1_re_tmp_tmp *= reconVar2[0].im;
  y[0].re = 0.5 * ((y_re_tmp - temp1_im_tmp * reconVar1[0].im) +
                   (b_y_re_tmp - -temp1_im_tmp * reconVar2[0].im));
  y[0].im = 0.5 * ((y_im_tmp + temp1_im_tmp * reconVar1[0].re) +
                   (temp1_re_tmp_tmp + -temp1_im_tmp * reconVar2[0].re));
  y[hnRows].re = 0.5 * ((b_y_re_tmp - temp1_im_tmp * reconVar2[0].im) +
                        (y_re_tmp - -temp1_im_tmp * reconVar1[0].im));
  y[hnRows].im = 0.5 * ((temp1_re_tmp_tmp + temp1_im_tmp * reconVar2[0].re) +
                        (y_im_tmp + -temp1_im_tmp * reconVar1[0].re));
  for (int i{2}; i <= iterVar; i++) {
    double temp2_im_tmp;
    double temp2_re_tmp;
    int i1;
    temp1_re_tmp = y[i - 1].re;
    temp1_im_tmp = y[i - 1].im;
    b_i = wrapIndex[i - 1];
    temp2_re_tmp = y[b_i - 1].re;
    temp2_im_tmp = y[b_i - 1].im;
    y_re_tmp = reconVar1[i - 1].im;
    b_y_re_tmp = reconVar1[i - 1].re;
    c_y_re_tmp = reconVar2[i - 1].im;
    d_y_re_tmp = reconVar2[i - 1].re;
    y[i - 1].re =
        0.5 * ((temp1_re_tmp * b_y_re_tmp - temp1_im_tmp * y_re_tmp) +
               (temp2_re_tmp * d_y_re_tmp - -temp2_im_tmp * c_y_re_tmp));
    y[i - 1].im =
        0.5 * ((temp1_re_tmp * y_re_tmp + temp1_im_tmp * b_y_re_tmp) +
               (temp2_re_tmp * c_y_re_tmp + -temp2_im_tmp * d_y_re_tmp));
    i1 = (hnRows + i) - 1;
    y[i1].re = 0.5 * ((temp1_re_tmp * d_y_re_tmp - temp1_im_tmp * c_y_re_tmp) +
                      (temp2_re_tmp * b_y_re_tmp - -temp2_im_tmp * y_re_tmp));
    y[i1].im = 0.5 * ((temp1_re_tmp * c_y_re_tmp + temp1_im_tmp * d_y_re_tmp) +
                      (temp2_re_tmp * y_re_tmp + -temp2_im_tmp * b_y_re_tmp));
    temp1_re_tmp_tmp = reconVar1[b_i - 1].im;
    b_temp2_re_tmp = reconVar1[b_i - 1].re;
    b_temp1_re_tmp = reconVar2[b_i - 1].im;
    y_im_tmp = reconVar2[b_i - 1].re;
    y[b_i - 1].re =
        0.5 *
        ((temp2_re_tmp * b_temp2_re_tmp - temp2_im_tmp * temp1_re_tmp_tmp) +
         (temp1_re_tmp * y_im_tmp - -temp1_im_tmp * b_temp1_re_tmp));
    y[b_i - 1].im =
        0.5 *
        ((temp2_re_tmp * temp1_re_tmp_tmp + temp2_im_tmp * b_temp2_re_tmp) +
         (temp1_re_tmp * b_temp1_re_tmp + -temp1_im_tmp * y_im_tmp));
    b_i = (b_i + hnRows) - 1;
    y[b_i].re =
        0.5 *
        ((temp2_re_tmp * y_im_tmp - temp2_im_tmp * b_temp1_re_tmp) +
         (temp1_re_tmp * b_temp2_re_tmp - -temp1_im_tmp * temp1_re_tmp_tmp));
    y[b_i].im =
        0.5 *
        ((temp2_re_tmp * b_temp1_re_tmp + temp2_im_tmp * y_im_tmp) +
         (temp1_re_tmp * temp1_re_tmp_tmp + -temp1_im_tmp * b_temp2_re_tmp));
  }
  if (iterVar != 0) {
    temp1_re_tmp = y[iterVar].re;
    temp1_im_tmp = y[iterVar].im;
    y_re_tmp = reconVar1[iterVar].im;
    b_y_re_tmp = reconVar1[iterVar].re;
    c_y_re_tmp = temp1_re_tmp * b_y_re_tmp;
    y_im_tmp = temp1_re_tmp * y_re_tmp;
    d_y_re_tmp = reconVar2[iterVar].im;
    b_temp2_re_tmp = reconVar2[iterVar].re;
    b_temp1_re_tmp = temp1_re_tmp * b_temp2_re_tmp;
    temp1_re_tmp_tmp = temp1_re_tmp * d_y_re_tmp;
    y[iterVar].re = 0.5 * ((c_y_re_tmp - temp1_im_tmp * y_re_tmp) +
                           (b_temp1_re_tmp - -temp1_im_tmp * d_y_re_tmp));
    y[iterVar].im = 0.5 * ((y_im_tmp + temp1_im_tmp * b_y_re_tmp) +
                           (temp1_re_tmp_tmp + -temp1_im_tmp * b_temp2_re_tmp));
    b_i = hnRows + iterVar;
    y[b_i].re = 0.5 * ((b_temp1_re_tmp - temp1_im_tmp * d_y_re_tmp) +
                       (c_y_re_tmp - -temp1_im_tmp * y_re_tmp));
    y[b_i].im = 0.5 * ((temp1_re_tmp_tmp + temp1_im_tmp * b_temp2_re_tmp) +
                       (y_im_tmp + -temp1_im_tmp * b_y_re_tmp));
  }
}

void FFTImplementationCallback::b_dobluesteinfft(
    const array<double, 2U> &x, int n2blue, int nfft,
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    const array<double, 2U> &sintabinv, array<creal_T, 2U> &y)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> r;
  array<creal_T, 1U> wwc;
  double b_re_tmp;
  double c_re_tmp;
  double d_re_tmp;
  double re_tmp;
  int b_k;
  int b_y;
  int i;
  int minNrowsNx;
  int nInt2m1;
  int u0;
  int xoff;
  if ((nfft != 1) && ((nfft & 1) == 0)) {
    int nInt2;
    int nRows;
    int rt;
    nRows = nfft / 2;
    nInt2m1 = (nRows + nRows) - 1;
    wwc.set_size(nInt2m1);
    rt = 0;
    wwc[nRows - 1].re = 1.0;
    wwc[nRows - 1].im = 0.0;
    nInt2 = nRows << 1;
    for (int k{0}; k <= nRows - 2; k++) {
      double nt_im;
      b_y = ((k + 1) << 1) - 1;
      if (nInt2 - rt <= b_y) {
        rt += b_y - nInt2;
      } else {
        rt += b_y;
      }
      nt_im = -3.1415926535897931 * static_cast<double>(rt) /
              static_cast<double>(nRows);
      i = (nRows - k) - 2;
      wwc[i].re = std::cos(nt_im);
      wwc[i].im = -std::sin(nt_im);
    }
    i = nInt2m1 - 1;
    for (int k{i}; k >= nRows; k--) {
      wwc[k] = wwc[(nInt2m1 - k) - 1];
    }
  } else {
    int nInt2;
    int rt;
    nInt2m1 = (nfft + nfft) - 1;
    wwc.set_size(nInt2m1);
    rt = 0;
    wwc[nfft - 1].re = 1.0;
    wwc[nfft - 1].im = 0.0;
    nInt2 = nfft << 1;
    for (int k{0}; k <= nfft - 2; k++) {
      double nt_im;
      b_y = ((k + 1) << 1) - 1;
      if (nInt2 - rt <= b_y) {
        rt += b_y - nInt2;
      } else {
        rt += b_y;
      }
      nt_im = -3.1415926535897931 * static_cast<double>(rt) /
              static_cast<double>(nfft);
      i = (nfft - k) - 2;
      wwc[i].re = std::cos(nt_im);
      wwc[i].im = -std::sin(nt_im);
    }
    i = nInt2m1 - 1;
    for (int k{i}; k >= nfft; k--) {
      wwc[k] = wwc[(nInt2m1 - k) - 1];
    }
  }
  nInt2m1 = x.size(0);
  y.set_size(nfft, x.size(1));
  if (nfft > x.size(0)) {
    y.set_size(nfft, x.size(1));
    b_y = nfft * x.size(1);
    for (i = 0; i < b_y; i++) {
      y[i].re = 0.0;
      y[i].im = 0.0;
    }
  }
  b_y = x.size(1) - 1;
#pragma omp parallel for num_threads(omp_get_max_threads()) private(           \
    fv, b_fv, r, xoff, minNrowsNx, u0, b_k, re_tmp, b_re_tmp, c_re_tmp,        \
    d_re_tmp)

  for (int chan = 0; chan <= b_y; chan++) {
    xoff = chan * nInt2m1;
    r.set_size(nfft);
    if (nfft > x.size(0)) {
      r.set_size(nfft);
      for (minNrowsNx = 0; minNrowsNx < nfft; minNrowsNx++) {
        r[minNrowsNx].re = 0.0;
        r[minNrowsNx].im = 0.0;
      }
    }
    if ((n2blue != 1) && ((nfft & 1) == 0)) {
      FFTImplementationCallback::doHalfLengthBluestein(
          x, xoff, r, x.size(0), nfft, n2blue, wwc, costab, sintab, costab,
          sintabinv);
    } else {
      minNrowsNx = x.size(0);
      if (nfft <= minNrowsNx) {
        minNrowsNx = nfft;
      }
      for (b_k = 0; b_k < minNrowsNx; b_k++) {
        u0 = (nfft + b_k) - 1;
        re_tmp = x[xoff + b_k];
        r[b_k].re = wwc[u0].re * re_tmp;
        r[b_k].im = wwc[u0].im * -re_tmp;
      }
      minNrowsNx++;
      for (b_k = minNrowsNx; b_k <= nfft; b_k++) {
        r[b_k - 1].re = 0.0;
        r[b_k - 1].im = 0.0;
      }
      FFTImplementationCallback::r2br_r2dit_trig_impl(r, n2blue, costab, sintab,
                                                      b_fv);
      FFTImplementationCallback::r2br_r2dit_trig_impl(wwc, n2blue, costab,
                                                      sintab, fv);
      fv.set_size(b_fv.size(0));
      u0 = b_fv.size(0);
      for (minNrowsNx = 0; minNrowsNx < u0; minNrowsNx++) {
        re_tmp = b_fv[minNrowsNx].re;
        b_re_tmp = fv[minNrowsNx].im;
        c_re_tmp = b_fv[minNrowsNx].im;
        d_re_tmp = fv[minNrowsNx].re;
        fv[minNrowsNx].re = re_tmp * d_re_tmp - c_re_tmp * b_re_tmp;
        fv[minNrowsNx].im = re_tmp * b_re_tmp + c_re_tmp * d_re_tmp;
      }
      FFTImplementationCallback::r2br_r2dit_trig_impl(fv, n2blue, costab,
                                                      sintabinv, b_fv);
      if (b_fv.size(0) > 1) {
        re_tmp = 1.0 / static_cast<double>(b_fv.size(0));
        u0 = b_fv.size(0);
        for (minNrowsNx = 0; minNrowsNx < u0; minNrowsNx++) {
          b_fv[minNrowsNx].re = re_tmp * b_fv[minNrowsNx].re;
          b_fv[minNrowsNx].im = re_tmp * b_fv[minNrowsNx].im;
        }
      }
      minNrowsNx = wwc.size(0);
      for (b_k = nfft; b_k <= minNrowsNx; b_k++) {
        re_tmp = wwc[b_k - 1].re;
        b_re_tmp = b_fv[b_k - 1].im;
        c_re_tmp = wwc[b_k - 1].im;
        d_re_tmp = b_fv[b_k - 1].re;
        u0 = b_k - nfft;
        r[u0].re = re_tmp * d_re_tmp + c_re_tmp * b_re_tmp;
        r[u0].im = re_tmp * b_re_tmp - c_re_tmp * d_re_tmp;
      }
    }
    u0 = y.size(0);
    for (minNrowsNx = 0; minNrowsNx < u0; minNrowsNx++) {
      y[minNrowsNx + y.size(0) * chan] = r[minNrowsNx];
    }
  }
}

void FFTImplementationCallback::b_r2br_r2dit_trig(
    const array<double, 2U> &x, int n1_unsigned,
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    array<creal_T, 2U> &y)
{
  array<creal_T, 1U> r;
  int i1;
  int loop_ub;
  int nrows;
  int xoff;
  nrows = x.size(0);
  y.set_size(n1_unsigned, x.size(1));
  if (n1_unsigned > x.size(0)) {
    y.set_size(n1_unsigned, x.size(1));
    loop_ub = n1_unsigned * x.size(1);
    for (int i{0}; i < loop_ub; i++) {
      y[i].re = 0.0;
      y[i].im = 0.0;
    }
  }
  loop_ub = x.size(1) - 1;
#pragma omp parallel for num_threads(omp_get_max_threads()) private(r, xoff, i1)

  for (int chan = 0; chan <= loop_ub; chan++) {
    xoff = chan * nrows;
    r.set_size(n1_unsigned);
    if (n1_unsigned > x.size(0)) {
      r.set_size(n1_unsigned);
      for (i1 = 0; i1 < n1_unsigned; i1++) {
        r[i1].re = 0.0;
        r[i1].im = 0.0;
      }
    }
    if (n1_unsigned != 1) {
      FFTImplementationCallback::doHalfLengthRadix2(x, xoff, r, n1_unsigned,
                                                    costab, sintab);
    } else {
      r[0].re = x[xoff];
      r[0].im = 0.0;
    }
    xoff = y.size(0);
    for (i1 = 0; i1 < xoff; i1++) {
      y[i1 + y.size(0) * chan] = r[i1];
    }
  }
}

void FFTImplementationCallback::c_generate_twiddle_tables(
    int nRows, boolean_T useRadix2, array<double, 2U> &costab,
    array<double, 2U> &sintab, array<double, 2U> &sintabinv)
{
  array<double, 2U> costab1q;
  double e;
  int i;
  int n;
  int nd2;
  e = 6.2831853071795862 / static_cast<double>(nRows);
  n = nRows / 2 / 2;
  costab1q.set_size(1, n + 1);
  costab1q[0] = 1.0;
  nd2 = n / 2 - 1;
  for (int k{0}; k <= nd2; k++) {
    costab1q[k + 1] = std::cos(e * (static_cast<double>(k) + 1.0));
  }
  i = nd2 + 2;
  nd2 = n - 1;
  for (int k{i}; k <= nd2; k++) {
    costab1q[k] = std::sin(e * static_cast<double>(n - k));
  }
  costab1q[n] = 0.0;
  if (!useRadix2) {
    n = costab1q.size(1) - 1;
    nd2 = (costab1q.size(1) - 1) << 1;
    costab.set_size(1, nd2 + 1);
    sintab.set_size(1, nd2 + 1);
    costab[0] = 1.0;
    sintab[0] = 0.0;
    sintabinv.set_size(1, nd2 + 1);
    for (int k{0}; k < n; k++) {
      sintabinv[k + 1] = costab1q[(n - k) - 1];
    }
    i = costab1q.size(1);
    for (int k{i}; k <= nd2; k++) {
      sintabinv[k] = costab1q[k - n];
    }
    for (int k{0}; k < n; k++) {
      costab[k + 1] = costab1q[k + 1];
      sintab[k + 1] = -costab1q[(n - k) - 1];
    }
    i = costab1q.size(1);
    for (int k{i}; k <= nd2; k++) {
      costab[k] = -costab1q[nd2 - k];
      sintab[k] = -costab1q[k - n];
    }
  } else {
    n = costab1q.size(1) - 1;
    nd2 = (costab1q.size(1) - 1) << 1;
    costab.set_size(1, nd2 + 1);
    sintab.set_size(1, nd2 + 1);
    costab[0] = 1.0;
    sintab[0] = 0.0;
    for (int k{0}; k < n; k++) {
      costab[k + 1] = costab1q[k + 1];
      sintab[k + 1] = costab1q[(n - k) - 1];
    }
    i = costab1q.size(1);
    for (int k{i}; k <= nd2; k++) {
      costab[k] = -costab1q[nd2 - k];
      sintab[k] = costab1q[k - n];
    }
    sintabinv.set_size(1, 0);
  }
}

void FFTImplementationCallback::doHalfLengthBluestein(
    const array<double, 1U> &x, array<creal_T, 1U> &y, int nrowsx, int nRows,
    int nfft, const array<creal_T, 1U> &wwc, const array<double, 2U> &costab,
    const array<double, 2U> &sintab, const array<double, 2U> &costabinv,
    const array<double, 2U> &sintabinv)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> reconVar1;
  array<creal_T, 1U> reconVar2;
  array<creal_T, 1U> ytmp;
  array<double, 2U> a__1;
  array<double, 2U> costable;
  array<double, 2U> hcostabinv;
  array<double, 2U> hsintab;
  array<double, 2U> hsintabinv;
  array<double, 2U> sintable;
  array<int, 2U> wrapIndex;
  double a_im;
  double a_re;
  double b_im;
  double b_re;
  double z_tmp;
  int b_i;
  int b_re_tmp;
  int hnRows;
  int i;
  int u0;
  boolean_T nxeven;
  hnRows = nRows / 2;
  ytmp.set_size(hnRows);
  if (hnRows > nrowsx) {
    ytmp.set_size(hnRows);
    for (i = 0; i < hnRows; i++) {
      ytmp[i].re = 0.0;
      ytmp[i].im = 0.0;
    }
  }
  if ((x.size(0) & 1) == 0) {
    nxeven = true;
    u0 = x.size(0);
  } else if (x.size(0) >= nRows) {
    nxeven = true;
    u0 = nRows;
  } else {
    nxeven = false;
    u0 = x.size(0) - 1;
  }
  if (u0 > nRows) {
    u0 = nRows;
  }
  FFTImplementationCallback::b_generate_twiddle_tables(nRows << 1, costable,
                                                       sintable, a__1);
  FFTImplementationCallback::get_half_twiddle_tables(costab, sintab, costabinv,
                                                     sintabinv, a__1, hsintab,
                                                     hcostabinv, hsintabinv);
  reconVar1.set_size(hnRows);
  reconVar2.set_size(hnRows);
  wrapIndex.set_size(1, hnRows);
  for (b_i = 0; b_i < hnRows; b_i++) {
    i = b_i << 1;
    z_tmp = sintable[i];
    a_re = costable[i];
    reconVar1[b_i].re = z_tmp + 1.0;
    reconVar1[b_i].im = -a_re;
    reconVar2[b_i].re = 1.0 - z_tmp;
    reconVar2[b_i].im = a_re;
    if (b_i + 1 != 1) {
      wrapIndex[b_i] = (hnRows - b_i) + 1;
    } else {
      wrapIndex[0] = 1;
    }
  }
  z_tmp = static_cast<double>(u0) / 2.0;
  i = static_cast<int>(z_tmp);
  for (int k1{0}; k1 < i; k1++) {
    b_i = (hnRows + k1) - 1;
    a_re = wwc[b_i].re;
    a_im = wwc[b_i].im;
    b_re_tmp = k1 << 1;
    b_re = x[b_re_tmp];
    b_im = x[b_re_tmp + 1];
    ytmp[k1].re = a_re * b_re + a_im * b_im;
    ytmp[k1].im = a_re * b_im - a_im * b_re;
  }
  if (!nxeven) {
    b_i = (hnRows + static_cast<int>(z_tmp)) - 1;
    a_re = wwc[b_i].re;
    a_im = wwc[b_i].im;
    if (static_cast<int>(z_tmp) - 1 < 0) {
      i = 0;
    } else {
      i = static_cast<int>(z_tmp) << 1;
    }
    b_re = x[i];
    ytmp[static_cast<int>(static_cast<double>(u0) / 2.0)].re =
        a_re * b_re + a_im * 0.0;
    ytmp[static_cast<int>(static_cast<double>(u0) / 2.0)].im =
        a_re * 0.0 - a_im * b_re;
    if (static_cast<int>(z_tmp) + 2 <= hnRows) {
      i = static_cast<int>(z_tmp) + 2;
      for (b_i = i; b_i <= hnRows; b_i++) {
        ytmp[b_i - 1].re = 0.0;
        ytmp[b_i - 1].im = 0.0;
      }
    }
  } else if (static_cast<int>(z_tmp) + 1 <= hnRows) {
    i = static_cast<int>(z_tmp) + 1;
    for (b_i = i; b_i <= hnRows; b_i++) {
      ytmp[b_i - 1].re = 0.0;
      ytmp[b_i - 1].im = 0.0;
    }
  }
  b_i = static_cast<int>(static_cast<double>(nfft) / 2.0);
  FFTImplementationCallback::r2br_r2dit_trig_impl(ytmp, b_i, a__1, hsintab, fv);
  FFTImplementationCallback::r2br_r2dit_trig_impl(wwc, b_i, a__1, hsintab,
                                                  b_fv);
  b_fv.set_size(fv.size(0));
  b_re_tmp = fv.size(0);
  for (i = 0; i < b_re_tmp; i++) {
    z_tmp = fv[i].re;
    a_re = b_fv[i].im;
    a_im = fv[i].im;
    b_re = b_fv[i].re;
    b_fv[i].re = z_tmp * b_re - a_im * a_re;
    b_fv[i].im = z_tmp * a_re + a_im * b_re;
  }
  FFTImplementationCallback::r2br_r2dit_trig_impl(b_fv, b_i, hcostabinv,
                                                  hsintabinv, fv);
  if (fv.size(0) > 1) {
    z_tmp = 1.0 / static_cast<double>(fv.size(0));
    b_re_tmp = fv.size(0);
    for (i = 0; i < b_re_tmp; i++) {
      fv[i].re = z_tmp * fv[i].re;
      fv[i].im = z_tmp * fv[i].im;
    }
  }
  i = wwc.size(0);
  for (b_re_tmp = hnRows; b_re_tmp <= i; b_re_tmp++) {
    z_tmp = wwc[b_re_tmp - 1].re;
    a_re = fv[b_re_tmp - 1].im;
    a_im = wwc[b_re_tmp - 1].im;
    b_re = fv[b_re_tmp - 1].re;
    b_i = b_re_tmp - hnRows;
    ytmp[b_i].re = z_tmp * b_re + a_im * a_re;
    ytmp[b_i].im = z_tmp * a_re - a_im * b_re;
  }
  for (b_i = 0; b_i < hnRows; b_i++) {
    double b_ytmp_re_tmp;
    double ytmp_im_tmp;
    double ytmp_re_tmp;
    i = wrapIndex[b_i];
    z_tmp = ytmp[b_i].re;
    a_re = reconVar1[b_i].im;
    a_im = ytmp[b_i].im;
    b_re = reconVar1[b_i].re;
    b_im = ytmp[i - 1].re;
    ytmp_im_tmp = -ytmp[i - 1].im;
    ytmp_re_tmp = reconVar2[b_i].im;
    b_ytmp_re_tmp = reconVar2[b_i].re;
    y[b_i].re = 0.5 * ((z_tmp * b_re - a_im * a_re) +
                       (b_im * b_ytmp_re_tmp - ytmp_im_tmp * ytmp_re_tmp));
    y[b_i].im = 0.5 * ((z_tmp * a_re + a_im * b_re) +
                       (b_im * ytmp_re_tmp + ytmp_im_tmp * b_ytmp_re_tmp));
    i = hnRows + b_i;
    y[i].re = 0.5 * ((z_tmp * b_ytmp_re_tmp - a_im * ytmp_re_tmp) +
                     (b_im * b_re - ytmp_im_tmp * a_re));
    y[i].im = 0.5 * ((z_tmp * ytmp_re_tmp + a_im * b_ytmp_re_tmp) +
                     (b_im * a_re + ytmp_im_tmp * b_re));
  }
}

void FFTImplementationCallback::doHalfLengthRadix2(
    const array<double, 1U> &x, array<creal_T, 1U> &y, int unsigned_nRows,
    const array<double, 2U> &costab, const array<double, 2U> &sintab)
{
  array<creal_T, 1U> reconVar1;
  array<creal_T, 1U> reconVar2;
  array<double, 2U> hcostab;
  array<double, 2U> hsintab;
  array<int, 2U> wrapIndex;
  array<int, 1U> bitrevIndex;
  double im;
  double re;
  double temp_im;
  double temp_re;
  double twid_im;
  double twid_re;
  int bitrevIndex_tmp;
  int hszCostab;
  int i;
  int iDelta;
  int iDelta2;
  int iheight;
  int j;
  int k;
  int nRows;
  int nRowsD2;
  int nRowsM2;
  boolean_T nxeven;
  nRows = unsigned_nRows / 2;
  nRowsM2 = nRows - 2;
  nRowsD2 = nRows / 2;
  k = nRowsD2 / 2;
  hszCostab = static_cast<int>(static_cast<unsigned int>(costab.size(1)) >> 1);
  hcostab.set_size(1, hszCostab);
  hsintab.set_size(1, hszCostab);
  for (i = 0; i < hszCostab; i++) {
    j = ((i + 1) << 1) - 2;
    hcostab[i] = costab[j];
    hsintab[i] = sintab[j];
  }
  reconVar1.set_size(nRows);
  reconVar2.set_size(nRows);
  wrapIndex.set_size(1, nRows);
  for (i = 0; i < nRows; i++) {
    re = sintab[i];
    im = costab[i];
    reconVar1[i].re = re + 1.0;
    reconVar1[i].im = -im;
    reconVar2[i].re = 1.0 - re;
    reconVar2[i].im = im;
    if (i + 1 != 1) {
      wrapIndex[i] = (nRows - i) + 1;
    } else {
      wrapIndex[0] = 1;
    }
  }
  bitrevIndex_tmp = static_cast<int>(static_cast<double>(unsigned_nRows) / 2.0);
  hszCostab = y.size(0);
  if (hszCostab > nRows) {
    hszCostab = nRows;
  }
  FFTImplementationCallback::get_bitrevIndex(hszCostab - 1, bitrevIndex_tmp,
                                             bitrevIndex);
  if ((x.size(0) & 1) == 0) {
    nxeven = true;
    hszCostab = x.size(0);
  } else if (x.size(0) >= unsigned_nRows) {
    nxeven = true;
    hszCostab = unsigned_nRows;
  } else {
    nxeven = false;
    hszCostab = x.size(0) - 1;
  }
  if (hszCostab > unsigned_nRows) {
    hszCostab = unsigned_nRows;
  }
  re = static_cast<double>(hszCostab) / 2.0;
  hszCostab = static_cast<int>(re);
  for (i = 0; i < hszCostab; i++) {
    j = i << 1;
    y[bitrevIndex[i] - 1].re = x[j];
    y[bitrevIndex[i] - 1].im = x[j + 1];
  }
  if (!nxeven) {
    hszCostab = bitrevIndex[static_cast<int>(re)] - 1;
    if (static_cast<int>(re) - 1 < 0) {
      j = 0;
    } else {
      j = static_cast<int>(re) << 1;
    }
    y[hszCostab].re = x[j];
    y[hszCostab].im = 0.0;
  }
  if (nRows > 1) {
    for (i = 0; i <= nRowsM2; i += 2) {
      re = y[i + 1].re;
      im = y[i + 1].im;
      temp_re = re;
      temp_im = im;
      twid_re = y[i].re;
      twid_im = y[i].im;
      re = twid_re - re;
      im = twid_im - im;
      y[i + 1].re = re;
      y[i + 1].im = im;
      twid_re += temp_re;
      twid_im += temp_im;
      y[i].re = twid_re;
      y[i].im = twid_im;
    }
  }
  iDelta = 2;
  iDelta2 = 4;
  iheight = ((k - 1) << 2) + 1;
  while (k > 0) {
    for (i = 0; i < iheight; i += iDelta2) {
      hszCostab = i + iDelta;
      temp_re = y[hszCostab].re;
      temp_im = y[hszCostab].im;
      y[hszCostab].re = y[i].re - temp_re;
      y[hszCostab].im = y[i].im - temp_im;
      y[i].re = y[i].re + temp_re;
      y[i].im = y[i].im + temp_im;
    }
    hszCostab = 1;
    for (j = k; j < nRowsD2; j += k) {
      twid_re = hcostab[j];
      twid_im = hsintab[j];
      i = hszCostab;
      nRows = hszCostab + iheight;
      while (i < nRows) {
        nRowsM2 = i + iDelta;
        re = y[nRowsM2].im;
        im = y[nRowsM2].re;
        temp_re = twid_re * im - twid_im * re;
        temp_im = twid_re * re + twid_im * im;
        y[nRowsM2].re = y[i].re - temp_re;
        y[nRowsM2].im = y[i].im - temp_im;
        y[i].re = y[i].re + temp_re;
        y[i].im = y[i].im + temp_im;
        i += iDelta2;
      }
      hszCostab++;
    }
    k /= 2;
    iDelta = iDelta2;
    iDelta2 += iDelta2;
    iheight -= iDelta;
  }
  FFTImplementationCallback::getback_radix2_fft(y, reconVar1, reconVar2,
                                                wrapIndex, bitrevIndex_tmp);
}

void FFTImplementationCallback::dobluesteinfft(
    const array<double, 2U> &x, int n2blue, int nfft,
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    const array<double, 2U> &sintabinv, array<creal_T, 2U> &y)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> r;
  array<creal_T, 1U> wwc;
  double b_re_tmp;
  double c_re_tmp;
  double d_re_tmp;
  double re_tmp;
  int a_re_tmp;
  int b_k;
  int b_y;
  int i;
  int minNrowsNx;
  int xoff;
  if ((nfft != 1) && ((nfft & 1) == 0)) {
    int nInt2;
    int nInt2m1;
    int nRows;
    int rt;
    nRows = nfft / 2;
    nInt2m1 = (nRows + nRows) - 1;
    wwc.set_size(nInt2m1);
    rt = 0;
    wwc[nRows - 1].re = 1.0;
    wwc[nRows - 1].im = 0.0;
    nInt2 = nRows << 1;
    for (int k{0}; k <= nRows - 2; k++) {
      double nt_im;
      b_y = ((k + 1) << 1) - 1;
      if (nInt2 - rt <= b_y) {
        rt += b_y - nInt2;
      } else {
        rt += b_y;
      }
      nt_im = -3.1415926535897931 * static_cast<double>(rt) /
              static_cast<double>(nRows);
      i = (nRows - k) - 2;
      wwc[i].re = std::cos(nt_im);
      wwc[i].im = -std::sin(nt_im);
    }
    i = nInt2m1 - 1;
    for (int k{i}; k >= nRows; k--) {
      wwc[k] = wwc[(nInt2m1 - k) - 1];
    }
  } else {
    int nInt2;
    int nInt2m1;
    int rt;
    nInt2m1 = (nfft + nfft) - 1;
    wwc.set_size(nInt2m1);
    rt = 0;
    wwc[nfft - 1].re = 1.0;
    wwc[nfft - 1].im = 0.0;
    nInt2 = nfft << 1;
    for (int k{0}; k <= nfft - 2; k++) {
      double nt_im;
      b_y = ((k + 1) << 1) - 1;
      if (nInt2 - rt <= b_y) {
        rt += b_y - nInt2;
      } else {
        rt += b_y;
      }
      nt_im = -3.1415926535897931 * static_cast<double>(rt) /
              static_cast<double>(nfft);
      i = (nfft - k) - 2;
      wwc[i].re = std::cos(nt_im);
      wwc[i].im = -std::sin(nt_im);
    }
    i = nInt2m1 - 1;
    for (int k{i}; k >= nfft; k--) {
      wwc[k] = wwc[(nInt2m1 - k) - 1];
    }
  }
  y.set_size(nfft, x.size(1));
  if (nfft > 120) {
    y.set_size(nfft, x.size(1));
    b_y = nfft * x.size(1);
    for (i = 0; i < b_y; i++) {
      y[i].re = 0.0;
      y[i].im = 0.0;
    }
  }
  b_y = x.size(1) - 1;
#pragma omp parallel for num_threads(omp_get_max_threads()) private(           \
    fv, b_fv, r, xoff, a_re_tmp, minNrowsNx, b_k, re_tmp, b_re_tmp, c_re_tmp,  \
    d_re_tmp)

  for (int chan = 0; chan <= b_y; chan++) {
    xoff = chan * 120;
    r.set_size(nfft);
    if (nfft > 120) {
      r.set_size(nfft);
      for (a_re_tmp = 0; a_re_tmp < nfft; a_re_tmp++) {
        r[a_re_tmp].re = 0.0;
        r[a_re_tmp].im = 0.0;
      }
    }
    if ((n2blue != 1) && ((nfft & 1) == 0)) {
      FFTImplementationCallback::doHalfLengthBluestein(
          x, xoff, r, nfft, n2blue, wwc, costab, sintab, costab, sintabinv);
    } else {
      if (nfft <= 120) {
        minNrowsNx = nfft - 1;
      } else {
        minNrowsNx = 119;
      }
      for (b_k = 0; b_k <= minNrowsNx; b_k++) {
        a_re_tmp = (nfft + b_k) - 1;
        re_tmp = x[xoff + b_k];
        r[b_k].re = wwc[a_re_tmp].re * re_tmp;
        r[b_k].im = wwc[a_re_tmp].im * -re_tmp;
      }
      a_re_tmp = minNrowsNx + 2;
      for (b_k = a_re_tmp; b_k <= nfft; b_k++) {
        r[b_k - 1].re = 0.0;
        r[b_k - 1].im = 0.0;
      }
      FFTImplementationCallback::r2br_r2dit_trig_impl(r, n2blue, costab, sintab,
                                                      b_fv);
      FFTImplementationCallback::r2br_r2dit_trig_impl(wwc, n2blue, costab,
                                                      sintab, fv);
      fv.set_size(b_fv.size(0));
      minNrowsNx = b_fv.size(0);
      for (a_re_tmp = 0; a_re_tmp < minNrowsNx; a_re_tmp++) {
        re_tmp = b_fv[a_re_tmp].re;
        b_re_tmp = fv[a_re_tmp].im;
        c_re_tmp = b_fv[a_re_tmp].im;
        d_re_tmp = fv[a_re_tmp].re;
        fv[a_re_tmp].re = re_tmp * d_re_tmp - c_re_tmp * b_re_tmp;
        fv[a_re_tmp].im = re_tmp * b_re_tmp + c_re_tmp * d_re_tmp;
      }
      FFTImplementationCallback::r2br_r2dit_trig_impl(fv, n2blue, costab,
                                                      sintabinv, b_fv);
      if (b_fv.size(0) > 1) {
        re_tmp = 1.0 / static_cast<double>(b_fv.size(0));
        minNrowsNx = b_fv.size(0);
        for (a_re_tmp = 0; a_re_tmp < minNrowsNx; a_re_tmp++) {
          b_fv[a_re_tmp].re = re_tmp * b_fv[a_re_tmp].re;
          b_fv[a_re_tmp].im = re_tmp * b_fv[a_re_tmp].im;
        }
      }
      a_re_tmp = wwc.size(0);
      for (b_k = nfft; b_k <= a_re_tmp; b_k++) {
        re_tmp = wwc[b_k - 1].re;
        b_re_tmp = b_fv[b_k - 1].im;
        c_re_tmp = wwc[b_k - 1].im;
        d_re_tmp = b_fv[b_k - 1].re;
        minNrowsNx = b_k - nfft;
        r[minNrowsNx].re = re_tmp * d_re_tmp + c_re_tmp * b_re_tmp;
        r[minNrowsNx].im = re_tmp * b_re_tmp - c_re_tmp * d_re_tmp;
      }
    }
    minNrowsNx = y.size(0);
    for (a_re_tmp = 0; a_re_tmp < minNrowsNx; a_re_tmp++) {
      y[a_re_tmp + y.size(0) * chan] = r[a_re_tmp];
    }
  }
}

void FFTImplementationCallback::dobluesteinfft(
    const array<creal_T, 2U> &x, int n2blue, int nfft,
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    const array<double, 2U> &sintabinv, array<creal_T, 2U> &y)
{
  array<creal_T, 1U> b_fv;
  array<creal_T, 1U> fv;
  array<creal_T, 1U> r;
  array<creal_T, 1U> wwc;
  double ar;
  double b_re_tmp;
  double denom_im;
  double denom_re;
  double re_tmp;
  int b_k;
  int b_y;
  int i;
  int i1;
  int minNrowsNx;
  int nInt2;
  int nInt2m1;
  int rt;
  int u0;
  int xoff;
  nInt2m1 = (nfft + nfft) - 1;
  wwc.set_size(nInt2m1);
  rt = 0;
  wwc[nfft - 1].re = 1.0;
  wwc[nfft - 1].im = 0.0;
  nInt2 = nfft << 1;
  for (int k{0}; k <= nfft - 2; k++) {
    double nt_im;
    b_y = ((k + 1) << 1) - 1;
    if (nInt2 - rt <= b_y) {
      rt += b_y - nInt2;
    } else {
      rt += b_y;
    }
    nt_im = 3.1415926535897931 * static_cast<double>(rt) /
            static_cast<double>(nfft);
    i = (nfft - k) - 2;
    wwc[i].re = std::cos(nt_im);
    wwc[i].im = -std::sin(nt_im);
  }
  i = nInt2m1 - 1;
  for (int k{i}; k >= nfft; k--) {
    wwc[k] = wwc[(nInt2m1 - k) - 1];
  }
  nInt2m1 = x.size(0);
  y.set_size(nfft, x.size(1));
  if (nfft > x.size(0)) {
    y.set_size(nfft, x.size(1));
    b_y = nfft * x.size(1);
    for (i = 0; i < b_y; i++) {
      y[i].re = 0.0;
      y[i].im = 0.0;
    }
  }
  b_y = x.size(1) - 1;
#pragma omp parallel for num_threads(omp_get_max_threads()) private(           \
    fv, b_fv, r, xoff, i1, minNrowsNx, b_k, u0, denom_re, denom_im, re_tmp,    \
    b_re_tmp, ar)

  for (int chan = 0; chan <= b_y; chan++) {
    xoff = chan * nInt2m1;
    r.set_size(nfft);
    if (nfft > x.size(0)) {
      r.set_size(nfft);
      for (i1 = 0; i1 < nfft; i1++) {
        r[i1].re = 0.0;
        r[i1].im = 0.0;
      }
    }
    minNrowsNx = x.size(0);
    if (nfft <= minNrowsNx) {
      minNrowsNx = nfft;
    }
    for (b_k = 0; b_k < minNrowsNx; b_k++) {
      u0 = (nfft + b_k) - 1;
      denom_re = wwc[u0].re;
      denom_im = wwc[u0].im;
      i1 = xoff + b_k;
      re_tmp = x[i1].im;
      b_re_tmp = x[i1].re;
      r[b_k].re = denom_re * b_re_tmp + denom_im * re_tmp;
      r[b_k].im = denom_re * re_tmp - denom_im * b_re_tmp;
    }
    i1 = minNrowsNx + 1;
    for (b_k = i1; b_k <= nfft; b_k++) {
      r[b_k - 1].re = 0.0;
      r[b_k - 1].im = 0.0;
    }
    FFTImplementationCallback::r2br_r2dit_trig_impl(r, n2blue, costab, sintab,
                                                    b_fv);
    FFTImplementationCallback::r2br_r2dit_trig_impl(wwc, n2blue, costab, sintab,
                                                    fv);
    fv.set_size(b_fv.size(0));
    u0 = b_fv.size(0);
    for (i1 = 0; i1 < u0; i1++) {
      denom_re = b_fv[i1].re;
      denom_im = fv[i1].im;
      re_tmp = b_fv[i1].im;
      b_re_tmp = fv[i1].re;
      fv[i1].re = denom_re * b_re_tmp - re_tmp * denom_im;
      fv[i1].im = denom_re * denom_im + re_tmp * b_re_tmp;
    }
    FFTImplementationCallback::r2br_r2dit_trig_impl(fv, n2blue, costab,
                                                    sintabinv, b_fv);
    if (b_fv.size(0) > 1) {
      denom_re = 1.0 / static_cast<double>(b_fv.size(0));
      u0 = b_fv.size(0);
      for (i1 = 0; i1 < u0; i1++) {
        b_fv[i1].re = denom_re * b_fv[i1].re;
        b_fv[i1].im = denom_re * b_fv[i1].im;
      }
    }
    i1 = wwc.size(0);
    for (b_k = nfft; b_k <= i1; b_k++) {
      denom_re = wwc[b_k - 1].re;
      denom_im = b_fv[b_k - 1].im;
      re_tmp = wwc[b_k - 1].im;
      b_re_tmp = b_fv[b_k - 1].re;
      ar = denom_re * b_re_tmp + re_tmp * denom_im;
      denom_re = denom_re * denom_im - re_tmp * b_re_tmp;
      if (denom_re == 0.0) {
        u0 = b_k - nfft;
        r[u0].re = ar / static_cast<double>(nfft);
        r[u0].im = 0.0;
      } else if (ar == 0.0) {
        u0 = b_k - nfft;
        r[u0].re = 0.0;
        r[u0].im = denom_re / static_cast<double>(nfft);
      } else {
        u0 = b_k - nfft;
        r[u0].re = ar / static_cast<double>(nfft);
        r[u0].im = denom_re / static_cast<double>(nfft);
      }
    }
    u0 = y.size(0);
    for (i1 = 0; i1 < u0; i1++) {
      y[i1 + y.size(0) * chan] = r[i1];
    }
  }
}

void FFTImplementationCallback::generate_twiddle_tables(
    int nRows, boolean_T useRadix2, array<double, 2U> &costab,
    array<double, 2U> &sintab, array<double, 2U> &sintabinv)
{
  array<double, 2U> costab1q;
  double e;
  int i;
  int n;
  int nd2;
  e = 6.2831853071795862 / static_cast<double>(nRows);
  n = nRows / 2 / 2;
  costab1q.set_size(1, n + 1);
  costab1q[0] = 1.0;
  nd2 = n / 2 - 1;
  for (int k{0}; k <= nd2; k++) {
    costab1q[k + 1] = std::cos(e * (static_cast<double>(k) + 1.0));
  }
  i = nd2 + 2;
  nd2 = n - 1;
  for (int k{i}; k <= nd2; k++) {
    costab1q[k] = std::sin(e * static_cast<double>(n - k));
  }
  costab1q[n] = 0.0;
  if (!useRadix2) {
    n = costab1q.size(1) - 1;
    nd2 = (costab1q.size(1) - 1) << 1;
    costab.set_size(1, nd2 + 1);
    sintab.set_size(1, nd2 + 1);
    costab[0] = 1.0;
    sintab[0] = 0.0;
    sintabinv.set_size(1, nd2 + 1);
    for (int k{0}; k < n; k++) {
      sintabinv[k + 1] = costab1q[(n - k) - 1];
    }
    i = costab1q.size(1);
    for (int k{i}; k <= nd2; k++) {
      sintabinv[k] = costab1q[k - n];
    }
    for (int k{0}; k < n; k++) {
      costab[k + 1] = costab1q[k + 1];
      sintab[k + 1] = -costab1q[(n - k) - 1];
    }
    i = costab1q.size(1);
    for (int k{i}; k <= nd2; k++) {
      costab[k] = -costab1q[nd2 - k];
      sintab[k] = -costab1q[k - n];
    }
  } else {
    n = costab1q.size(1) - 1;
    nd2 = (costab1q.size(1) - 1) << 1;
    costab.set_size(1, nd2 + 1);
    sintab.set_size(1, nd2 + 1);
    costab[0] = 1.0;
    sintab[0] = 0.0;
    for (int k{0}; k < n; k++) {
      costab[k + 1] = costab1q[k + 1];
      sintab[k + 1] = -costab1q[(n - k) - 1];
    }
    i = costab1q.size(1);
    for (int k{i}; k <= nd2; k++) {
      costab[k] = -costab1q[nd2 - k];
      sintab[k] = -costab1q[k - n];
    }
    sintabinv.set_size(1, 0);
  }
}

int FFTImplementationCallback::get_algo_sizes(int nfft, boolean_T useRadix2,
                                              int &nRows)
{
  int n2blue;
  n2blue = 1;
  if (useRadix2) {
    nRows = nfft;
  } else {
    if (nfft > 0) {
      int n;
      int pmax;
      n = (nfft + nfft) - 1;
      pmax = 31;
      if (n <= 1) {
        pmax = 0;
      } else {
        int pmin;
        boolean_T exitg1;
        pmin = 0;
        exitg1 = false;
        while ((!exitg1) && (pmax - pmin > 1)) {
          int k;
          int pow2p;
          k = (pmin + pmax) >> 1;
          pow2p = 1 << k;
          if (pow2p == n) {
            pmax = k;
            exitg1 = true;
          } else if (pow2p > n) {
            pmax = k;
          } else {
            pmin = k;
          }
        }
      }
      n2blue = 1 << pmax;
    }
    nRows = n2blue;
  }
  return n2blue;
}

void FFTImplementationCallback::r2br_r2dit_trig(const array<double, 2U> &x,
                                                int n1_unsigned,
                                                const array<double, 2U> &costab,
                                                const array<double, 2U> &sintab,
                                                array<creal_T, 2U> &y)
{
  array<creal_T, 1U> r;
  int i1;
  int loop_ub;
  int xoff;
  y.set_size(n1_unsigned, x.size(1));
  if (n1_unsigned > 120) {
    y.set_size(n1_unsigned, x.size(1));
    loop_ub = n1_unsigned * x.size(1);
    for (int i{0}; i < loop_ub; i++) {
      y[i].re = 0.0;
      y[i].im = 0.0;
    }
  }
  loop_ub = x.size(1) - 1;
#pragma omp parallel for num_threads(omp_get_max_threads()) private(r, xoff, i1)

  for (int chan = 0; chan <= loop_ub; chan++) {
    xoff = chan * 120;
    r.set_size(n1_unsigned);
    if (n1_unsigned > 120) {
      r.set_size(n1_unsigned);
      for (i1 = 0; i1 < n1_unsigned; i1++) {
        r[i1].re = 0.0;
        r[i1].im = 0.0;
      }
    }
    if (n1_unsigned != 1) {
      FFTImplementationCallback::b_doHalfLengthRadix2(x, xoff, r, n1_unsigned,
                                                      costab, sintab);
    } else {
      r[0].re = x[xoff];
      r[0].im = 0.0;
    }
    xoff = y.size(0);
    for (i1 = 0; i1 < xoff; i1++) {
      y[i1 + y.size(0) * chan] = r[i1];
    }
  }
}

void FFTImplementationCallback::r2br_r2dit_trig(const array<creal_T, 2U> &x,
                                                int n1_unsigned,
                                                const array<double, 2U> &costab,
                                                const array<double, 2U> &sintab,
                                                array<creal_T, 2U> &y)
{
  array<creal_T, 1U> r;
  double temp_im;
  double temp_re;
  double temp_re_tmp;
  double twid_im;
  double twid_re;
  int b_i;
  int iDelta;
  int iheight;
  int ihi;
  int iy;
  int j;
  int ju;
  int k;
  int loop_ub;
  int nRowsD2;
  int nrows;
  int xoff;
  boolean_T tst;
  nrows = x.size(0);
  y.set_size(n1_unsigned, x.size(1));
  if (n1_unsigned > x.size(0)) {
    y.set_size(n1_unsigned, x.size(1));
    loop_ub = n1_unsigned * x.size(1);
    for (int i{0}; i < loop_ub; i++) {
      y[i].re = 0.0;
      y[i].im = 0.0;
    }
  }
  loop_ub = x.size(1) - 1;
#pragma omp parallel for num_threads(omp_get_max_threads()) private(           \
    r, xoff, j, iy, iDelta, ihi, nRowsD2, k, ju, b_i, tst, temp_re_tmp,        \
    temp_im, temp_re, twid_re, iheight, twid_im)

  for (int chan = 0; chan <= loop_ub; chan++) {
    xoff = chan * nrows;
    r.set_size(n1_unsigned);
    if (n1_unsigned > x.size(0)) {
      r.set_size(n1_unsigned);
      for (j = 0; j < n1_unsigned; j++) {
        r[j].re = 0.0;
        r[j].im = 0.0;
      }
    }
    iy = x.size(0);
    iDelta = n1_unsigned;
    if (iy <= n1_unsigned) {
      iDelta = iy;
    }
    ihi = n1_unsigned - 2;
    nRowsD2 = n1_unsigned / 2;
    k = nRowsD2 / 2;
    iy = 0;
    ju = 0;
    for (b_i = 0; b_i <= iDelta - 2; b_i++) {
      r[iy] = x[xoff + b_i];
      j = n1_unsigned;
      tst = true;
      while (tst) {
        j >>= 1;
        ju ^= j;
        tst = ((ju & j) == 0);
      }
      iy = ju;
    }
    if (iDelta - 2 >= 0) {
      xoff = (xoff + iDelta) - 1;
    }
    r[iy] = x[xoff];
    if (n1_unsigned > 1) {
      for (b_i = 0; b_i <= ihi; b_i += 2) {
        temp_re_tmp = r[b_i + 1].re;
        temp_im = r[b_i + 1].im;
        temp_re = r[b_i].re;
        twid_re = r[b_i].im;
        r[b_i + 1].re = temp_re - temp_re_tmp;
        r[b_i + 1].im = twid_re - temp_im;
        r[b_i].re = temp_re + temp_re_tmp;
        r[b_i].im = twid_re + temp_im;
      }
    }
    iDelta = 2;
    xoff = 4;
    iheight = ((k - 1) << 2) + 1;
    while (k > 0) {
      for (b_i = 0; b_i < iheight; b_i += xoff) {
        iy = b_i + iDelta;
        temp_re = r[iy].re;
        temp_im = r[iy].im;
        r[iy].re = r[b_i].re - temp_re;
        r[iy].im = r[b_i].im - temp_im;
        r[b_i].re = r[b_i].re + temp_re;
        r[b_i].im = r[b_i].im + temp_im;
      }
      iy = 1;
      for (j = k; j < nRowsD2; j += k) {
        twid_re = costab[j];
        twid_im = sintab[j];
        b_i = iy;
        ihi = iy + iheight;
        while (b_i < ihi) {
          ju = b_i + iDelta;
          temp_re_tmp = r[ju].im;
          temp_im = r[ju].re;
          temp_re = twid_re * temp_im - twid_im * temp_re_tmp;
          temp_im = twid_re * temp_re_tmp + twid_im * temp_im;
          r[ju].re = r[b_i].re - temp_re;
          r[ju].im = r[b_i].im - temp_im;
          r[b_i].re = r[b_i].re + temp_re;
          r[b_i].im = r[b_i].im + temp_im;
          b_i += xoff;
        }
        iy++;
      }
      k /= 2;
      iDelta = xoff;
      xoff += xoff;
      iheight -= iDelta;
    }
    iy = y.size(0);
    for (j = 0; j < iy; j++) {
      y[j + y.size(0) * chan] = r[j];
    }
  }
  if (y.size(0) > 1) {
    double b;
    b = 1.0 / static_cast<double>(y.size(0));
    loop_ub = y.size(0) * y.size(1);
    for (int i{0}; i < loop_ub; i++) {
      y[i].re = b * y[i].re;
      y[i].im = b * y[i].im;
    }
  }
}

void FFTImplementationCallback::r2br_r2dit_trig_impl(
    const array<creal_T, 1U> &x, int unsigned_nRows,
    const array<double, 2U> &costab, const array<double, 2U> &sintab,
    array<creal_T, 1U> &y)
{
  double temp_im;
  double temp_re;
  double temp_re_tmp;
  double twid_re;
  int i;
  int iDelta;
  int iDelta2;
  int iheight;
  int ihi;
  int iy;
  int j;
  int ju;
  int k;
  int nRowsD2;
  y.set_size(unsigned_nRows);
  if (unsigned_nRows > x.size(0)) {
    y.set_size(unsigned_nRows);
    for (iy = 0; iy < unsigned_nRows; iy++) {
      y[iy].re = 0.0;
      y[iy].im = 0.0;
    }
  }
  j = x.size(0);
  if (j > unsigned_nRows) {
    j = unsigned_nRows;
  }
  ihi = unsigned_nRows - 2;
  nRowsD2 = unsigned_nRows / 2;
  k = nRowsD2 / 2;
  iy = 0;
  ju = 0;
  for (i = 0; i <= j - 2; i++) {
    boolean_T tst;
    y[iy] = x[i];
    iy = unsigned_nRows;
    tst = true;
    while (tst) {
      iy >>= 1;
      ju ^= iy;
      tst = ((ju & iy) == 0);
    }
    iy = ju;
  }
  if (j - 2 < 0) {
    j = 0;
  } else {
    j--;
  }
  y[iy] = x[j];
  if (unsigned_nRows > 1) {
    for (i = 0; i <= ihi; i += 2) {
      temp_re_tmp = y[i + 1].re;
      temp_im = y[i + 1].im;
      temp_re = y[i].re;
      twid_re = y[i].im;
      y[i + 1].re = temp_re - temp_re_tmp;
      y[i + 1].im = twid_re - temp_im;
      y[i].re = temp_re + temp_re_tmp;
      y[i].im = twid_re + temp_im;
    }
  }
  iDelta = 2;
  iDelta2 = 4;
  iheight = ((k - 1) << 2) + 1;
  while (k > 0) {
    for (i = 0; i < iheight; i += iDelta2) {
      iy = i + iDelta;
      temp_re = y[iy].re;
      temp_im = y[iy].im;
      y[iy].re = y[i].re - temp_re;
      y[iy].im = y[i].im - temp_im;
      y[i].re = y[i].re + temp_re;
      y[i].im = y[i].im + temp_im;
    }
    iy = 1;
    for (j = k; j < nRowsD2; j += k) {
      double twid_im;
      twid_re = costab[j];
      twid_im = sintab[j];
      i = iy;
      ihi = iy + iheight;
      while (i < ihi) {
        ju = i + iDelta;
        temp_re_tmp = y[ju].im;
        temp_im = y[ju].re;
        temp_re = twid_re * temp_im - twid_im * temp_re_tmp;
        temp_im = twid_re * temp_re_tmp + twid_im * temp_im;
        y[ju].re = y[i].re - temp_re;
        y[ju].im = y[i].im - temp_im;
        y[i].re = y[i].re + temp_re;
        y[i].im = y[i].im + temp_im;
        i += iDelta2;
      }
      iy++;
    }
    k /= 2;
    iDelta = iDelta2;
    iDelta2 += iDelta2;
    iheight -= iDelta;
  }
}

} // namespace fft
} // namespace internal
} // namespace coder

// End of code generation (FFTImplementationCallback.cpp)
