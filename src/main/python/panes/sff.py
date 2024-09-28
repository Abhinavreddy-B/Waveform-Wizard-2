import numpy as np

from panes.base import Pane_Base
from dependencies.SINGLE_FREQ_FILTER_FS.main import \
    SINGLE_FREQ_FILTER_FS as single_freq_filter_fs

class Pane_Sff(Pane_Base):
    def __init__(self, *args):
        super().__init__(*args)
        self._pane_name = 'SFF'

    def _generate_plot(self):
        env, _ = single_freq_filter_fs(self._resampled_data, self._resampled_fs, 20, 0, self._resampled_fs/2, 0.98)
        # env, _ = single_freq_filter_fs(self._resampled_data, self._resampled_fs)
        env = env.T

        freq_bins_s, time_bins_s = env.shape

        fsn = self._resampled_fs / 2
        fs_s = np.arange(1, freq_bins_s + 1) * fsn / freq_bins_s    

        ts_s = np.arange(1, time_bins_s + 1) * len(self._resampled_data) / time_bins_s
        ts_s = ts_s / self._resampled_fs

        T_s, F_s = np.meshgrid(ts_s, fs_s)

        self._ax.pcolormesh(T_s, F_s, np.abs(env), shading='auto')
        self._ax.set_xlabel('Time')
        self._ax.set_ylabel('Frequency')