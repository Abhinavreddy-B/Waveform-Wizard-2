import numpy as np

from panes.base import Pane_Base
from dependencies.ZERO_TIME_WIND_SPECTRUM.main import zero_time_wind_spectrum

class Pane_Ztws(Pane_Base):
    def __init__(self, data, fs, resampled_data, resampled_fs):
        super().__init__(data, fs, resampled_data, resampled_fs)
        self._pane_name = 'ZTWS'

    def _generate_plot(self):
        _, result_HNGD_SPEC = zero_time_wind_spectrum(self._resampled_data, self._resampled_fs)
    
        time_bins = result_HNGD_SPEC.shape[1]
        tz = np.arange(time_bins) * len(self._resampled_data) / (time_bins - 1) / self._resampled_fs
        freq_bins = result_HNGD_SPEC.shape[0]
        fsn = self._resampled_fs / 2
        fz = np.arange(freq_bins) * fsn / (freq_bins - 1)
        
        T, F = np.meshgrid(tz, fz)
        
        self._ax.pcolormesh(T, F, abs(result_HNGD_SPEC), shading='auto')
        # self.ax.colorbar(label='Magnitude')
        self._ax.set_xlabel('Time')
        self._ax.set_ylabel('Frequency')