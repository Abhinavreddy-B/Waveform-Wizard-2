"""A python port of portions of the Matlab Auditory Toolbox.
"""
from __future__ import division

from typing import List

import librosa
import matplotlib.pyplot as plt
import numpy as np
import scipy.io.wavfile as wf
import scipy.signal as sps


def ErbSpace(low_freq: float = 100, high_freq: float = 44100/4,
             n: int = 100) -> np.ndarray:
  """This function computes an array of N frequencies uniformly spaced between
  high_freq and low_freq on an erb scale.  N is set to 100 if not specified.

  See also linspace, logspace, MakeErbCoeffs, MakeErbFilters.

  For a definition of erb, see Moore, B. C. J., and Glasberg, B. R. (1983).
  "Suggested formulae for calculating auditory-filter bandwidths and
  excitation patterns," J. Acoust. Soc. Am. 74, 750-753.

  Args:
    low_freq: The center frequency in Hz of the lowest channel
    high_freq: The upper limit in Hz of the channel bank.  The center frequency
      of the highest channel will be below this frequency.
    n: Number of channels

  Returns:
    An array of center frequencies, equally spaced on the ERB scale.
  """

  #  Change the following three parameters if you wish to use a different
  #  erb scale.  Must change in MakeerbCoeffs too.
  ear_q = 9.26449				# Glasberg and Moore Parameters
  min_bw = 24.7

  # All of the follow_freqing expressions are derived in Apple TR #35, "An
  # Efficient Implementation of the Patterson-Holdsworth Cochlear
  # Filter Bank."  See pages 33-34.
  cf_array = (-(ear_q*min_bw) +
             np.exp(np.arange(1, 1+n)*
                    (-np.log(high_freq + ear_q*min_bw) +
                     np.log(low_freq + ear_q*min_bw))/n) * (high_freq +
                                                             ear_q*min_bw))
  return cf_array


def MakeErbFilters(fs: float, num_channels: int,
                   low_freq: float = 20) -> List[np.ndarray]:
  """This function computes the filter coefficients for a bank of
  Gammatone filters.  These filters were defined by Patterson and
  Holdworth for simulating the cochlea.

  The result is returned as an array of filter coefficients.  Each row
  of the filter arrays contains the coefficients for four second order
  filters.  The transfer function for these four filters share the same
  denominator (poles) but have different numerators (zeros).  All of these
  coefficients are assembled into one vector that the ErbFilterBank
  can take apart to implement the filter.

  The filter bank contains "num_channels" channels that extend from
  half the sampling rate (fs) to "low_freq".  Alternatively, if the num_channels
  input argument is a vector, then the values of this vector are taken to
  be the center frequency of each desired filter.  (The low_freq argument is
  ignored in this case.)

  Note this implementation fixes a problem in the original code by
  computing four separate second order filters.  This avoids a big
  problem with round off errors in cases of very small cfs (100Hz) and
  large sample rates (44kHz).  The problem is caused by roundoff error
  when a number of poles are combined, all very close to the unit
  circle.  Small errors in the eigth order coefficient, are multiplied
  when the eigth root is taken to give the pole location.  These small
  errors lead to poles outside the unit circle and instability.  Thanks
  to Julius Smith for leading me to the proper explanation.

  Execute the following code to evaluate the frequency
  response of a 10 channel filterbank.
    fcoefs = MakeErbFilters(16000,10,100)
    y = ErbFilterBank([1 zeros(1,511)], fcoefs)
   	resp = 20*log10(abs(fft(y')))
   	freqScale = (0:511)/512*16000
  	semilogx(freqScale(1:255),resp(1:255,:))
  	axis([100 16000 -60 0])
  	xlabel('Frequency (Hz)') ylabel('Filter Response (dB)')

  Args:
    fs: Sampling rate (in Hz) of the filterbank (needed to determine CFs).
    num_channel: How many channels in the filterbank.
    low_freq: The lowest center frequency of the filterbank.

  Returns: 
    A list of 11 num_channel-D arrays containing the filter parameters.
  """

  t = 1/fs
  if isinstance(num_channels, int):
    cf = ErbSpace(low_freq, fs/2, num_channels)
  else:
    cf = num_channels

  # So equations below match the original Matlab syntax
  pi = np.pi
  abs = np.abs  # pylint: disable=redefined-builtin
  sqrt = np.sqrt
  sin = np.sin
  cos = np.cos
  exp = np.exp
  i = np.array([1j], dtype=np.csingle)

  # Change the follow_freqing three parameters if you wish to use a different
  # erb scale.  Must change in ErbSpace too.
  ear_q = 9.26449				#  Glasberg and Moore Parameters
  min_bw = 24.7
  order = 1

  erb = ((cf/ear_q)**order + min_bw**order)**(1/order)
  b=1.019*2*pi*erb

  a0 = t
  a2 = 0
  b0 = 1
  b1 = -2*cos(2*cf*pi*t)/exp(b*t)
  b2 = exp(-2*b*t)

  a11 = -(2*t*cos(2*cf*pi*t)/exp(b*t) + 2*sqrt(3+2**1.5)*t*sin(2*cf*pi*t)/
      exp(b*t))/2
  a12 = -(2*t*cos(2*cf*pi*t)/exp(b*t) - 2*sqrt(3+2**1.5)*t*sin(2*cf*pi*t)/
      exp(b*t))/2
  a13 = -(2*t*cos(2*cf*pi*t)/exp(b*t) + 2*sqrt(3-2**1.5)*t*sin(2*cf*pi*t)/
      exp(b*t))/2
  a14 = -(2*t*cos(2*cf*pi*t)/exp(b*t) - 2*sqrt(3-2**1.5)*t*sin(2*cf*pi*t)/
      exp(b*t))/2

  gain = abs((-2*exp(4*i*cf*pi*t)*t +
                  2*exp(-(b*t) + 2*i*cf*pi*t)*t*
                          (cos(2*cf*pi*t) - sqrt(3 - 2**(3/2))*
                            sin(2*cf*pi*t))) *
            (-2*exp(4*i*cf*pi*t)*t +
              2*exp(-(b*t) + 2*i*cf*pi*t)*t*
                (cos(2*cf*pi*t) + sqrt(3 - 2**(3/2)) *
                sin(2*cf*pi*t)))*
            (-2*exp(4*i*cf*pi*t)*t +
              2*exp(-(b*t) + 2*i*cf*pi*t)*t*
                (cos(2*cf*pi*t) -
                sqrt(3 + 2**(3/2))*sin(2*cf*pi*t))) *
            (-2*exp(4*i*cf*pi*t)*t + 2*exp(-(b*t) + 2*i*cf*pi*t)*t*
            (cos(2*cf*pi*t) + sqrt(3 + 2**(3/2))*sin(2*cf*pi*t))) /
            (-2 / exp(2*b*t) - 2*exp(4*i*cf*pi*t) +
            2*(1 + exp(4*i*cf*pi*t))/exp(b*t))**4)

  allfilts = np.ones(len(cf))
  fcoefs = [a0*allfilts, a11, a12, a13, a14, a2*allfilts,
            b0*allfilts, b1, b2, gain]
  return fcoefs


def ErbFilterBank(x: np.ndarray, fcoefs: List[np.ndarray]) -> np.ndarray:
  """Filter an input signal with a filterbank, producing one output vector
  per channel.
  
  Args:
    x: The input signal, one-dimensional
    fcoefs: A list of 11 num-channel-dimensional arrays that describe the 
      filterbank.
  
  Returns:
    num-channel outputs in a num_channel x time array.
  """
  [a0, a11, a12, a13, a14, a2, b0, b1, b2, gain] = fcoefs
  n_chan = a0.shape[0]
  assert n_chan == a11.shape[0]
  assert n_chan == a12.shape[0]
  assert n_chan == a13.shape[0]
  assert n_chan == a14.shape[0]
  assert n_chan == b0.shape[0]
  assert n_chan == b1.shape[0]
  assert n_chan == gain.shape[0]

  sos = np.stack((np.stack([a0/gain,  a0,   a0, a0], axis=1),
                  np.stack([a11/gain, a12, a13, a14], axis=1),
                  np.stack([a2/gain,  a2,   a2, a2], axis=1),
                  np.stack([b0, b0, b0, b0], axis=1),
                  np.stack([b1, b1, b1, b1], axis=1),
                  np.stack([b2, b2, b2, b2], axis=1)),
                axis=2)

  all_y = None
  for c in range(n_chan):
    y = sps.sosfilt(sos[c, :, :], x)
    if all_y is None:
      all_y = np.zeros((n_chan, len(y)), dtype=x.dtype)
    all_y[c, :] = y
  return all_y

# def fft2gammatonemx(nfft, sr=16000, nfilts=64, width=1.0, minfreq=100, 
#                     maxfreq=8000, maxlen=1024):    
#     """
#     # Ellis' description in MATLAB:
#     # [wts,cfreqa] = fft2gammatonemx(nfft, sr, nfilts, width, minfreq, maxfreq, maxlen)
#     #      Generate a matrix of weights to combine FFT bins into
#     #      Gammatone bins.  nfft defines the source FFT size at
#     #      sampling rate sr.  Optional nfilts specifies the number of
#     #      output bands required (default 64), and width is the
#     #      constant width of each band in Bark (default 1).
#     #      minfreq, maxfreq specify range covered in Hz (100, sr/2).
#     #      While wts has nfft columns, the second half are all zero. 
#     #      Hence, aud spectrum is
#     #      fft2gammatonemx(nfft,sr)*abs(fft(xincols,nfft));
#     #      maxlen truncates the rows to this many bins.
#     #      cfreqs returns the actual center frequencies of each
#     #      gammatone band in Hz.
#     #
#     # 2009/02/22 02:29:25 Dan Ellis dpwe@ee.columbia.edu  based on rastamat/audspec.m
#     # Sat May 27 15:37:50 2017 Maddie Cusimano, mcusi@mit.edu 27 May 2017: convert to python
#     """
    
#     wts = np.zeros([nfilts,nfft])
    
#     #after Slaney's MakeERBFilters
#     EarQ = 9.26449; minBW = 24.7; order = 1
    
#     #nFr = np.array(range(nfilts)) + 1
#     #em = EarQ*minBW
#     #cfreqs = (maxfreq+em)*np.exp(nFr*(-np.log(maxfreq + em)+np.log(minfreq + em))/nfilts)-em
#     cfreqs = ErbSpace(minfreq, maxfreq, nfilts)
#     cfreqs = cfreqs[::-1]
    
#     GTord = 4
#     ucircArray = np.array(range(int(nfft/2 + 1)))
#     ucirc = np.exp(1j*2*np.pi*ucircArray/nfft)
#     justpoles = 0 #:taking out the 'if' corresponding to this. 
#     for i in range(nfilts):
#         cf = cfreqs[i]
#         ERB = width*np.power(np.power(cf/EarQ,order) + np.power(minBW,order),1/order);
#         B = 1.019 * 2 * np.pi * ERB
#         r = np.exp(-B/sr)
#         theta = 2*np.pi*cf/sr
#         pole = r*np.exp(1j*theta)
#         if justpoles == 1:
#             cosomegamax = (1 + r * r) / (2 * r) * np.cos(theta)
#             omegamax = np.arccos(np.clip(cosomegamax, -1, 1))
#             gain = np.abs((pole - np.exp(1j * omegamax)) * (np.conj(pole) - np.exp(1j * omegamax))) ** GTord
#             wts[i, :nfft // 2 + 1] = gain * (np.abs((pole - ucirc) * (np.conj(pole) - ucirc)) ** -GTord)
#         else:
#             T = 1/sr
#             ebt = np.exp(B*T); cpt = 2*cf*np.pi*T
#             ccpt = 2*T*np.cos(cpt); scpt = 2*T*np.sin(cpt)
#             A11 = -np.divide(np.divide(ccpt,ebt) + np.divide(np.sqrt(3+2**1.5)*scpt,ebt),2) 
#             A12 = -np.divide(np.divide(ccpt,ebt) - np.divide(np.sqrt(3+2**1.5)*scpt,ebt),2)
#             A13 = -np.divide(np.divide(ccpt,ebt) + np.divide(np.sqrt(3-2**1.5)*scpt,ebt),2) 
#             A14 = -np.divide(np.divide(ccpt,ebt) - np.divide(np.sqrt(3-2**1.5)*scpt,ebt),2)
#             zros = -np.array([A11, A12, A13, A14])/T
#             wIdx = range(int(nfft/2 + 1))  
#             gain[i] = np.abs((-2*np.exp(4*1j*cf*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cf*np.pi*T)*T* (np.cos(2*cf*np.pi*T) - np.sqrt(3 - 2**(3/2))*  np.sin(2*cf*np.pi*T))) *(-2*np.exp(4*1j*cf*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cf*np.pi*T)*T* (np.cos(2*cf*np.pi*T) + np.sqrt(3 - 2**(3/2)) *  np.sin(2*cf*np.pi*T)))*(-2*np.exp(4*1j*cf*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cf*np.pi*T)*T* (np.cos(2*cf*np.pi*T) -  np.sqrt(3 + 2**(3/2))*np.sin(2*cf*np.pi*T))) *(-2*np.exp(4*1j*cf*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cf*np.pi*T)*T* (np.cos(2*cf*np.pi*T) + np.sqrt(3 + 2**(3/2))*np.sin(2*cf*np.pi*T))) /(-2 / np.exp(2*B*T) - 2*np.exp(4*1j*cf*np.pi*T) +  2*(1 + np.exp(4*1j*cf*np.pi*T))/np.exp(B*T))**4)
#             #in MATLAB, there used to be 64 where here it says nfilts:
#             wts[i, wIdx] =  ((T**4)/np.reshape(gain[i],(nfilts,1))) * np.abs(ucirc-np.reshape(zros[0],(nfilts,1)))*np.abs(ucirc-np.reshape(zros[1],(nfilts,1)))*np.abs(ucirc-np.reshape(zros[2],(nfilts,1)))*np.abs(ucirc-np.reshape(zros[3],(nfilts,1)))*(np.abs(np.power(np.multiply(np.reshape(pole,(nfilts,1))-ucirc,np.conj(np.reshape(pole,(nfilts,1)))-ucirc),-GTord)))        
#     wts = wts[:,:maxlen]  
#     return wts, cfreqs

def fft2gammatonemx(nfft, sr=16000, nfilts=64, width=1.0, minfreq=100, 
                    maxfreq=10000, maxlen=1024):    
    """
    # Ellis' description in MATLAB:
    # [wts,cfreqa] = fft2gammatonemx(nfft, sr, nfilts, width, minfreq, maxfreq, maxlen)
    #      Generate a matrix of weights to combine FFT bins into
    #      Gammatone bins.  nfft defines the source FFT size at
    #      sampling rate sr.  Optional nfilts specifies the number of
    #      output bands required (default 64), and width is the
    #      constant width of each band in Bark (default 1).
    #      minfreq, maxfreq specify range covered in Hz (100, sr/2).
    #      While wts has nfft columns, the second half are all zero. 
    #      Hence, aud spectrum is
    #      fft2gammatonemx(nfft,sr)*abs(fft(xincols,nfft));
    #      maxlen truncates the rows to this many bins.
    #      cfreqs returns the actual center frequencies of each
    #      gammatone band in Hz.
    #
    # 2009/02/22 02:29:25 Dan Ellis dpwe@ee.columbia.edu  based on rastamat/audspec.m
    # Sat May 27 15:37:50 2017 Maddie Cusimano, mcusi@mit.edu 27 May 2017: convert to python
    """
    
    wts = np.zeros([nfilts,nfft])
    
    #after Slaney's MakeERBFilters
    EarQ = 9.26449; minBW = 24.7; order = 1
    
    nFr = np.array(range(nfilts)) + 1
    em = EarQ*minBW
    cfreqs = (maxfreq+em)*np.exp(nFr*(-np.log(maxfreq + em)+np.log(minfreq + em))/nfilts)-em
    cfreqs = cfreqs[::-1]
    
    GTord = 4
    ucircArray = np.array(range(int(nfft/2 + 1)))
    ucirc = np.exp(1j*2*np.pi*ucircArray/nfft);
    #justpoles = 0 :taking out the 'if' corresponding to this. 

    ERB = width*np.power(np.power(cfreqs/EarQ,order) + np.power(minBW,order),1/order);
    B = 1.019 * 2 * np.pi * ERB
    r = np.exp(-B/sr)
    theta = 2*np.pi*cfreqs/sr
    pole = r*np.exp(1j*theta)
    T = 1/sr
    ebt = np.exp(B*T); cpt = 2*cfreqs*np.pi*T  
    ccpt = 2*T*np.cos(cpt); scpt = 2*T*np.sin(cpt)
    A11 = -np.divide(np.divide(ccpt,ebt) + np.divide(np.sqrt(3+2**1.5)*scpt,ebt),2)
    A12 = -np.divide(np.divide(ccpt,ebt) - np.divide(np.sqrt(3+2**1.5)*scpt,ebt),2)
    A13 = -np.divide(np.divide(ccpt,ebt) + np.divide(np.sqrt(3-2**1.5)*scpt,ebt),2) 
    A14 = -np.divide(np.divide(ccpt,ebt) - np.divide(np.sqrt(3-2**1.5)*scpt,ebt),2)
    zros = -np.array([A11, A12, A13, A14])/T
    wIdx = range(int(nfft/2 + 1))  
    gain = np.abs((-2*np.exp(4*1j*cfreqs*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cfreqs*np.pi*T)*T* (np.cos(2*cfreqs*np.pi*T) - np.sqrt(3 - 2**(3/2))*  np.sin(2*cfreqs*np.pi*T))) *(-2*np.exp(4*1j*cfreqs*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cfreqs*np.pi*T)*T* (np.cos(2*cfreqs*np.pi*T) + np.sqrt(3 - 2**(3/2)) *  np.sin(2*cfreqs*np.pi*T)))*(-2*np.exp(4*1j*cfreqs*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cfreqs*np.pi*T)*T* (np.cos(2*cfreqs*np.pi*T) -  np.sqrt(3 + 2**(3/2))*np.sin(2*cfreqs*np.pi*T))) *(-2*np.exp(4*1j*cfreqs*np.pi*T)*T + 2*np.exp(-(B*T) + 2*1j*cfreqs*np.pi*T)*T* (np.cos(2*cfreqs*np.pi*T) + np.sqrt(3 + 2**(3/2))*np.sin(2*cfreqs*np.pi*T))) /(-2 / np.exp(2*B*T) - 2*np.exp(4*1j*cfreqs*np.pi*T) +  2*(1 + np.exp(4*1j*cfreqs*np.pi*T))/np.exp(B*T))**4)
    #in MATLAB, there used to be 64 where here it says nfilts:
    wts[:, wIdx] =  ((T**4)/np.reshape(gain,(nfilts,1))) * np.abs(ucirc-np.reshape(zros[0],(nfilts,1)))*np.abs(ucirc-np.reshape(zros[1],(nfilts,1)))*np.abs(ucirc-np.reshape(zros[2],(nfilts,1)))*np.abs(ucirc-np.reshape(zros[3],(nfilts,1)))*(np.abs(np.power(np.multiply(np.reshape(pole,(nfilts,1))-ucirc,np.conj(np.reshape(pole,(nfilts,1)))-ucirc),-GTord)))
    wts = wts[:,range(maxlen)]
    
    return wts, cfreqs

def gammatonegram(x, sr=20000, twin=0.025, thop=0.010, N=64, fmin=50, fmax=10000, USEFFT=1, width=1.0):
    if fmax is None:
        fmax = sr / 2
    
    if USEFFT == 0:
        # Use Malcolm's function to filter into subbands
        # IGNORES FMAX!
        fcoefs = np.flipud(MakeErbFilters(sr, N, fmin))

        xf = ErbFilterBank(x, fcoefs)

        nwin = round(twin * sr)
        xe = [xf ** 2]

        hopsamps = round(thop * sr)

        ncols = 1 + np.floor((xe.shape[1] - nwin) / hopsamps).astype(int)

        Y = np.zeros((N, ncols))

        for i in range(ncols):
            Y[:, i] = np.sqrt(np.mean(xe[:, (i - 1) * hopsamps + np.arange(1, nwin)], axis=1))

    else:
        # USEFFT version
        # How long a window to use relative to the integration window requested
        winext = 1
        twinmod = winext * twin
        # First spectrogram
        nfft = int(2 ** (np.ceil(np.log2(2 * twinmod * sr))))
        nhop = round(thop * sr)
        nwin = round(twinmod * sr)
        gtm, _ = fft2gammatonemx(nfft, sr, N, width, fmin, fmax, nfft // 2 + 1)
        plotF, plotT, Sxx = sps.spectrogram(x, fs=sr, window='hann', nperseg=nwin, 
                                noverlap=nwin-nhop, nfft=nfft, detrend=False, 
                                scaling='density', mode='magnitude')
        Y = (1 / nfft) * np.dot(gtm, np.abs(Sxx))

    return Y

def plot_gammatone_2d(T, F, ZTS):
    plt.pcolormesh(T, F, abs(ZTS), shading='auto')
    plt.colorbar(label='Magnitude')
    plt.xlabel('Time')
    plt.ylabel('Frequency')
    plt.title('Gammatonegram')
    plt.show()

def plot_gammatone(data, fs):
    YG = gammatonegram(data, fs)
    f_bins = YG.shape[0]
    fg = np.arange(0, f_bins) * fs / (2 * f_bins)
    time_bins_g = YG.shape[1]
    ts_g = np.arange(0, time_bins_g) * len(data) / time_bins_g
    ts_g = ts_g / fs
    T_g, F_g = np.meshgrid(ts_g, fg)
    print(T_g.shape, F_g.shape)    
    plot_gammatone_2d(T_g, F_g, np.abs(YG))

if __name__ == '__main__':
  audio_file_path = '/home/sasidhar/Sem6/BTP/Waveform-Wizard/samples/audio1.wav'
  y, sr = librosa.load(audio_file_path, sr=None)
  data = y.astype(float)
  fs = float(sr)
  plot_gammatone(data, fs)