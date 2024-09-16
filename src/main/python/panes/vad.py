import numpy as np

from panes.base import Pane_Base
from dependencies.voiced_unvoiced_own.main import voiced_unvoiced_own

class Pane_Vad(Pane_Base):
    def __init__(self, data, fs, resampled_data, resampled_fs):
        super().__init__(data, fs, resampled_data, resampled_fs)

    def _generate_plot(self):
        vuv, _ = voiced_unvoiced_own(self._data, self._fs)

        t = np.arange(len(self._data)) / self._fs

        self._ax.plot(t, vuv*0.6)
        self._ax.set_xlabel('Time')
        self._ax.set_ylabel('Voiced/Unvoiced')
        self._ax.set_ylim(0, 1)