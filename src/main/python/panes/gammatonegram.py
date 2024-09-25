import numpy as np

from panes.base import Pane_Base
from dependencies.gammatonegram.main import gammatonegram

class Pane_Gammatonegram(Pane_Base):
    def __init__(self, data, fs, resampled_data, resampled_fs):
        super().__init__(data, fs, resampled_data, resampled_fs)
        self._pane_name = 'Gammatonegram'

    def _generate_plot(self):
        YG = gammatonegram(self._resampled_data, self._resampled_fs)
        f_bins = YG.shape[0]
        fg = np.arange(0, f_bins) * self._resampled_fs / (2 * f_bins)
        time_bins_g = YG.shape[1]
        ts_g = np.arange(0, time_bins_g) * len(self._resampled_data) / time_bins_g
        ts_g = ts_g / self._resampled_fs
        T_g, F_g = np.meshgrid(ts_g, fg)
        self._ax.pcolormesh(T_g, F_g, np.abs(YG), shading='auto')