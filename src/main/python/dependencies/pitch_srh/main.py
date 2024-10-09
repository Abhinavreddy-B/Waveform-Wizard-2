import librosa
import numpy as np
from scipy.signal import lfilter, resample
from scipy.signal.windows import hann
from scipy.stats import median_abs_deviation as mad
from librosa import lpc
import copy

# from resample import resample

def pitch_srh(wave_original, fs, f0min, f0max, hopsize=10):
    wave = copy.deepcopy(wave_original)

    if len(wave) / fs < 0.1:
        print('SRH error: the duration of your file should be at least 100ms long')
        return 0, 0, 0, 0

    if f0max <= f0min:
        print('You look funny! Your f0min should be lower than f0max!!')
        return 0, 0, 0, 0

    if fs > 16000:
        print('Sample rate is bigger than 16kHz. Audio is resampled.')
        wave = resample(wave, 16000, fs)
        fs = 16000

    nHarmonics = 5
    SRHiterThresh = 0.1
    SRHstdThresh = 0.05
    VoicingThresh = 0.05
    VoicingThresh2 = 0.085
    LPCorder = int(round(3 / 4 * fs / 1000))
    Niter = 2

    res, LPCcoeff = lpcresidual(wave, round(25 / 1000 * fs), round(5 / 1000 * fs), LPCorder)

    waveLen = len(wave)
    frameDuration = int(round(100 / 1000 * fs)) - 2
    frameDuration = int(round(frameDuration / 2)) * 2
    shift = int(round(hopsize / 1000 * fs))
    halfDur = int(round(frameDuration / 2))
    time = np.arange(halfDur, waveLen - halfDur, shift)
    N = len(time)
    frameMat = np.zeros((frameDuration, N))
    # print('Res',res)
    # print('Res Size', res.shape)
    for i, t in enumerate(time):
        # if i == 0:
        #     print('t-', t-halfDur)
        #     print('t+', t+halfDur)
        frameMat[:, i] = res[t - halfDur:t + halfDur]
    
    win = np.blackman(frameDuration)
    frameMatWin = frameMat * win[:, None]

    frameMean = np.mean(frameMatWin, axis=0)
    frameMatWinMean = frameMatWin - frameMean

    specMat = np.abs(np.fft.fft(frameMatWinMean, int(fs), axis=0)[:int(fs) // 2])
    specDenom = np.sqrt(np.sum(specMat ** 2, axis=0))
    specMat /= specDenom

    for Iter in range(Niter):
        F0s, SRHVal = SRH(specMat, nHarmonics, f0min, f0max)
        if max(SRHVal) > SRHiterThresh:
            F0medEst = np.median(F0s[SRHVal > SRHiterThresh])
            if round(0.5 * F0medEst) > f0min:
                f0min = round(0.5 * F0medEst)
            if round(2 * F0medEst) < f0max:
                f0max = round(2 * F0medEst)

    time = time / fs
    VUVDecisions = np.zeros(N)
    if np.std(SRHVal) > SRHstdThresh:
        VoicingThresh = VoicingThresh2
    VUVDecisions[SRHVal > VoicingThresh] = 1

    return F0s, VUVDecisions, SRHVal, time

def lpcresidual(x, L, shift, order):
    """
    Function to derive the Linear Prediction residual signal.

    Args:
    - x: Input signal (Nx1)
    - L: Window length in samples
    - shift: Window shift in samples
    - order: Order of Linear Prediction

    Returns:
    - res: Linear Prediction residual (Mx1)
    - LPCcoeff: Linear Prediction coefficients (order+1 x M)
    """

    # Initial settings
    x = np.squeeze(x)
    shift = int(round(shift))
    order = int(round(order))

    start = 0
    L = round(L)
    stop = start + L

    # Allocate space
    res = np.zeros(len(x))
    LPCcoeff = np.zeros((order + 1, len(x) // shift))

    # Do processing
    n = 0
    win = hann(L)
    while stop <= len(x):
        segment = x[start:stop]
        segment *= win

        A = lpc(segment, order=order)
        LPCcoeff[:, n] = A

        inv = lfilter(A, 1, segment)
        inv *= np.sqrt(np.sum(segment**2) / np.sum(inv**2))

        res[start:stop] += inv

        # Increment
        start += shift
        stop += shift
        n += 1

    res /= np.max(np.abs(res))  # Normalize amplitude
    return res, LPCcoeff


def SRH(specMat, nHarmonics, f0min, f0max):
    N = specMat.shape[1]
    SRHmat = np.zeros((f0max, N))

    fSeq = np.arange(f0min, f0max + 1)
    fLen = len(fSeq)

    # Prepare harmonic indices matrices.
    plusIdx = (np.arange(1, nHarmonics + 1)[:, None] * fSeq).flatten()
    subtrIdx = np.round((np.arange(1, nHarmonics)[:, None] + 0.5) * fSeq).flatten()

    # Avoid costly modulo operation by adjusting indices
    plusIdx = (plusIdx - 1) % specMat.shape[0] + 1
    subtrIdx = (subtrIdx - 1) % specMat.shape[0] + 1

    plusIdx = plusIdx.astype(int)
    subtrIdx = subtrIdx.astype(int)
    # Do harmonic summation
    for n in range(N):
        specMatCur = specMat[:, n]
        SRHmat[fSeq - 1, n] = np.sum(specMatCur[plusIdx].reshape(nHarmonics, fLen), axis=0) - \
                              np.sum(specMatCur[subtrIdx].reshape(nHarmonics - 1, fLen), axis=0)

    # Retrieve f0 and SRH value
    F0 = np.argmax(SRHmat, axis=0) + f0min
    SRHVal = np.max(SRHmat, axis=0)

    return F0, SRHVal

# audio_file_path = '/home/sasidhar/Sem6/BTP/Waveform-Wizard/samples/audio1.wav'
# y, sr = librosa.load(audio_file_path, sr=None)
# data = y.astype(float)
# fs = float(sr)
# temp = pitch_srh(data, fs, 50, 500)
# print(temp)