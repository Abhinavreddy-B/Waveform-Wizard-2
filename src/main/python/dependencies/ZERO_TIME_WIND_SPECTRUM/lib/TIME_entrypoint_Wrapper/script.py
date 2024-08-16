import ctypes

import librosa
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

lib = ctypes.CDLL('./libTIME.so')  

lib.WIND.argtypes = [
    ctypes.POINTER(ctypes.c_double),   # data
    ctypes.c_int,                      # data_size
    ctypes.c_double,                   # fs
    ctypes.c_double,                   # N
    ctypes.c_double,                   # M
    ctypes.POINTER(ctypes.POINTER(ctypes.c_double)),  # result_ZTW_SPEC
    ctypes.POINTER(ctypes.POINTER(ctypes.c_double)),  # result_ZTW_HNGD_SPEC
    ctypes.POINTER(ctypes.c_int),       # result_ztwHngdSpec_size
    ctypes.POINTER(ctypes.c_int)       # result_ztwSpec_size
]

lib.WIND.restype = None  
lib.free_array.argtypes = [ctypes.POINTER(ctypes.c_double)]  # array
lib.free_array.restype = None

def wind(data, fs):

    #y = np.array([-1.0891, 0.0326, 0.5525, 1.1006, 1.5442, 0.0859, -1.4916,-0.7423, -1.0616, 2.3505, -0.6156, 0.7481, -0.1924, 0.8886, -0.7648, -1.4023, -1.4224, 0.4882, -0.1774, -0.1961])

    wav = data.astype(float)
    N = 120
    M = 1
    fs = float(fs)
    data_size = wav.shape[0]
    data_ptr = (ctypes.c_double * data_size)(*wav)

    result_ZTW_SPEC = ctypes.POINTER(ctypes.c_double)()
    result_ztwHngdSpec_size = ctypes.c_int()
    result_ZTW_HNGD_SPEC = ctypes.POINTER(ctypes.c_double)()
    result_ztwSpec_size = ctypes.c_int()

    lib.WIND(
        data_ptr,
        ctypes.c_int(data_size),
        ctypes.c_double(fs),  # Provide a value for 'fs'
        ctypes.c_double(N),
        ctypes.c_double(M),
        ctypes.byref(result_ZTW_SPEC), 
        ctypes.byref(result_ZTW_HNGD_SPEC),
        ctypes.byref(result_ztwSpec_size),
        ctypes.byref(result_ztwHngdSpec_size),
    )

    result_SPEC = np.zeros(result_ztwSpec_size.value, dtype=np.double)
    result_HNGD_SPEC = np.zeros(result_ztwHngdSpec_size.value, dtype=np.double)

    for i in range(result_ztwSpec_size.value):
        result_SPEC[i] = result_ZTW_SPEC[i]

    for i in range(result_ztwHngdSpec_size.value):
        result_HNGD_SPEC[i] = result_ZTW_HNGD_SPEC[i]
    
    lib.free_array(result_ZTW_SPEC)
    lib.free_array(result_ZTW_HNGD_SPEC)

    result_SPEC = result_SPEC.reshape((data_size, int(N/2))).T
    result_HNGD_SPEC = result_HNGD_SPEC.reshape((data_size, int(N/2))).T
    return result_SPEC, result_HNGD_SPEC

def plot_ztws_2d(T, F, ZTS):
    plt.pcolormesh(T, F, abs(ZTS), shading='auto')
    plt.colorbar(label='Magnitude')
    plt.xlabel('Time')
    plt.ylabel('Frequency')
    plt.title('Zero Time Wind Spectrum')
    plt.show()


def plot_ztws(data, fs):
    result_SPEC, result_HNGD_SPEC = wind(data, fs)
    
    time_bins = result_HNGD_SPEC.shape[1]
    tz = np.arange(time_bins) * len(data) / (time_bins - 1) / fs
    freq_bins = result_HNGD_SPEC.shape[0]
    fsn = fs / 2
    fz = np.arange(freq_bins) * fsn / (freq_bins - 1)
    
    T, F = np.meshgrid(tz, fz)
    
    print(T.shape, F.shape)    
    plot_ztws_2d(T, F, result_HNGD_SPEC)
    # fig = plt.figure()
    # ax = fig.add_subplot(111, projection='3d')
    
    # ax.plot_surface(T, F, np.abs(result_HNGD_SPEC), edgecolor='none', cmap='viridis')
    
    # ax.set_xlabel('Frequency (Hz)')
    # ax.set_ylabel('Time (s)')
    # ax.set_zlabel('Magnitude')
    
    # plt.show()

# audio_file_path = '/home/sasidhar/Sem6/BTP/Waveform-Wizard/samples/audio1.wav'
# y, sr = librosa.load(audio_file_path, sr=None)
# data = y.astype(float)
# fs = float(sr)
data = np.array([-1.0891, 0.0326, 0.5525, 1.1006, 1.5442, 0.0859, -1.4916,-0.7423, -1.0616, 2.3505, -0.6156, 0.7481, -0.1924, 0.8886, -0.7648, -1.4023, -1.4224, 0.4882, -0.1774, -0.1961])
fs = 16000
plot_ztws(data, fs)
# ZTW_SPEC, ZTW_HNGD_SPEC = wind(audio_file_path)

# print("ZTW_SPEC:")
# print(ZTW_SPEC[40][10])

# print("ZTW_HNGD_SPEC:")
# print(ZTW_HNGD_SPEC[40][10])
