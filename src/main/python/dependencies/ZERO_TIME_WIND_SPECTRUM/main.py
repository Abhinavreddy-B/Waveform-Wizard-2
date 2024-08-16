# # import ctypes

# # import librosa
# # import numpy as np

# # # wav = np.array([-1.0891, 0.0326, 0.5525, 1.1006, 1.5442, 0.0859, -1.4916, 
# # #-0.7423, -1.0616, 2.3505, -0.6156, 0.7481, -0.1924, 0.8886, 
# # #-0.7648, -1.4023, -1.4224, 0.4882, -0.1774, -0.1961])

# # my_library = ctypes.CDLL('./lib/ZERO_TIME_WIND_SPECTRUM_Wrapper/libZERO_TIME_WIND_SPECTRUM.so')  # Update with the actual path

# # my_library.ZERO_TIME_WIND_SPECTRUM_entrypoint.argtypes = [
# #     ctypes.POINTER(ctypes.c_double),   # wav_ptr
# #     ctypes.c_double,                   # fs
# #     ctypes.c_double,                   # N
# #     ctypes.c_double,                   # M
# #     ctypes.POINTER(ctypes.c_double),   # ZTW_SPEC
# #     ctypes.POINTER(ctypes.c_double),   # ZTW_HNGD_SPEC
# # ]

# # my_library.ZERO_TIME_WIND_SPECTRUM_entrypoint.restype = None  
# # my_library.free_array.argtypes = [ctypes.POINTER(ctypes.c_double)]  # array
# # my_library.free_array.restype = None

# # def ZERO_TIME(audio_file):
# #     y, sr = librosa.load(audio_file, sr=None)
# #     wav = y.astype(ctypes.c_double)
# #     print(wav.shape)
# #     data_size = wav.shape[0]
# #     data_ptr = (ctypes.c_double * data_size)(*wav)
    
# #     ZTW_SPEC  = ctypes.POINTER(ctypes.c_double)()
# #     ZTW_HNGD_SPEC  = ctypes.POINTER(ctypes.c_double)()

# #     result_size = ctypes.c_int()

# #     N = 4
# #     M = 1
# #     fs = float(sr)
    
# #     my_library.ZERO_TIME_WIND_SPECTRUM_entrypoint(
# #         wav.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
# #         ctypes.c_double(fs),  # Provide a value for 'fs'
# #         ctypes.c_double(N),
# #         ctypes.c_double(M),
# #         ctypes.byref(ZTW_SPEC),
# #         ctypes.byref(ZTW_HNGD_SPEC)
# #     )

# #     ZTW_SPEC_final = np.zeros(result_size.value, dtype=np.double)
# #     ZTW_HNGD_SPEC_final = np.zeros()
# #     for i in range(N/2):
# #         result[i] = result_array[i]
    
# #     lib.free_array(result_array)

# #     return ZTW_SPEC, ZTW_HNGD_SPEC

# # audio_file_path = '../../../samples/audio1.wav'
# # #audio_file_path = '/home/sasidhar/Sem6/BTP/Waveform-Wizard/samples/audio1.wav'

# # ZTW_SPEC, ZTW_HNGD_SPEC = ZERO_TIME(audio_file_path)

# # # Display the results
# # print("ZTW_SPEC:")
# # print(round(ZTW_SPEC[1][51760], 4))

# # # print("ZTW_HNGD_SPEC:")
# # # print(ZTW_HNGD_SPEC)

# import ctypes

# import numpy as np

# # Load the shared library
# lib = ctypes.CDLL("./lib/ZERO_TIME_WIND_SPECTRUM_Wrapper/libZERO_TIME_WIND_SPECTRUM.so")  # Update with the actual library name

# # Define the argument and return types for ZERO_TIME_WIND_SPECTRUM_entrypoint
# lib.ZERO_TIME_WIND_SPECTRUM_entrypoint.argtypes = [
#     ctypes.POINTER(ctypes.c_double),    # wav
#     ctypes.c_double,                     # fs
#     ctypes.c_double,                     # N
#     ctypes.c_double,                     # M
#     ctypes.POINTER(ctypes.POINTER(ctypes.c_double)),  # ZTW_SPEC
#     ctypes.POINTER(ctypes.POINTER(ctypes.c_double))   # ZTW_HNGD_SPEC
# ]
# lib.ZERO_TIME_WIND_SPECTRUM_entrypoint.restype = None

# lib.free_array.argtypes = [ctypes.POINTER(ctypes.c_double)]  # array
# lib.free_array.restype = None

# # Define a function to call ZERO_TIME_WIND_SPECTRUM_entrypoint and handle the memory properly
# def zero_time_wind_spectrum(wav, fs, N, M):
#     wav_size = len(wav)
#     wav_ptr = (ctypes.c_double * wav_size)(*wav)

#     ZTW_SPEC_ptr = ctypes.POINTER(ctypes.c_double)()
#     ZTW_HNGD_SPEC_ptr = ctypes.POINTER(ctypes.c_double)()

#     lib.ZERO_TIME_WIND_SPECTRUM_entrypoint(
#         wav_ptr,
#         ctypes.c_double(fs),
#         ctypes.c_double(N),
#         ctypes.c_double(M),
#         ctypes.byref(ZTW_SPEC_ptr),
#         ctypes.byref(ZTW_HNGD_SPEC_ptr)
#     )

#     ZTW_SPEC_size = int(wav_size / M)
#     ZTW_HNGD_SPEC_size = int(wav_size / M)

#     ZTW_SPEC = np.zeros(ZTW_SPEC_size, dtype=np.double)
#     ZTW_HNGD_SPEC = np.zeros(ZTW_HNGD_SPEC_size, dtype=np.double)

#     for i in range(ZTW_SPEC_size):
#         ZTW_SPEC[i] = ZTW_SPEC_ptr[i]

#     for i in range(ZTW_HNGD_SPEC_size):
#         ZTW_HNGD_SPEC[i] = ZTW_HNGD_SPEC_ptr[i]

#     lib.free_array(ZTW_SPEC_ptr)
#     lib.free_array(ZTW_HNGD_SPEC_ptr)

#     return ZTW_SPEC, ZTW_HNGD_SPEC

# if __name__ == "__main__":
#     # Example usage
#     wav = np.random.rand(20)  # Replace with actual values
#     fs = 1000.0  # Replace with actual value
#     N = 256.0  # Replace with actual value
#     M = 2.0  # Replace with actual value

#     ZTW_SPEC, ZTW_HNGD_SPEC = zero_time_wind_spectrum(wav, fs, N, M)

#     print("ZTW_SPEC:")
#     print(ZTW_SPEC)

#     print("ZTW_HNGD_SPEC:")
#     print(ZTW_HNGD_SPEC)

import ctypes

import librosa
import matplotlib.pyplot as plt
import numpy as np

lib = ctypes.CDLL('./dependencies/ZERO_TIME_WIND_SPECTRUM/lib/TIME_entrypoint_Wrapper/libTIME.so')  

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
    
if __name__ == '__main__':
    data = np.array([-1.0891, 0.0326, 0.5525, 1.1006, 1.5442, 0.0859, -1.4916,-0.7423, -1.0616, 2.3505, -0.6156, 0.7481, -0.1924, 0.8886, -0.7648, -1.4023, -1.4224, 0.4882, -0.1774, -0.1961])
    fs = 16000
    plot_ztws(data, fs)
    
# ZTW_SPEC, ZTW_HNGD_SPEC = wind(audio_file_path)

# print("ZTW_SPEC:")
# print(ZTW_SPEC[40][10])

# print("ZTW_HNGD_SPEC:")
# print(ZTW_HNGD_SPEC[40][10])
