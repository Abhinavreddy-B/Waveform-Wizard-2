import numpy as np

from panes.base import Pane_Base
from dependencies.constantQ.main import constantq

class Pane_ConstantQ(Pane_Base):
    def __init__(self, data, fs, resampled_data, resampled_fs):
        super().__init__(data, fs, resampled_data, resampled_fs)

    def _generate_plot(self):
        CC, fs_cq = constantq(self._resampled_data, self._resampled_fs)
        time_bins_cq = CC.shape[1]

        ts_cq = np.linspace(0, len(self._resampled_data) / self._resampled_fs, time_bins_cq)
        T_cq, F_cq = np.meshgrid(ts_cq, fs_cq)

        self._ax.pcolormesh(T_cq, F_cq, np.abs(CC), shading='auto')
        self._ax.set_xlabel('Time')
        self._ax.set_ylabel('Frequency')