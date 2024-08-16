//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
//
// TIME_entrypoint.cpp
//
// Code generation for function 'TIME_entrypoint'
//

// Include files
#include "TIME_entrypoint.h"
#include "TIME_entrypoint_data.h"
#include "TIME_entrypoint_initialize.h"
#include "ZERO_TIME_WIND_SPECTRUM.h"
#include "diff.h"
#include "fft.h"
#include "hilbert.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
static int div_s32(int numerator, int denominator);

static double rt_hypotd_snf(double u0, double u1);

// Function Definitions
static int div_s32(int numerator, int denominator)
{
  int quotient;
  if (denominator == 0) {
    if (numerator >= 0) {
      quotient = MAX_int32_T;
    } else {
      quotient = MIN_int32_T;
    }
  } else {
    unsigned int tempAbsQuotient;
    unsigned int u;
    if (numerator < 0) {
      tempAbsQuotient = ~static_cast<unsigned int>(numerator) + 1U;
    } else {
      tempAbsQuotient = static_cast<unsigned int>(numerator);
    }
    if (denominator < 0) {
      u = ~static_cast<unsigned int>(denominator) + 1U;
    } else {
      u = static_cast<unsigned int>(denominator);
    }
    tempAbsQuotient /= u;
    if ((numerator < 0) != (denominator < 0)) {
      quotient = -static_cast<int>(tempAbsQuotient);
    } else {
      quotient = static_cast<int>(tempAbsQuotient);
    }
  }
  return quotient;
}

static double rt_hypotd_snf(double u0, double u1)
{
  double a;
  double b;
  double y;
  a = std::abs(u0);
  b = std::abs(u1);
  if (a < b) {
    a /= b;
    y = b * std::sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    y = a * std::sqrt(b * b + 1.0);
  } else if (std::isnan(b)) {
    y = rtNaN;
  } else {
    y = a * 1.4142135623730951;
  }
  return y;
}

void TIME_entrypoint(const coder::array<double, 1U> &wav, double, double N,
                     double M, coder::array<double, 2U> &ZTW_SPEC,
                     coder::array<double, 2U> &ZTW_HNGD_SPEC)
{
  static const double b_a[120]{0.0,
                               1.0,
                               0.25004284171844887,
                               0.11116189518316938,
                               0.062553574179307167,
                               0.040054876889276718,
                               0.02783337429781289,
                               0.020464206199607423,
                               0.015681345570728152,
                               0.012402245283937024,
                               0.010056737614528938,
                               0.0083213408982654551,
                               0.0070014432668753408,
                               0.0059742666078265423,
                               0.0051592475887892149,
                               0.0045017461892694,
                               0.0039636439578399152,
                               0.0035176926007095479,
                               0.0031439957473688405,
                               0.00282775094710231,
                               0.002557761203410276,
                               0.0023254302839081382,
                               0.0021240701097044513,
                               0.0019484141598901839,
                               0.001794269722173484,
                               0.001658265490115665,
                               0.001537665759539536,
                               0.0014302318724291419,
                               0.0013341176601936093,
                               0.001247789675369491,
                               0.0011699657159388666,
                               0.0010995670005601687,
                               0.0010356806372195989,
                               0.00097752992904596449,
                               0.00092445070127607752,
                               0.00087587229340523834,
                               0.00083130219466542128,
                               0.00079031354605373853,
                               0.00075253491360206587,
                               0.00071764187313672274,
                               0.00068535004888544,
                               0.000655409325807715,
                               0.00062759901481432084,
                               0.00060172379570909147,
                               0.00057761029809650472,
                               0.00055510420813034591,
                               0.00053406781067105269,
                               0.00051437789354689034,
                               0.00049592395421178478,
                               0.00047860665994443039,
                               0.00046233652143708821,
                               0.00044703274663686853,
                               0.00043262224738110423,
                               0.00041903877598614052,
                               0.00040622217271958909,
                               0.00039411770817764097,
                               0.000382675507133561,
                               0.00037185004252564263,
                               0.00036159968999580224,
                               0.00035188633484007546,
                               0.00034267502444272,
                               0.00033393366027932917,
                               0.00032563272442589575,
                               0.00031774503622824081,
                               0.00031024553539249228,
                               0.000303111088271027,
                               0.00029632031455478861,
                               0.00028985343195474467,
                               0.00028369211677281477,
                               0.00027781937853450973,
                               0.00027221944708884807,
                               0.00026687767078183665,
                               0.00026178042448283607,
                               0.00025691502639263355,
                               0.00025226966269148714,
                               0.00024783331919770807,
                               0.00024359571930497825,
                               0.00023954726755163531,
                               0.00023567899824937185,
                               0.00023198252866367272,
                               0.00022845001629514665,
                               0.00022507411986076486,
                               0.000221847963617847,
                               0.00021876510471221004,
                               0.00021581950326592315,
                               0.00021300549495015421,
                               0.00021031776581518174,
                               0.00020775132917319011,
                               0.00020530150435037068,
                               0.00020296389714341429,
                               0.00020073438183201351,
                               0.000198609084613718,
                               0.00019658436834063498,
                               0.00019465681844921481,
                               0.000192823229984881,
                               0.00019108059563268805,
                               0.0001894260946736552,
                               0.00018785708279402762,
                               0.00018637108268156676,
                               0.00018496577534914382,
                               0.00018363899213148408,
                               0.00018238870730595351,
                               0.00018121303129284456,
                               0.00018011020439476208,
                               0.00017907859103847974,
                               0.00017811667448606718,
                               0.00017722305198522128,
                               0.00017639643033160151,
                               0.00017563562181859444,
                               0.00017493954055235113,
                               0.00017430719911216695,
                               0.0001737377055383346,
                               0.00017323026063151335,
                               0.00017278415554943967,
                               0.00017239876968847085,
                               0.0001720735688390205,
                               0.00017180810360542452,
                               0.00017160200808218119,
                               0.00017145499877985117,
                               0.00017136687379519321};
  coder::array<creal_T, 2U> Y;
  coder::array<creal_T, 2U> r;
  coder::array<creal_T, 2U> r1;
  coder::array<creal_T, 2U> y2;
  coder::array<double, 2U> a;
  coder::array<double, 2U> r2;
  coder::array<double, 2U> r3;
  coder::array<double, 2U> r4;
  coder::array<double, 2U> x;
  coder::array<double, 2U> y3;
  coder::array<double, 1U> s;
  double d;
  double ex;
  int i;
  int ii;
  int jj;
  int k;
  int last;
  int nx;
  if (!isInitialized_TIME_entrypoint) {
    TIME_entrypoint_initialize();
  }
  //  entry_points/ZERO_TIME_WIND_SPECTRUM_entrypoint.m
  //  Call the main function with the provided arguments
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  Edited by: G Krishna  (Speech Lab, IIIT Hyderabad)
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  ------------       inputs         --------
  //  wav           : input raw speech signal
  //  fs            : sampling freqnecy
  //  N             : N refers to N-point FFT (N:256/512/1024)
  //  M             : M refers to rate of  down sampling the spectrum in
  //  temporal domain (M:1/2/4/8)
  //  ------------      outputs         --------
  //  ZTW_SPEC      : Zero time windowes spectrum
  //  ZTW_HNGD_SPEC : Zero time windowed HNGD spectrum
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //  s=resample(s,8000,fs);
  nx = wav.size(0);
  s.set_size(wav.size(0));
  for (k = 0; k < nx; k++) {
    s[k] = std::abs(wav[k]);
  }
  last = s.size(0);
  if (s.size(0) <= 2) {
    if (s.size(0) == 1) {
      ex = s[0];
    } else {
      ex = s[s.size(0) - 1];
      if ((!(s[0] < ex)) && ((!std::isnan(s[0])) || std::isnan(ex))) {
        ex = s[0];
      }
    }
  } else {
    if (!std::isnan(s[0])) {
      nx = 1;
    } else {
      boolean_T exitg1;
      nx = 0;
      k = 2;
      exitg1 = false;
      while ((!exitg1) && (k <= last)) {
        if (!std::isnan(s[k - 1])) {
          nx = k;
          exitg1 = true;
        } else {
          k++;
        }
      }
    }
    if (nx == 0) {
      ex = s[0];
    } else {
      ex = s[nx - 1];
      i = nx + 1;
      for (k = i; k <= last; k++) {
        d = s[k - 1];
        if (ex < d) {
          ex = d;
        }
      }
    }
  }
  s.set_size(wav.size(0));
  last = wav.size(0);
  for (i = 0; i < last; i++) {
    s[i] = wav[i] / ex;
  }
  //  segement the speech signal in such a way that it is one sample shift
  x.set_size(120, s.size(0));
  if (s.size(0) != 0) {
    for (jj = 0; jj < 119; jj++) {
      x[jj] = 0.0;
    }
    x[119] = s[0];
    i = s.size(0) - 1;
    for (ii = 2; ii <= i; ii++) {
      nx = (ii - 1) * 120;
      last = (ii - 2) * 120;
      for (jj = 0; jj < 119; jj++) {
        x[jj + nx] = x[(jj + last) + 1];
      }
      x[nx + 119] = s[ii - 1];
    }
    nx = (s.size(0) - 1) * 120;
    if (s.size(0) > 1) {
      last = (s.size(0) - 2) * 120;
      for (jj = 0; jj < 119; jj++) {
        x[jj + nx] = x[(jj + last) + 1];
      }
      if (s.size(0) - 1 < 2) {
        i = 1;
      } else {
        i = s.size(0) - 1;
      }
      x[nx + 119] = s[i];
    }
  }
  //  design hd window
  //  hdwindow1=hdwindow1.^2;
  //  hdwindow2=[4 4*cos(pi*n/win_len).^2];
  //  hdwindow2=hdwindow2/max(hdwindow2);
  //  hdwindow1=hdwindow2*hdwindow1;
  //  hdwindow1=gausswin(win_len,20);
  //  hdwindow1=exp(-n.^2)';
  //  hdwindow1=black-man(win_len);
  //  hdwindow2=[0 1./(n.^4)]';
  //  hdwindow2=hdwindow2./max(hdwindow2);
  //  calculating the  windowed speech signal
  //  hd_wind=hdwindow1*ones(1,size(x,2));
  //  x=x.*hd_wind;
  a.set_size(120, x.size(1));
  last = x.size(1);
  for (i = 0; i < last; i++) {
    for (k = 0; k < 120; k++) {
      a[k + 120 * i] = b_a[k];
    }
  }
  jj = 120 * x.size(1);
  x.set_size(120, x.size(1));
  for (i = 0; i < jj; i++) {
    x[i] = x[i] * a[i];
  }
  //  calculating the hngd spectrum
  //
  coder::fft(x, N, y2);
  //
  coder::diff(y2, r);
  coder::diff(r, r1);
  y2.set_size(r1.size(0), r1.size(1));
  ii = r1.size(0) * r1.size(1);
  for (i = 0; i < ii; i++) {
    y2[i] = r1[i];
  }
  y3.set_size(y2.size(0), y2.size(1));
  for (k = 0; k < ii; k++) {
    y3[k] = rt_hypotd_snf(y2[k].re, y2[k].im);
  }
  coder::hilbert(y3, y2);
  nx = y2.size(0) * y2.size(1);
  y3.set_size(y2.size(0), y2.size(1));
  for (k = 0; k < nx; k++) {
    y3[k] = rt_hypotd_snf(y2[k].re, y2[k].im);
  }
  //
  if ((M == 0.0) ||
      (((M > 0.0) && (y3.size(1) < 1)) || ((M < 0.0) && (y3.size(1) > 1)))) {
    i = 1;
    k = 0;
  } else {
    i = static_cast<int>(M);
    k = y3.size(1);
  }
  //
  d = N / 2.0;
  if (d < 1.0) {
    ii = 0;
  } else {
    ii = static_cast<int>(d);
  }
  last = div_s32(k - 1, i);
  ZTW_SPEC.set_size(ii, last + 1);
  for (k = 0; k <= last; k++) {
    for (int i1{0}; i1 < ii; i1++) {
      ZTW_SPEC[i1 + ZTW_SPEC.size(0) * k] = y3[i1 + y3.size(0) * (i * k)];
    }
  }
  //
  //  % surf(y4)
  //
  //  clearvars y2 y3 y4
  //  Assign empty matrices to release memory
  //
  coder::fft(x, N, y2);
  //
  a.set_size(120, x.size(1));
  last = x.size(1);
  for (i = 0; i < last; i++) {
    for (k = 0; k < 120; k++) {
      a[k + 120 * i] = static_cast<double>(k) + 1.0;
    }
  }
  r2.set_size(120, a.size(1));
  for (i = 0; i < jj; i++) {
    r2[i] = a[i] * x[i];
  }
  coder::fft(r2, N, Y);
  //
  //  % ABS_X = abs(y4);
  //
  //  clearvars X Y
  //  ABS_X = abs(y4);
  //  Assign empty matrices to release memory
  //
  //  2-times successive differenation to compensate the integration effect
  //
  if ((y2.size(0) == Y.size(0)) && (y2.size(1) == Y.size(1)) &&
      (y2.size(0) == Y.size(0)) && (y2.size(1) == Y.size(1))) {
    y3.set_size(y2.size(0), y2.size(1));
    last = y2.size(0) * y2.size(1);
    for (i = 0; i < last; i++) {
      y3[i] = y2[i].re * Y[i].re + y2[i].im * Y[i].im;
    }
    coder::diff(y3, r3);
  } else {
    binary_expand_op(r3, y2, Y);
  }
  coder::diff(r3, r4);
  coder::hilbert(r4, y2);
  nx = y2.size(0) * y2.size(1);
  ZTW_HNGD_SPEC.set_size(y2.size(0), y2.size(1));
  for (k = 0; k < nx; k++) {
    ZTW_HNGD_SPEC[k] = rt_hypotd_snf(y2[k].re, y2[k].im);
  }
  //  hilbert envelope of two times successive differeniated NGD Spectrum of
  //  windowed signal
  //
  if ((M == 0.0) || (((M > 0.0) && (ZTW_HNGD_SPEC.size(1) < 1)) ||
                     ((M < 0.0) && (ZTW_HNGD_SPEC.size(1) > 1)))) {
    i = 1;
    k = -1;
  } else {
    i = static_cast<int>(M);
    k = ZTW_HNGD_SPEC.size(1) - 1;
  }
  nx = ZTW_HNGD_SPEC.size(0);
  last = div_s32(k, i);
  y3.set_size(ZTW_HNGD_SPEC.size(0), last + 1);
  for (k = 0; k <= last; k++) {
    for (int i1{0}; i1 < nx; i1++) {
      y3[i1 + y3.size(0) * k] =
          ZTW_HNGD_SPEC[i1 + ZTW_HNGD_SPEC.size(0) * (i * k)];
    }
  }
  ZTW_HNGD_SPEC.set_size(y3.size(0), y3.size(1));
  last = y3.size(0) * y3.size(1);
  for (i = 0; i < last; i++) {
    ZTW_HNGD_SPEC[i] = y3[i];
  }
  //
  nx = ZTW_HNGD_SPEC.size(1);
  for (i = 0; i < nx; i++) {
    for (k = 0; k < ii; k++) {
      ZTW_HNGD_SPEC[k + ii * i] = ZTW_HNGD_SPEC[k + ZTW_HNGD_SPEC.size(0) * i];
    }
  }
  ZTW_HNGD_SPEC.set_size(ii, ZTW_HNGD_SPEC.size(1));
  //
  //
  //  clearvars y5 y6
  //  Assign empty matrices to release memory
}

// End of code generation (TIME_entrypoint.cpp)
