import numpy as np
from scipy.signal import spectrogram

from panes.base import Pane_Base

class Pane_Spectrogram(Pane_Base):
    def __init__(self, *args):
        super().__init__(*args)
        self._pane_name = 'Spectrogram'

    def _generate_plot(self):
        f, t, Sxx = spectrogram(self._data, nfft=self._fs, window='hamming', nperseg=160, noverlap=80)
        
        t = t/max(t)
        t = t * len(self._data)/self._fs
        print('DEBUG:', t, len(t))
        self._ax.pcolormesh(t, f, 10 * np.log10(Sxx))
        self._ax.set_ylabel('Frequency [Hz]')
        self._ax.set_xlabel('Time [sec]')