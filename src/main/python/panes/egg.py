import numpy as np

from panes.base import Pane_Base

class Pane_Egg(Pane_Base):
    def __init__(self, data, fs, resampled_data, resampled_fs):
        super().__init__(data, fs, resampled_data, resampled_fs)
        self._pane_name = 'EGG'

    def _generate_plot(self):
        self._ax.set_axis_off()
        self._ax.text(0.5, 0.5, '2nd channel Not available for this file', horizontalalignment='center', verticalalignment='center', fontsize=12)
        # if self.second_data is None:
        #     self._ax.set_axis_off()
        #     self._ax.text(0.5, 0.5, '2nd channel Not available for this file', horizontalalignment='center', verticalalignment='center', fontsize=12)
        # else:
        #     self._ax.plot(self.second_data)