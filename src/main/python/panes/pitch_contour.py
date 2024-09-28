import numpy as np

from panes.base import Pane_Base
from dependencies.pitch_srh.main import pitch_srh

class Pane_Contour(Pane_Base):
    def __init__(self, *args):
        super().__init__(*args)
        self._pane_name = 'Pitch Contour'

    def _generate_plot(self):
        f0min = 50
        f0max = 500
        hopsize = 5

        t = np.arange(len(self._data)) / self._fs

        F0s, VUVDecisions, _, _ = pitch_srh(self._data, self._fs, f0min, f0max, hopsize)
        F0s = F0s * VUVDecisions
        tf0 = np.linspace(0, t[-1], len(F0s))

        self._ax.plot(tf0, F0s, '*')