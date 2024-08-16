import matplotlib.pyplot as plt
import numpy as np
from scipy.io import wavfile
from scipy.signal import lfilter


def SINGLE_FREQ_FILTER_FS(wav, fs, freq_step, f1, f2, r=None):
    #######################################################################
    # This code finds single frequency filtered envelopes wave from speech Signal
    # Edited by: G Krishna (Speech Lab, IIIT Hyderabad)
    #######################################################################
    # --------       inputs         --------
    # wav       : input raw speech signal
    # fs        : sampling frequency
    # freq_step : frequency step size in finding the envelopes
    # --------      outputs         --------
    # env       : single frequency envelopes by SFF
    # env_et    : weighted single frequency envelopes by SFF
    #######################################################################

    # wav = wav / max(abs(wav))
    # wav = resample(wav, 8000, fs)
    # fs = 8000

    # pre-emphasis
    if r is None:
        r = 0.995

    x = np.diff(wav)
    x = np.append(x, x[-1])

    fk1 = np.arange(f1, f2 + freq_step, freq_step)
    fk = (fs / 2) - fk1

    wk = 2 * np.pi * fk / fs

    yk = np.zeros((len(x), len(wk)), dtype=complex)
    for i, wk_val in enumerate(wk):
        xk = x * np.exp(1j * np.arange(1, len(x) + 1) * wk_val)
        yk[:, i] = lfilter([1], [1, -r], xk)

    env = np.abs(yk)

    # weighted component envelopes
    wks = np.mean(env, axis=0) / np.mean(np.mean(env))
    env_wt = env * wks

    return env, env_wt

if __name__ == '__main__':
    fs, x1 = wavfile.read("../../../samples/audio1.wav")


    env, _ = SINGLE_FREQ_FILTER_FS(x1, fs, 20, 0, fs/2, 0.98)
    env = env.T

    freq_bins_s, time_bins_s = env.shape

    fsn = fs / 2
    fs_s = np.arange(1, freq_bins_s + 1) * fsn / freq_bins_s    

    ts_s = np.arange(1, time_bins_s + 1) * len(x1) / time_bins_s
    ts_s = ts_s / fs

    T_s, F_s = np.meshgrid(ts_s, fs_s)

    plt.figure()
    plt.pcolormesh(T_s, F_s, np.abs(env), shading='auto')
    plt.xlabel('Time')
    plt.ylabel('Frequency')
    plt.colorbar(label='Amplitude')
    plt.show()


