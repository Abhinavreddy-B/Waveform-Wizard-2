import librosa
import numpy as np


def constantq(data, fs):
    CC1 = librosa.cqt(data, sr=fs, bins_per_octave=12, fmin=10)
    CC = np.abs(CC1)
    nyquist_bin = CC.shape[0]
    CC = CC[:nyquist_bin, :]
    fs_bins_cq = librosa.cqt_frequencies(n_bins=nyquist_bin, fmin=10, bins_per_octave=12)
    return CC, fs_bins_cq
