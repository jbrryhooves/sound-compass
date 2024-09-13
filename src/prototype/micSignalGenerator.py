import time
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import constants
# import beamformer_fir as BF
import beamformer_iir as BF
import enum

# This script synthesises the signals from a circular array and plots the beam pattern
# Heavily inspired and adapted from: https://pysdr.org/content/doa.html


# https://dsp.stackexchange.com/questions/60476/how-to-do-fft-fractional-time-delay-solved

class AudioSource:
    """Generates a single stream of audio from a certain angle"""

    class NoiseTypes(enum.Enum):
        SIN=1
        WHITE=2
        CHIRP=3
        pass

    frequency: float
    level: float
    direction: float
    noise_type:NoiseTypes

    _phase: float = 0
    def __init__(self, frequency: float, level: float, direction: float, type:NoiseTypes = NoiseTypes.SIN) -> None:
        self.frequency = frequency
        self.level = level
        self.direction = direction
        self.noise_type = type
        pass

    def GetAudio(self, numberOfSamples: int) -> np.ndarray:
        if self.noise_type == self.NoiseTypes.SIN:
            t = (np.arange(numberOfSamples) + self._phase) / constants.sample_rate  # time vector
            # tx = self.level * np.exp(2j * np.pi * self.frequency * t)
            tx = self.level * np.sin(2 * np.pi * self.frequency * t)
            self._phase += numberOfSamples
            return tx
        elif self.noise_type == self.NoiseTypes.WHITE:
            
            tx = self.level * np.random.normal(0, 1, size=numberOfSamples)
            return tx


            pass
        pass

    pass


class MicSignalGenerator:
    """Generates a set of mic signals from a set of audio sources"""

    audioSources: list[AudioSource]
    _beamformer: BF.BeamFormer

    def __init__(self, audioSources: list[AudioSource], beamformer:BF.BeamFormer):
        self.audioSources = audioSources
        self._beamformer = beamformer
        pass


    # def _calculate_steering_vector(self, direction_deg: float) -> np.ndarray:
    #     direction_rad = direction_deg / 180 * np.pi  # convert to radians

    #     x = np.array([mic["x"] for mic in self.microphone_positions])
    #     y = np.array([mic["y"] for mic in self.microphone_positions])

    #     steering_vector = np.exp(
    #         # -2j * np.pi * (x * np.cos(direction_rad) + y * np.sin(direction_rad))

    #         # https://pysdr.org/content/doa.html#circular-arrays
    #         1j * np.pi * (x * np.cos(direction_rad) + y * np.sin(direction_rad))
    #     )
    #     steering_vector = steering_vector.reshape(-1, 1)  # Nrx1
    #     return steering_vector
    #     pass

    # def GenerateSignals(self):

    #     mic_signals :np.ndarray = np.zeros((constants.NUM_MICS, self.buffer_size), 'complex128' )
    #     for a in self.audioSources:
    #         source_signal = a.GetAudio(self.buffer_size)
    #         source_signal = source_signal.reshape(1,-1)
            
    #         steering_vector = self.calculate_steering_vector(a.direction)
    #         source_signals = steering_vector @ source_signal # Simulate the received signal X through a matrix multiply
    #         mic_signals += source_signals
    #         # print(mic_signals.shape)
    #         pass

    def GenerateSignals(self):

        mic_signals :np.ndarray = np.zeros((constants.NUM_MICS, constants.buffer_length), 'float' )
        for a in self.audioSources:
            source_signal = a.GetAudio(constants.buffer_length)
            
            _signals = _beamformer.generate_shifted_mic_signals(source_signal, a.direction)
            mic_signals += _signals
            # print(mic_signals.shape)
            pass

        return mic_signals
    

    pass

if __name__ == "__main__":

    _beamformer = BF.BeamFormer()

    sources = [
        # AudioSource(600,   1.0, 0, type=AudioSource.NoiseTypes.WHITE), 
        AudioSource(600,  1.0, 45), 
        # AudioSource(1900,  1.0, 60),
        # AudioSource(5900,  1.0, 100),
        # AudioSource(12900, 1.0, 190),
        # AudioSource(16900, 1.0, 270)
        ]
    
    generator = MicSignalGenerator(sources, _beamformer)
    mic_signals = generator.GenerateSignals()
    mic_signals = generator.GenerateSignals()
    mic_signals = generator.GenerateSignals()




    theta_scan = np.linspace(0, 360, 1024) # 1024 different thetas between -180 and +180 degrees
    results = []
    for i in range(3):
        mic_signals = generator.GenerateSignals()
        for angle in theta_scan:
            beam = _beamformer.create_beam(mic_signals, angle)
            
    
    mic_signals = generator.GenerateSignals()
    for angle in theta_scan:
        beam = _beamformer.create_beam(mic_signals, angle)
        # results.append(10*np.log10(np.var(beam)))
        # results.append(np.mean(np.abs(beam)**2))    # power
        # results.append(10*np.log10(np.mean(np.abs(beam)**2)))    # power dB
        results.append(10*np.log10(np.sqrt(np.mean(beam**2)))) # RMS dB
        # plt.plot(beam)
        # plt.show()
        pass
    results -= np.max(results)
    # results /= np.max(results)

    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.plot(theta_scan/180*np.pi, results) # MAKE SURE TO USE RADIAN FOR POLAR
    ax.set_theta_zero_location('E') # make 0 degrees point up
    ax.set_theta_direction(-1) # increase clockwise
    ax.set_rlabel_position(55)  # Move grid labels away from other labels
    plt.show()


    # THETA_COUNT = 180
    # FREQ_COUNT = THETA_COUNT

    # mesh_thetas = np.linspace(-180+360/THETA_COUNT, 180, THETA_COUNT) 
    # mesh_freq_range = np.linspace(20, 20000, FREQ_COUNT)

    # mesh_results = np.zeros((FREQ_COUNT, THETA_COUNT))

    # # generate a surface plot with the following axis:
    # # X = theta
    # # Y = freq
    # # Z = array output
    # for y, freq in enumerate(mesh_freq_range):
    #     print(f"Freq: {freq}")
    #     _, beam_pattern = generate_beam_pattern(freq, THETA_COUNT)
    #     bp = np.asarray(beam_pattern)
    #     array_results[y] = bp
        
    #     pass

