import time
from typing import Tuple
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

import constants


class MicrophoneBeamFilter(object):

    _filter_state: any
    _numerators: np.ndarray
    _denominators: np.ndarray
    
    pass

class BeamFormer(object):


    _microphone_positions:list[dict[str, float]] = []
    _filter_states: dict[str, MicrophoneBeamFilter] = {}

    def __init__(self) -> None:

        pass    

    def update_mic_positions(self, mic_positions:list[dict[str, float]]):
        self._microphone_positions = mic_positions
        self._mic_positions_x = np.array([mic["x"] for mic in self._microphone_positions])
        self._mic_positions_y = np.array([mic["y"] for mic in self._microphone_positions])

        pass

    def _calculate_steering_vector(self, direction_deg: float) -> np.ndarray:
        direction_rad = direction_deg / 180 * np.pi  # convert to radians

        x = np.array([mic["x"] for mic in self._microphone_positions])
        y = np.array([mic["y"] for mic in self._microphone_positions])

        steering_vector = np.exp(
            # https://pysdr.org/content/doa.html#circular-arrays
            -2j * np.pi * (x * np.cos(direction_rad) + y * np.sin(direction_rad))
        )
        # steering_vector = steering_vector.reshape(-1, 1)  # Nrx1
        return steering_vector
        pass

    def do_fft(self, waveform:np.ndarray[np.complex128]) -> np.ndarray[np.complex128]:
        '''Generate a complex FFT of a single buffer'''

        fftOut = np.fft.fft(waveform)

        return fftOut
        pass

    def calculate_time_delay_s(self, mic_position:Tuple[float,float], steering_angle_deg:float)->float:
        '''calculate the time delay of the wave front at the specified angle between the mic position and the centre of the array (i.e. x=0,y=0) '''

        steering_angle_rad = steering_angle_deg / 180 * np.pi
        # calculate the time delay between the mic and the centre of the array.
        # A positive timedelay means the wave front hits the mic before the centre of the array.
        steering_delay_s = (mic_position[0] * np.cos(steering_angle_rad) + mic_position[1] * np.sin(steering_angle_rad)) / constants.c
        # steering_delay_s = np.dot(mic_position, np.array([np.cos(steering_angle_rad), np.sin(steering_angle_rad)])) / constants.c
        return steering_delay_s
        pass

    def calculate_time_delay_in_samples(self, time_delay_s:float)->float:
        '''convert the time delay into terms of the sample frequency'''
        return time_delay_s * constants.sample_rate

        pass


    def get_filter(self, mic_index:int, beam_angle_deg:float) -> MicrophoneBeamFilter:
        '''Get the running filter for a specific microphone for a specific beam. If one does not exist, a new one is created and initialised '''

        state_key = f"{mic_index}_{beam_angle_deg}"
        if state_key in self._filter_states:
            return self._filter_states[state_key]

        N = 5   # order of the IIR all pass filter
        mic = self._microphone_positions[mic_index]
        print(mic)
        delay_samples = self.calculate_time_delay_s((mic["x"],mic['y']), beam_angle_deg)
        n = np.arange(0,N-1) 

        # Calculate the IIR filter coeficients https://eeweb.engineering.nyu.edu/iselesni/EL713/zoom/allpass.pdf
        d = (N-n)*(N-n-delay_samples)/(n+1)/(n+1+delay_samples)
        _denominators = np.cumprod(np.concatenate(([1],d)))
        _numerators = np.flip(d)

        zi = signal.lfilter_zi(_numerators, _denominators)

        self._filter_states[f"{mic_index}_{beam_angle_deg}"] = MicrophoneBeamFilter()
        self._filter_states[f"{mic_index}_{beam_angle_deg}"]._filter_state = zi
        self._filter_states[f"{mic_index}_{beam_angle_deg}"]._numerators = _numerators
        self._filter_states[f"{mic_index}_{beam_angle_deg}"]._denominators = _denominators

        return self._filter_states[state_key]

        # z, _ = signal.lfilter(numerators, numerators, xn, zi=zi*xn[0])

        return zi
        pass

    def save_filter_state(self, mic_index:int, beam_angle_deg:float, filter_state):
        '''Save a running filter for a specific microphone for a specific beam. If one does not exist for this mic/beam angle combo, it is saved. '''

        self._filter_states[f"{mic_index}_{beam_angle_deg}"]._filter_state = filter_state
        pass

    def create_beam2(self, waveforms:np.ndarray[np.ndarray], beam_angle_deg:float) -> np.ndarray[float]:
        steering_vector = self.calculate_steering_vector(beam_angle_deg)
        waveform_weighted = steering_vector.conj().T @ waveforms # example of applying the weights to the received signal (i.e., perform the beamforming)

        print(beam_angle_deg)
        # for w in mic_signals:
        #     plt.plot(np.asarray(w).squeeze().real[0:]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix

        plt.plot(np.asarray(waveform_weighted).squeeze().real[0:], linestyle="dashed") # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        plt.plot(np.asarray(steering_vector).squeeze().real[0:], linestyle="dashed") # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        plt.show()

        return waveform_weighted

        pass

    def create_beam(self, waveforms:np.ndarray[np.ndarray], beam_angle_deg:float) -> np.ndarray[float]:
        '''Accept several buffers of audio and a beam angle, and return a single buffer of the desired beam'''

        # 1. do FFT of each channel
        ffts = [] #np.ndarray[np.complex128]
        for mic in waveforms:
            ffts.append(self.do_fft(mic))
            pass

        # # 3. Do the fftshift on the phase shift coefficients
        # timeDelayPhaseShift = np.fft.fftshift(timeDelayPhaseShift)

        # # 4. Multiply the fft data with the coefficients to apply the time shift
        # fftWithDelay = np.multiply(fftData, timeDelayPhaseShift)

        # # 5. Do the IFFT
        # shiftedWaveform = np.fft.ifft(fftWithDelay)

        shiftedWaveforms = []
        # 2. phase shift each FFT
        # samplePeriod = (tend - tstart) / (samples)
        # tDelayInSamples = tDelay / samplePeriod
        for i in range(len(ffts)):
            mic_pos = (self._mic_positions_x[i],self._mic_positions_y[i])
            delay_s =  self.calculate_time_delay_s(mic_pos, beam_angle_deg)
            delay_samples = self.calculate_time_delay_in_samples(delay_s)

            N = ffts[i].shape[0]
            k = np.linspace(0, N-1, N)
            timeDelayPhaseShift = np.exp(((-2*np.pi*1j*k*delay_samples)/(N)) + (delay_samples*np.pi*1j))

            timeDelayPhaseShift = np.fft.fftshift(timeDelayPhaseShift)

            fftWithDelay = np.multiply(ffts[i], timeDelayPhaseShift)

            shiftedWaveforms.append(np.fft.ifft(fftWithDelay))
            pass

        mic_signals = shiftedWaveforms
        combined_waveform = np.sum(mic_signals, axis=0)
        # plt.plot(np.asarray(waveforms[0]).squeeze().real[0:]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        # plt.plot(np.asarray(waveforms[1]).squeeze().real[0:])
        # plt.plot(np.asarray(waveforms[2]).squeeze().real[0:])
        # plt.show()

        time.sleep(0.1)
        print(beam_angle_deg)
        # for w in mic_signals:
        #     plt.plot(np.asarray(w).squeeze().real[0:]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix

        plt.plot(np.asarray(combined_waveform).squeeze().real[0:], linestyle="dashed") # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        plt.plot(np.asarray(mic_signals[0]).squeeze().real[0:]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        # # plt.show()
        plt.plot(np.asarray(waveforms[1]).squeeze().real[0:], linestyle="dotted") # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        plt.plot(np.asarray(mic_signals[1]).squeeze().real[0:])
        # # plt.show()
        plt.plot(np.asarray(waveforms[2]).squeeze().real[0:], linestyle="dotted") # the asarray and squeeze are just annoyances we have to do because we came from a matrix
        plt.plot(np.asarray(mic_signals[2]).squeeze().real[0:])

        plt.show()
        time.sleep(0.1)

        # 2.1 emit fft's?

        # 3. combine all FFT's

        # 4. do IFFT

        # 5. return single channel

        pass

    pass
