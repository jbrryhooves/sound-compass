
import numpy as np
import matplotlib.pyplot as plt

# This script synthesises the signals from a circular array and plots the beam pattern
# Heavily inspired and adapted from: https://pysdr.org/content/doa.html 


c = 343  # Speed of sound in m/s

N = 10000 # number of samples to simulate
sample_rate = 48000
NUM_MICS_outer = 8 # number of samples to simulate
NUM_MICS_inner = 5 # number of samples to simulate
NUM_MICS = NUM_MICS_outer + NUM_MICS_inner # number of samples to simulate
ARRAY_RADIUS = 0.3    # meters
ARRAY_RADIUS_inner = 0.15    # meters

# Create a tone to act as the transmitter signal
t = np.arange(N)/sample_rate # time vector
f_tone = 15560
tx = np.exp(2j * np.pi * f_tone * t)

theta_degrees = 20 # direction of arrival (feel free to change this, it's arbitrary)
steering_angle_radians = theta_degrees / 180 * np.pi # convert to radians


# Define the array geometry
microphone_positions_outer = np.array([
    [ARRAY_RADIUS * np.cos(2 * np.pi * i_outer / NUM_MICS_outer), ARRAY_RADIUS * np.sin(2 * np.pi * i_outer / NUM_MICS_outer)]
    for i_outer in range(NUM_MICS_outer)
])
microphone_positions_inner =  np.array([
    [ARRAY_RADIUS_inner * np.cos(2 * np.pi * i / NUM_MICS_inner), ARRAY_RADIUS_inner * np.sin(2 * np.pi * i / NUM_MICS_inner)]
    for i in range(NUM_MICS_inner)
])

microphone_positions = np.concatenate((microphone_positions_outer, microphone_positions_inner))

def calculate_array_factor(steering_angle_deg: float, frequency_hz: float):

    steering_angle_radians = steering_angle_deg / 180 * np.pi # convert to radians

    # Calculate the steering delays
    steering_delays_s = np.dot(microphone_positions, np.array([np.cos(steering_angle_radians), np.sin(steering_angle_radians)])) / c
    steering_delays_wavelenths = steering_delays_s  / (1/frequency_hz)
    # print(f"steering_delays: {steering_delays_s}") 

    af = np.exp(-1j * np.pi * steering_delays_wavelenths) # array factor

    return af
    pass

array_factor = calculate_array_factor(theta_degrees, f_tone)
print(array_factor) # note that it's a 1x3, it's complex, and the first element is 1+0j

array_factor_mat = np.asmatrix(array_factor)
tx = np.asmatrix(tx)

realtime_mic_signals = array_factor_mat.T @ tx  # don't get too caught up by the transpose a, the important thing is we're multiplying the array factor by the tx signal
print(realtime_mic_signals.shape) # r is now going to be a 2D array, 1D is time and 1D is the spatial dimension

n = np.random.randn(NUM_MICS, N) + 1j*np.random.randn(NUM_MICS, N)
# realtime_mic_signals = realtime_mic_signals + 0.1*n # r and n are both 3x10000

plt.plot(np.asarray(realtime_mic_signals[0,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
plt.plot(np.asarray(realtime_mic_signals[1,:]).squeeze().real[0:200])
plt.plot(np.asarray(realtime_mic_signals[2,:]).squeeze().real[0:200])
plt.plot(np.asarray(realtime_mic_signals[3,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
plt.plot(np.asarray(realtime_mic_signals[4,:]).squeeze().real[0:200])
plt.plot(np.asarray(realtime_mic_signals[5,:]).squeeze().real[0:200])
plt.plot(np.asarray(realtime_mic_signals[6,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
plt.plot(np.asarray(realtime_mic_signals[7,:]).squeeze().real[0:200])

plt.show()

# generate beam response
theta_scan = np.linspace(0, 2 * np.pi, 1000) 
results = []
for theta_i in theta_scan:
    #print(theta_i)
    steering_angle_deg = theta_i * 180 / np.pi # convert to radians
    w = np.asmatrix(calculate_array_factor(steering_angle_deg, f_tone)) 
    r_weighted = np.conj(w) @ realtime_mic_signals # apply our weights corresponding to the direction theta_i
    r_weighted = np.asarray(r_weighted).squeeze() # get it back to a normal 1d numpy array
    # plt.plot(np.abs(r_weighted))[0:200]
    # plt.plot(r_weighted.real[0:200])

    results.append(np.mean(np.abs(r_weighted)**2)) # energy detector


# print angle that gave us the max value
print(theta_scan[np.argmax(results)] * 180 / np.pi) 


fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
ax.plot(theta_scan, results) # MAKE SURE TO USE RADIAN FOR POLAR
ax.set_theta_zero_location('N') # make 0 degrees point up
ax.set_theta_direction(-1) # increase clockwise
# ax.set_rgrids([0,2,4,6,8])
# ax.set_rlabel_position(22.5)  # Move grid labels away from other labels
plt.show()    
plt.show()    