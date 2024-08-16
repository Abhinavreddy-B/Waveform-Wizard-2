import numpy as np
from scipy.signal import filtfilt
from scipy.signal.windows import gaussian

def voiced_unvoiced_own(x1, fs):
    y1 = np.abs(x1)
    
    w1 = gaussian(20 * fs // 1000, 4)
    y2 = filtfilt(w1, 1, y1)
    y2 = y2 / np.max(y2)
    y2[0] = 0
    y2[-1] = 0
    Th = 1 / 25
    vuv = np.zeros_like(y2)
    vuv[y2 > Th] = 1
    
    pind = np.where(np.diff(vuv) == 1)[0]
    nind = np.where(np.diff(vuv) == -1)[0]
    
    vuv2 = np.copy(vuv)
    for i in range(len(pind) - 1):
        D = pind[i + 1] - nind[i]
        if D < 40 * fs // 1000:
            vuv2[nind[i]:pind[i + 1]] = 1
    
    vuv3 = np.copy(vuv2)
    pind = np.where(np.diff(vuv2) == 1)[0]
    nind = np.where(np.diff(vuv2) == -1)[0]
    for i in range(len(pind)):
        D = nind[i] - pind[i]
        if D < 30 * fs // 1000:
            vuv3[pind[i]:nind[i]] = 0
    
    if len(pind) == 1:
        wave = x1
    else:
        for i in range(len(pind)):
            if i == 0:
                reg1 = x1[:(nind[i] + pind[i + 1]) // 2]
                t = np.arange(1, (nind[i] + pind[i + 1]) // 2 + 1)
                temppind = 1
            else:
                if i < len(pind)-1:
                    reg1 = x1[(nind[i - 1] + pind[i]) // 2 + 1:(nind[i] + pind[i + 1]) // 2]
                    t = np.arange((nind[i - 1] + pind[i]) // 2 + 1, (nind[i] + pind[i + 1]) // 2 + 1)
                    temppind = (nind[i - 1] + pind[i]) // 2
                else:
                    reg1 = x1[(nind[i - 1] + pind[i]) // 2 + 1:]
                    t = np.arange((nind[i - 1] + pind[i]) // 2 + 1, len(x1) + 1)
                    temppind = (nind[i - 1] + pind[i]) // 2
            wave = reg1

    return vuv3, vuv2

import matplotlib.pyplot as plt
from scipy.io import wavfile


# Assuming the function is defined and imported

def test_voiced_unvoiced_own():
    fs, x1 = wavfile.read("../../../samples/audio1.wav")

    # Normalize the audio signal
    x1 = x1 / np.max(np.abs(x1))

    # Call the function
    vuv3, vuv2 = voiced_unvoiced_own(x1, fs)

    # Plotting
    t = np.arange(len(x1)) / fs

    plt.figure(figsize=(10, 6))
    plt.subplot(3, 1, 1)
    plt.plot(t, x1)
    plt.title('Original Signal')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')

    plt.subplot(3, 1, 2)
    plt.plot(t, vuv2 * 0.95)
    plt.title('Voiced/Unvoiced Signal (before removing false boundaries)')
    plt.xlabel('Time (s)')
    plt.ylabel('Voiced (1) / Unvoiced (0)')

    plt.subplot(3, 1, 3)
    plt.plot(t, vuv3 * 0.95)
    plt.title('Voiced/Unvoiced Signal (after removing false boundaries)')
    plt.xlabel('Time (s)')
    plt.ylabel('Voiced (1) / Unvoiced (0)')

    plt.tight_layout()
    plt.show()


if __name__ == '__main__':
    # Call the test function
    test_voiced_unvoiced_own()
