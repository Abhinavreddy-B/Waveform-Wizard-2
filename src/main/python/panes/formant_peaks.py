import numpy as np
from scipy.signal import spectrogram

from panes.base import Pane_Base
from dependencies.pitch_srh.main import pitch_srh
from dependencies.formant_CGDZP.main import formant_CGDZP

class Pane_FormantPeaks(Pane_Base):
    def __init__(self, *args):
        super().__init__(*args)
        self._pane_name = 'Formant Peaks'

    def _generate_plot(self):
        f0min = 80
        f0max = 500
        hopsize = 10
                
        F0s, VUVDecisions, _, _ = pitch_srh(self._data, self._fs, f0min, f0max, hopsize)
        F0s = F0s * VUVDecisions
        
        _, _, s1 = spectrogram(self._data, self._fs, nperseg=160, noverlap=80, nfft=512)
        formantPeaks, t_analysis = formant_CGDZP(self._data, self._fs)
        F1 = formantPeaks[:len(F0s), 0] * VUVDecisions
        F2 = formantPeaks[:len(F0s), 1] * VUVDecisions
        F3 = formantPeaks[:len(F0s), 2] * VUVDecisions

        F1[F1 < np.mean(F1) / 10] = np.nan
        F2[F2 < np.mean(F2) / 10] = np.nan
        F3[F3 < np.mean(F3) / 10] = np.nan
        
        freq_bins_sp1 = s1.shape[0]
        fsn = self._fs / 2
        fs_sp1 = np.arange(1, freq_bins_sp1 + 1) * fsn / freq_bins_sp1
        time_bins_sp1 = s1.shape[1]
        ts_sp1 = np.arange(0, time_bins_sp1) * len(self._data) / (time_bins_sp1 - 1)
        ts_sp1 = ts_sp1 / self._fs
        T_sp1, F_sp1 = np.meshgrid(ts_sp1, fs_sp1)

        
        print(T_sp1.shape, F_sp1.shape, ts_sp1.shape, fs_sp1.shape, s1.shape)
        self._ax.pcolormesh(T_sp1, F_sp1, np.abs(s1), cmap='gray')

        tf0 = np.arange(len(F0s)) * 0.01
        self._ax.plot(tf0, F0s, 'r*', markersize=3)

        debug = self._ax.plot(t_analysis[:len(VUVDecisions)], F1, 'yx', linewidth=3, markersize=6)
        # print(debug, debug[0].get_marker(), debug[0].get_markerfacecolor())
        self._ax.plot(t_analysis[:len(VUVDecisions)], F2, 'rx', linewidth=3, markersize=6)
        self._ax.plot(t_analysis[:len(VUVDecisions)], F3, 'gx', linewidth=3, markersize=6)

        self._ax.set_xlabel('Time')
        self._ax.set_ylabel('Frequency')   