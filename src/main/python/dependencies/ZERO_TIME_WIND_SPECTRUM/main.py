import numpy as np
from scipy.fft import fft
from scipy.signal import hilbert


def buffer(x, n, p=0, opt=None):
    """
    Buffer a signal vector into a matrix of data frames.

    Parameters:
    x (array-like): Signal vector to buffer.
    n (int): Number of samples per frame.
    p (int): Number of overlapping samples (p > 0) or underlapping samples (p < 0). Default is 0.
    opt (str or array-like): Initial condition for overlap, or scalar offset for underlap. Default is None.

    Returns:
    y (ndarray): Matrix where each column is a frame.
    z (ndarray): Remaining samples that do not form a complete frame.
    """
    x = np.asarray(x).flatten()  # Ensure x is a 1-D array
    
    # Overlap/Underlap handling
    if p > 0:
        if opt is None:
            ic = np.zeros(p)
        elif opt == 'nodelay':
            ic = np.array([])
        else:
            ic = np.asarray(opt)
        x = np.concatenate((ic, x))
    
    elif p < 0:
        offset = 0 if opt is None else opt
        x = x[offset:]
        p = abs(p)
    
    step = n - p
    num_frames = (len(x) + step - 1) // step  # Compute number of frames
    
    # Pad the end of x with zeros if necessary
    pad_len = (num_frames * step + p) - len(x)
    x = np.concatenate((x, np.zeros(pad_len)))
    
    # Reshape x into a matrix where each column is a frame
    indices = np.arange(0, num_frames * step, step)[:, None] + np.arange(n)
    y = x[indices]
    
    # Determine the remaining samples
    z = x[(num_frames * step + p):]
    
    return y, z

def zero_time_wind_spectrum(wav, fs, N=512, M=1):
    """
    Zero Time Windowed Spectrum calculation

    Parameters:
    wav (array): input raw speech signal
    fs (int): sampling frequency
    N (int): N-point FFT (default: 512)
    M (int): rate of down sampling the spectrum in temporal domain (default: 1)

    Returns:
    ZTW_SPEC: Zero time windowed spectrum
    ZTW_HNGD_SPEC: Zero time windowed HNGD spectrum
    """

    s = wav
    fs = 8000
    s = s / np.max(np.abs(s))

    win_len = int(15 * fs / 1000)
    overlap = win_len - 1

    # Buffer the signal
    x, _ = buffer(s, win_len, overlap)  # 'x' will be of shape (win_len, num_frames)
    
    n = np.arange(1, win_len)  # Correctly generate n for the window length
    hdwindow1 = np.concatenate(([0], 1. / (4 * np.sin(np.pi * n / (2 * win_len)))**2))
    hdwindow1 = hdwindow1 / np.max(hdwindow1)
    
    # Adjust 'hdwindow1' size to match x if necessary
    if len(hdwindow1) != x.shape[0]:
        hdwindow1 = np.concatenate((hdwindow1, np.zeros(x.shape[0] - len(hdwindow1))))
    
    # Reshape hd_wind to match 'x'
    hd_wind = np.tile(hdwindow1[:, np.newaxis], (1, x.shape[1]))
    
    # Multiply 'x' with 'hd_wind'
    x = x * hd_wind

    y2 = np.fft.fft(x, N, axis=0)
    y3 = np.abs(hilbert(np.abs(np.diff(np.diff(y2, axis=0), axis=0))))
    y4 = y3[:, ::M]
    y4 = y4[:N//2, :]

    ZTW_SPEC = y4

    y2, y3, y4 = None, None, None

    y5 = gd_fns(x, N)
    y6 = np.diff(np.diff(y5, axis=0), axis=0)
    y6 = np.abs(hilbert(y6))
    y6 = y6[:, ::M]
    y6 = y6[:N//2, :]

    ZTW_HNGD_SPEC = y6

    y5, y6 = None, None

    return ZTW_SPEC, ZTW_HNGD_SPEC

def gd_fns(x, N):
    y = (np.arange(1, x.shape[0] + 1)[:, np.newaxis] * np.ones((1, x.shape[1]))) * x
    X = fft(x, N)
    Y = fft(y, N)
    NGD = np.real(X) * np.real(Y) + np.imag(X) * np.imag(Y)
    
    X, Y = None, None
    
    return NGD

# if __name__ == '__main__':
#     data = np.array([-1.0891, 0.0326, 0.5525, 1.1006, 1.5442, 0.0859, -1.4916,-0.7423, -1.0616, 2.3505, -0.6156, 0.7481, -0.1924, 0.8886, -0.7648, -1.4023, -1.4224, 0.4882, -0.1774, -0.1961])
#     fs = 16000
#     ZTW_SPEC, ZTW_HNGD_SPEC  = zero_time_wind_spectrum(data, fs)
#     print("ZTW_SPEC:")
#     print(ZTW_SPEC[40][10])

#     print("ZTW_HNGD_SPEC:")
#     print(ZTW_HNGD_SPEC[40][10])
    
# # # ZTW_SPEC, ZTW_HNGD_SPEC = wind(audio_file_path)

# # # print("ZTW_SPEC:")
# # # print(ZTW_SPEC[40][10])

# # # print("ZTW_HNGD_SPEC:")
# # # print(ZTW_HNGD_SPEC[40][10])

