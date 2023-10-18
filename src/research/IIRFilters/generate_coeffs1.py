import numpy as np
import scipy as sp
import scipy.signal as signal
import matplotlib.pyplot as plt
import timeit
sampling_frequency = 48000.0        # Sampling frequency in Hz
nyq = sampling_frequency * 0.5  # Nyquist frequency
passband_frequencies = (44.5, 56.1)  # Filter cutoff in Hz
stopband_frequencies = (1.0, 1000.0)
max_loss_passband = 1 # The maximum loss allowed in the passband
min_loss_stopband = 30 # The minimum loss allowed in the stopband
# x = np.random.random(size=(5000, 12500)) # Random data to be filtered

order, normal_cutoff = signal.buttord(passband_frequencies, stopband_frequencies, max_loss_passband, min_loss_stopband, fs=sampling_frequency)
# iir_b, iir_a = signal.butter(order, normal_cutoff, btype="bandpass", fs=sampling_frequency)
iir_b, iir_a = signal.butter(2, passband_frequencies, btype="bandpass", fs=sampling_frequency)
w, h = signal.freqz(iir_b, iir_a, worN=np.logspace(0, 4, 1000), fs=sampling_frequency)
plt.semilogx(w, 20 * np.log10(abs(h)))
plt.title('Butterworth IIR bandpass filter fit to constraints')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Amplitude [dB]')
plt.grid(which='both', axis='both')
plt.show()
plt.show()
