import time
from typing import Tuple
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

import constants


class MicrophoneBeamFilter(object):

    filter_taps: any
    working_buffer: np.ndarray
    buffer_index: np.ndarray
    delay:float = 0

    def __init__(self) -> None:
        self.filter_taps = []
        self.working_buffer = []
        self.buffer_index = 0
        pass
    
    pass

class BeamFormer(object):


    _microphone_positions:list[dict[str, float]] = []
    _filter_states: dict[str, MicrophoneBeamFilter] = {}
    _mic_generation_filter_states: dict[str, MicrophoneBeamFilter] = {}
    
    def __init__(self) -> None:
        # self._calculateArrayGeometry()
        self._calculateArrayGeometry_spiral()
        self._mic_positions_x = np.array([mic["x"] for mic in self._microphone_positions])
        self._mic_positions_y = np.array([mic["y"] for mic in self._microphone_positions])

        fig = plt.figure()
        ax = fig.add_subplot()
        c = ax.scatter(self._mic_positions_x, self._mic_positions_y)

        pass    

    def _calculateArrayGeometry_spiral(self):

        angle_increment = 3 * 360/constants.NUM_MICS
        final_radius = 1 # m

        for mic_index in range(constants.NUM_MICS):
            # self._microphone_positions = np.insert(self._microphone_positions, 0, {"x":0.0, "y":0.0})
            
            self._microphone_positions.append({
                    "x": mic_index/constants.NUM_MICS * final_radius
                    * np.cos(np.pi * angle_increment/180 * mic_index),
                    "y": mic_index/constants.NUM_MICS * final_radius
                    * np.sin(np.pi * angle_increment/180 * mic_index),
                }
            )

        pass


    def _calculateArrayGeometry(self):
        # Define the array geometry
        microphone_positions_outer = np.array(
            [
                {
                    "x": constants.ARRAY_RADIUS
                    * np.cos(2 * np.pi * i_outer / constants.NUM_MICS_outer),
                    "y": constants.ARRAY_RADIUS
                    * np.sin(2 * np.pi * i_outer / constants.NUM_MICS_outer),
                }
                for i_outer in range(constants.NUM_MICS_outer)
            ]
        )
        microphone_positions_inner = np.array(
            [
                {
                    "x": constants.ARRAY_RADIUS_inner
                    * np.cos(2 * np.pi * i / constants.NUM_MICS_inner),
                    "y": constants.ARRAY_RADIUS_inner
                    * np.sin(2 * np.pi * i / constants.NUM_MICS_inner),
                }
                for i in range(constants.NUM_MICS_inner)
            ]
        )

        self._microphone_positions = np.concatenate(
            (microphone_positions_outer, microphone_positions_inner)
        )
        
        # add the center mic
        self._microphone_positions = np.insert(self._microphone_positions, 0, {"x":0.0, "y":0.0})

        pass

    def update_mic_positions(self, mic_positions:list[dict[str, float]]):
        self._microphone_positions = mic_positions
        self._mic_positions_x = np.array([mic["x"] for mic in self._microphone_positions])
        self._mic_positions_y = np.array([mic["y"] for mic in self._microphone_positions])

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


    def generate_shifted_mic_signals(self, audio_source:np.ndarray, angle_deg:float)-> np.ndarray[np.ndarray]:
        '''Generate a set of shifted mic_signals from a single audio source. 
        Used for simulating an audio source as heard by the array'''

        assert(len(audio_source) == constants.buffer_length)
        
        mic_signals :np.ndarray = np.zeros((constants.NUM_MICS, constants.buffer_length), 'float' )

        for mic in range(len(self._microphone_positions)):
            filter = self.get_mic_generation_filter(mic, angle_deg)

            if filter.buffer_index >= 2:
                filter.working_buffer = np.concatenate((filter.working_buffer, audio_source))
                
                # perform filtering on the whole 3x buffer
                conv_signal = np.convolve(filter.working_buffer, filter.filter_taps) # apply filter
                
                # remove the extra sample created by the convolution (could also be done by adding mode='valid' to the convolve method above)
                conv_signal = conv_signal[constants.TAPS//2:-(constants.TAPS//2 -1)] # apply filter
                
                # extract the "middle" buffer by removing the extra samples from the con
                shifted_signal = conv_signal[constants.buffer_length:-constants.buffer_length]

                # t_buff = np.linspace(filter.buffer_index * buffer_length, (filter.buffer_index + 1) * buffer_length -1, buffer_length)/48000
                # plt.plot(t_buff, filter.working_buffer[buffer_length:-buffer_length])
                # plt.plot(t_buff, shifted_signal)
                # # plt.plot(shifted_signal[12000+delay:-(12000+delay)])
                # plt.title("Linear Chirp, f(0)=6, f(10)=1")
                # plt.xlabel('t (sec)')
                # plt.show()

                # remove the first samples in preparation for the next buffer
                filter.working_buffer = filter.working_buffer[constants.buffer_length:]
            else:
                # build up at least 2 buffer_length's worth of samples
                filter.working_buffer = np.concatenate((filter.working_buffer, audio_source))
                shifted_signal = np.zeros(constants.buffer_length)

            filter.buffer_index += 1

            mic_signals[mic] = shifted_signal

            pass

        return mic_signals
        pass

    def create_beam(self, mic_signals:np.ndarray[np.ndarray], angle_deg:float)->np.ndarray[float]:
        '''Generate a beam of audio from the mic signals'''

        beam_signal:np.ndarray = np.zeros(constants.buffer_length, 'float' )
        plot = False
        for channel_index in range(len(mic_signals)):

            filter = self.get_beam_filter(channel_index, angle_deg)

            if filter.buffer_index >= 2:
                # plot = True
                filter.working_buffer = np.concatenate((filter.working_buffer, mic_signals[channel_index]))
                
                # perform filtering on the whole 3x buffer
                conv_signal = np.convolve(filter.working_buffer, filter.filter_taps) # apply filter
                
                # remove the extra sample created by the convolution (could also be done by adding mode='valid' to the convolve method above)
                conv_signal = conv_signal[constants.TAPS//2:-(constants.TAPS//2 -1)] # apply filter
                
                # extract the "middle" buffer by removing the extra samples from the con
                shifted_signal = conv_signal[constants.buffer_length:-constants.buffer_length]

                # remove the first samples in preparation for the next buffer
                filter.working_buffer = filter.working_buffer[constants.buffer_length:]
            else:
                # build up at least 2 buffer_length's worth of samples
                filter.working_buffer = np.concatenate((filter.working_buffer, mic_signals[channel_index]))
                shifted_signal = np.zeros(constants.buffer_length)

            filter.buffer_index += 1
            beam_signal += shifted_signal
        
            if plot:
                plt.plot(shifted_signal)
            pass
    
        if plot:
            plt.show()

        return beam_signal
        pass

    def get_mic_generation_filter(self, mic_index:int, beam_angle_deg:float) -> MicrophoneBeamFilter:
        '''Get the running filter for a specific microphone for a specific beam. If one does not exist, a new one is created and initialised '''

        mic_beam_key = f"{mic_index}_{beam_angle_deg}"
        if mic_beam_key not in self._mic_generation_filter_states:
            mic = self._microphone_positions[mic_index]
            delay_s = self.calculate_time_delay_s((mic["x"],mic["y"]),beam_angle_deg) 
            delay_samples = self.calculate_time_delay_in_samples(delay_s) # fractional delay, in samples

            h = self.generate_filter_taps(delay_samples)
            self._mic_generation_filter_states[mic_beam_key] = MicrophoneBeamFilter()
            self._mic_generation_filter_states[mic_beam_key].filter_taps = h

            pass
        return self._mic_generation_filter_states[mic_beam_key]
    
        pass

    def get_beam_filter(self, mic_index:int, beam_angle_deg:float) -> MicrophoneBeamFilter:
        '''Get the running filter for a specific microphone for a specific beam. If one does not exist, a new one is created and initialised '''

        mic_beam_key = f"{mic_index}_{beam_angle_deg}"
        if mic_beam_key not in self._filter_states:
            mic = self._microphone_positions[mic_index]
            delay_s = self.calculate_time_delay_s((mic["x"],mic["y"]),beam_angle_deg) 
            delay_samples = self.calculate_time_delay_in_samples(delay_s) # fractional delay, in samples
            
            # note negative delay here
            h = self.generate_filter_taps(-delay_samples)
            self._filter_states[mic_beam_key] = MicrophoneBeamFilter()
            self._filter_states[mic_beam_key].filter_taps = h
            self._filter_states[mic_beam_key].delay = delay_samples


            pass
        return self._filter_states[mic_beam_key]

        pass


    def generate_filter_taps(self, delay_samples:float):
        n = np.arange(-constants.TAPS//2, constants.TAPS//2) # ...-3,-2,-1,0,1,2,3...
        h = np.sinc(n - delay_samples) # calc filter taps
        h *= np.hamming(constants.TAPS) # window the filter to make sure it decays to 0 on both sides
        h /= np.sum(h) # normalize to get unity gain, we don't want to change the amplitude/power
        return h

   
    pass
