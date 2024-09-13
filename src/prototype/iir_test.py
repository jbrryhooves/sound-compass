import time
from typing import Tuple
import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
from beamformer_iir import MicrophoneBeamFilter
import constants





N = 1   # order of the IIR all pass filter
t = np.arange(0,1,1/48000)
# print(mic)
delay_samples = 0#3.9/20
n = np.arange(0,N) 

# Calculate the IIR filter coeficients https://eeweb.engineering.nyu.edu/iselesni/EL713/zoom/allpass.pdf
d = (N-n)*(N-n-delay_samples)/(n+1)/(n+1+delay_samples)

filter = MicrophoneBeamFilter()
filter._denominators = np.cumprod(np.concatenate(([1],d)))
filter._numerators = np.flip(filter._denominators[1:])
filter.working_buffer = np.cos(100*2*np.pi*t)
zi = signal.lfilter_zi(filter._numerators, filter._denominators)
filtered_signal = signal.lfilter(filter._numerators, filter._denominators, filter.working_buffer)

plt.plot(filter.working_buffer)
plt.plot(filtered_signal)
plt.show()
