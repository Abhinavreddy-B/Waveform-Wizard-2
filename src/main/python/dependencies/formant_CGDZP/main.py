import numpy as np
from scipy.fft import fft, ifft

def formant_CGDZP(wave, fs, frame_size=30, frame_shift=10):
    num_formants = 5
    num_formats_final = num_formants
    frame_size = int(fs / 1000 * frame_size)
    frame_shift = int(fs / 1000 * frame_shift)
    fs_lr = 2048  # lower resolution
    view_range = int(fs_lr / 3.2)
    r = 1.12
    rfix = r
    max_formant_delta = 250  # discontinuity constraint (in Hz.) in formant tracks

    n = np.arange(frame_size - 1)  # zeroPhase data has length of N-1

    scaling_freq_axis = np.linspace(0.5, 1.5, view_range) - 0.25
    ceps_smooth_select_percent = 3
    select_time_index = int(ceps_smooth_select_percent * fs_lr / 2 / 100)

    size_wave = len(wave)
    num_frames = (size_wave - frame_size) // frame_shift

    t_analysis = np.zeros(num_frames)
    black_win = np.blackman(frame_size)

    formant_peaks = []
    for kk in range(num_frames):
        speech_data = wave[kk * frame_shift:kk * frame_shift + frame_size]
        windowed_data = speech_data * black_win

        zero_phase_data = np.real(ifft(np.abs(fft(np.diff(windowed_data)))))  # obtain zero-pha version

        num_peaks = 0
        r = rfix  # the following loop searches for the R value where we have numFormants number of formats
        while num_peaks != num_formants and 1.01 < r < 1.25:
            # chirp z-transform calculation using fft, multiplication with an exponential function is sufficient
            exponential_envelope = np.exp(np.log(1 / r) * n)  # this is needed for computation of z-transform using fft
            fourier_trans = fft(zero_phase_data * exponential_envelope, fs_lr)
            ang_fft = np.angle(fourier_trans[:view_range])
            chirp_group_delay = -np.diff(ang_fft)

            peak_index = formant_peak_pick(chirp_group_delay, 1)
            num_peaks = len(peak_index)
            if num_peaks > num_formants and r >= rfix:
                r += 0.01
                peak_index = peak_index[:num_formants]
            elif num_peaks < num_formants and r <= rfix:
                r -= 0.01
                peak_index = np.pad(peak_index, (0, num_formants - num_peaks), mode='constant')
            else:
                break

        if num_peaks > num_formants:
            peak_index = peak_index[:num_formants]
        elif num_peaks < num_formants:
            peak_index = np.pad(peak_index, (0, num_formants - len(peak_index)), mode='constant')
        formant_peaks.append(peak_index)

        t_analysis[kk] = (kk * frame_shift + 1 + kk * frame_shift + frame_size) / 2 / fs

    formant_peaks = np.round(np.array(formant_peaks) * fs / fs_lr).astype(int)

    if True:  # filtering
        # form a matrix of distance cost
        formant_peaks_cost = np.zeros_like(formant_peaks)
        for kk in range(2, num_frames - 2):
            pre_pre_peaks = formant_peaks[kk - 2, :]
            post_post_peaks = formant_peaks[kk + 2, :]
            pre_peaks = formant_peaks[kk - 1, :]
            post_peaks = formant_peaks[kk + 1, :]
            current_peaks = formant_peaks[kk, :]
            current_peaks_cost = np.zeros_like(current_peaks)  # cost will correspond to average distance from closest match to neighbours
            for mm in range(num_formants):
                if current_peaks[mm] == 0:
                    current_peaks_cost[mm] = fs / 2
                else:
                    # search for closest matches
                    distance_array_pre = np.sort(np.abs(pre_peaks - current_peaks[mm]))
                    distance_array_post = np.sort(np.abs(post_peaks - current_peaks[mm]))
                    distance_array_pre_pre = np.sort(np.abs(pre_pre_peaks - current_peaks[mm]))
                    distance_array_post_post = np.sort(np.abs(post_post_peaks - current_peaks[mm]))
                    all_distances = [(distance_array_pre[0] + distance_array_post[0]) / 2, distance_array_pre[0], distance_array_post[0]]
                    all_distances2 = [(distance_array_pre_pre[0] + distance_array_post_post[0]) / 2, distance_array_pre_pre[0], distance_array_post_post[0]]
                    current_peaks_cost[mm] = min(all_distances + all_distances2)
            formant_peaks_cost[kk, :] = current_peaks_cost

        # make decision based on costs
        for kk in range(num_frames):
            for mm in range(num_formants):
                if formant_peaks_cost[kk, mm] > max_formant_delta:
                    formant_peaks[kk, mm] = 0

    if True:  # replace possible continuation values instead of zero values
        for kk in range(1, num_frames - 1):
            current_peaks = formant_peaks[kk, :]
            index = np.nonzero(current_peaks)[0]  # finds non-zero elements
            non_zero_formants = current_peaks[index]
            num_non_zero_formants = len(index)
            num_zero_formants = num_formants - num_non_zero_formants
            if num_non_zero_formants < num_formants and index.size > 0:
                possible_values = np.sort(np.concatenate((formant_peaks[kk - 1, :], formant_peaks[kk + 1, :])))
                possible_values = possible_values[possible_values != 0]  # discard zero entries
                possible_candidates = []  # candidate for the zero valued formant
                for mm in range(len(possible_values)):
                    distance_array = np.sort(np.abs(non_zero_formants - possible_values[mm]))
                    if not distance_array.size or distance_array[0] > max_formant_delta:  # this possible value not found in the current vector
                        possible_candidates.append(possible_values[mm])
                possible_candidates = np.array(possible_candidates)

                # choose among possible candidates
                len_possible_candidates = len(possible_candidates)
                if len_possible_candidates <= num_zero_formants:
                    current_peaks = np.sort(np.concatenate((non_zero_formants, possible_candidates, np.zeros(num_zero_formants - len_possible_candidates))))
                elif num_zero_formants == 1:  # the most common case
                    index = np.argsort(np.diff(possible_candidates))[0]
                    current_peaks = np.sort(np.concatenate((non_zero_formants, [possible_candidates[index]])))
                elif num_zero_formants < len_possible_candidates:
                    possible_candidates = possible_candidates[:num_zero_formants]
                    current_peaks = np.sort(np.concatenate((non_zero_formants, possible_candidates)))
                formant_peaks[kk, :] = current_peaks

    return formant_peaks, t_analysis

def formant_peak_pick(diff_phase, min_peak_dist):
    len_diff_phase = len(diff_phase)

    peak_index = []
    for kk in range(6, len_diff_phase - 6):
        if (diff_phase[kk] >= diff_phase[kk - 1] and diff_phase[kk] >= diff_phase[kk + 1]) and \
           (diff_phase[kk] >= diff_phase[kk - 2] and diff_phase[kk] >= diff_phase[kk + 2]) and \
           (diff_phase[kk] > diff_phase[kk - 3] and diff_phase[kk] > diff_phase[kk + 3]) and \
           (diff_phase[kk] > diff_phase[kk - 4] and diff_phase[kk] > diff_phase[kk + 4]) and \
           (diff_phase[kk] > diff_phase[kk - 5] and diff_phase[kk] > diff_phase[kk + 5]):
            peak_index.append(kk)

    len_peak_ind = len(peak_index)
    kk = 1
    while kk < len_peak_ind:
        if peak_index[kk] - peak_index[kk - 1] < min_peak_dist:
            peak_index[kk] = int((peak_index[kk] * diff_phase[kk] + peak_index[kk - 1] * diff_phase[kk - 1]) / (diff_phase[kk] + diff_phase[kk - 1]))
            peak_index = peak_index[:kk] + peak_index[kk + 1:]
            len_peak_ind -= 1
        else:
            kk += 1

    return peak_index


