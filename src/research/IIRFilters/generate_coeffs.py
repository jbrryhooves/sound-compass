import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

b, a = signal.iirfilter(4, [30, 600], 
                        fs=48000,
                        btype='band', analog=False, ftype='butter')

w, h = signal.freqz(b, a, 1000, fs=48000*2*np.pi)
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.semilogx(w / (2*np.pi), 20 * np.log10(np.maximum(abs(h), 1e-5)))
ax.set_title('Chebyshev Type II bandpass frequency response')
ax.set_xlabel('Frequency [Hz]')
ax.set_ylabel('Amplitude [dB]')
# ax.axis((10, 1000, -20, 10))
ax.grid(which='both', axis='both')
plt.show()
plt.show()
