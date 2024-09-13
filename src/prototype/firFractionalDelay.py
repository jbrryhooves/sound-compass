import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
import math

from scipy.signal import chirp, spectrogram
import matplotlib.pyplot as plt


t = np.linspace(0, 1, 48000)

original_signal = chirp(t, f0=30, f1=20000, t1=1, method='linear')


delay = -30.30 # fractional delay, in samples
N = 200 #int(len(t)/2) # number of taps
n = np.arange(-N//2, N//2) # ...-3,-2,-1,0,1,2,3...
h = np.sinc(n - delay) # calc filter taps
h *= np.hamming(N) # window the filter to make sure it decays to 0 on both sides
h /= np.sum(h) # normalize to get unity gain, we don't want to change the amplitude/power


# The length of audio samples (per channel) received from the hardware
buffer_length = int(48000*0.01)
buffer_index = 0

# the buffer that holds 3 buffer_length's worth of samples. 
# The filtering is done only on the middle buffer
working_buffer = []

while buffer_index * buffer_length < len(t):
    if buffer_index >= 2:
        working_buffer = np.concatenate((working_buffer, original_signal[buffer_index*buffer_length:(buffer_index + 1) * buffer_length ]))
        
        # perform filtering on the whole 3x buffer
        conv_signal = np.convolve(working_buffer, h) # apply filter
        
        # remove the extra sample created by the convolution (could also be done by adding mode='valid' to the convolve method above)
        conv_signal = conv_signal[N//2:-(N//2 -1)] # apply filter
        
        # extract the "middle" buffer by removing the extra samples from the con
        shifted_signal = conv_signal[buffer_length:-buffer_length]

        t_buff = np.linspace(buffer_index * buffer_length, (buffer_index + 1) * buffer_length -1, buffer_length)/48000
        plt.plot(t_buff, working_buffer[buffer_length:-buffer_length])
        plt.plot(t_buff, shifted_signal)
        # plt.plot(shifted_signal[12000+delay:-(12000+delay)])
        plt.title("Linear Chirp, f(0)=6, f(10)=1")
        plt.xlabel('t (sec)')
        plt.show()

        # remove the first samples in preparation for the next buffer
        working_buffer = working_buffer[buffer_length:]
    else:
        # build up at least 2 buffer_length's worth of samples
        working_buffer = np.concatenate((working_buffer, original_signal[buffer_index*buffer_length:(buffer_index + 1) * buffer_length ]))
        shifted_signal = np.zeros(buffer_length)

    buffer_index += 1
    pass


# plt.plot(h)
# plt.show()


plt.plot(t, original_signal)
plt.plot(t, shifted_signal[N//2:-(N//2-1)])
# plt.plot(shifted_signal[12000+delay:-(12000+delay)])
plt.title("Linear Chirp, f(0)=6, f(10)=1")
plt.xlabel('t (sec)')
plt.show()
print("done")