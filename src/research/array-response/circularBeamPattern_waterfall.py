
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

# This script synthesises the signals from a circular array and plots the beam pattern
# Heavily inspired and adapted from: https://pysdr.org/content/doa.html 


c = 343  # Speed of sound in m/s

N = 10000 # number of samples to simulate
sample_rate = 48000
NUM_MICS_outer = 8 # number of samples to simulate
NUM_MICS_inner = 7 # number of samples to simulate
NUM_MICS = NUM_MICS_outer + NUM_MICS_inner # number of samples to simulate
ARRAY_RADIUS = 0.3    # meters
ARRAY_RADIUS_inner = 0.16    # meters


# Define the array geometry
microphone_positions_inner =  np.array([
    [ARRAY_RADIUS_inner * np.cos(2 * np.pi * i / NUM_MICS_inner), ARRAY_RADIUS_inner * np.sin(2 * np.pi * i / NUM_MICS_inner)]
    for i in range(NUM_MICS_inner)
])
microphone_positions_outer = np.array([
    [ARRAY_RADIUS * np.cos(2 * np.pi * i_outer / NUM_MICS_outer), ARRAY_RADIUS * np.sin(2 * np.pi * i_outer / NUM_MICS_outer)]
    for i_outer in range(NUM_MICS_outer)
])

microphone_positions = np.concatenate((microphone_positions_inner,microphone_positions_outer, ))


def generate_beam_pattern(freq: float, thetaCount:int):
    # Create a tone to act as the transmitter signal
    t = np.arange(N)/sample_rate # time vector
    source_tone = np.exp(2j * np.pi * freq * t)

    theta_degrees = 3 # direction of arrival of "source"
    steering_angle_radians = theta_degrees / 180 * np.pi # convert to radians


    array_factor = calculate_array_factor(theta_degrees, freq)
    # print(array_factor) # note that it's a 1x3, it's complex, and the first element is 1+0j

    array_factor_mat = np.asmatrix(array_factor)
    source_tone = np.asmatrix(source_tone)


    realtime_mic_signals = array_factor_mat.T @ source_tone  # don't get too caught up by the transpose a, the important thing is we're multiplying the array factor by the tx signal
    # print(realtime_mic_signals.shape) # r is now going to be a 2D array, 1D is time and 1D is the spatial dimension

    # if(freq> 18000):
    #     plt.plot(np.asarray(realtime_mic_signals[0,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
    #     plt.plot(np.asarray(realtime_mic_signals[1,:]).squeeze().real[0:200])
    #     plt.plot(np.asarray(realtime_mic_signals[2,:]).squeeze().real[0:200])
    #     plt.plot(np.asarray(realtime_mic_signals[3,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
    #     plt.plot(np.asarray(realtime_mic_signals[4,:]).squeeze().real[0:200])
    #     plt.plot(np.asarray(realtime_mic_signals[5,:]).squeeze().real[0:200])
    #     plt.plot(np.asarray(realtime_mic_signals[6,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
    #     plt.plot(np.asarray(realtime_mic_signals[7,:]).squeeze().real[0:200])

    #     plt.show()

    theta_scan = np.linspace(-1 * np.pi + np.pi/thetaCount, 1 * np.pi, thetaCount) 
    results = []
    for theta_i in theta_scan:
        #print(theta_i)
        steering_angle_deg = theta_i * 180 / np.pi # convert to radians
        w = np.asmatrix(calculate_array_factor(steering_angle_deg, freq)) 
        r_weighted = np.conj(w) @ realtime_mic_signals # apply our weights corresponding to the direction theta_i
        r_weighted = np.asarray(r_weighted).squeeze() # get it back to a normal 1d numpy array
        # plt.plot(np.abs(r_weighted))[0:200]
        # plt.plot(r_weighted.real[0:200])

        results.append(np.mean(np.abs(r_weighted)**2)) # energy detector

    return theta_scan, results
    pass





def calculate_array_factor(steering_angle_deg: float, frequency_hz: float):

    steering_angle_radians = steering_angle_deg / 180 * np.pi # convert to radians

    # Calculate the steering delays
    steering_delays_s = np.dot(microphone_positions, np.array([np.cos(steering_angle_radians), np.sin(steering_angle_radians)])) / c
    steering_delays_wavelenths = steering_delays_s  / (1/frequency_hz)
    # print(f"steering_delays: {steering_delays_s}") 

    sampleDelays = steering_delays_s*sample_rate
    # print(f"Max delay: {max(steering_delays_s)*1000}ms, {max(steering_delays_s)*sample_rate}samples ")
    # print(f"Steering delays: {np.round(sampleDelays)}samples ")
    # print(f"Steering delay errors: {sampleDelays - np.round(sampleDelays)}samples ")
    # print(f"Steering delay errors %: {(sampleDelays - np.round(sampleDelays))*100/max(sampleDelays)}samples ")

    af = np.exp(-1j * np.pi * steering_delays_wavelenths) # array factor

    return af
    pass

THETA_COUNT = 180
FREQ_COUNT = THETA_COUNT
mesh_thetas = np.linspace(-180+360/THETA_COUNT, 180, THETA_COUNT) 
mesh_freq_range = np.linspace(20, 20000, FREQ_COUNT)

array_results = np.zeros((FREQ_COUNT, THETA_COUNT))

# generate a surface plot with the following axis:
# X = theta
# Y = freq
# Z = array output
for y, freq in enumerate(mesh_freq_range):
    print(f"Freq: {freq}")
    _, beam_pattern = generate_beam_pattern(freq, THETA_COUNT)
    bp = np.asarray(beam_pattern)
    array_results[y] = bp
    
    pass

# def f(x, y):
#     return np.sin(np.sqrt(x ** 2 + y ** 2))


X, Y = np.meshgrid(mesh_thetas, mesh_freq_range)
# Z = f(X, Y)

fig = plt.figure()
ax = plt.axes(projection='3d')
ax.plot_surface(X, Y, array_results, 
                # rstride=1, cstride=1,
                # rcount=3, ccount=3,
                cmap= cm.coolwarm,
                linewidth=2, 
                # edgecolor='none',
                antialiased = False
                )
ax.set_title('surface')

ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
plt.show()
plt.show()


# fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
# ax.plot(theta_scan, beam_pattern) # MAKE SURE TO USE RADIAN FOR POLAR
# ax.set_theta_zero_location('N') # make 0 degrees point up
# ax.set_theta_direction(-1) # increase clockwise
# # ax.set_rgrids([0,2,4,6,8])
# # ax.set_rlabel_position(22.5)  # Move grid labels away from other labels
# plt.show()    



