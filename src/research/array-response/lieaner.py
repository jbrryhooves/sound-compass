import numpy as np
import matplotlib.pyplot as plt

sample_rate = 1e6
N = 10000 # number of samples to simulate

# Create a tone to act as the transmitter signal
t = np.arange(N)/sample_rate # time vector
f_tone = 0.02e6
tx = np.exp(2j * np.pi * f_tone * t)

d = 0.5 # half wavelength spacing
Nr = 3
theta_degrees = 20 # direction of arrival (feel free to change this, it's arbitrary)
theta = theta_degrees / 180 * np.pi # convert to radians

delays = d * np.arange(Nr) * np.sin(theta)
af = np.exp(-2j * np.pi * delays) # array factor
print(af) # note that it's a 1x3, it's complex, and the first element is 1+0j

af = np.asmatrix(af)
tx = np.asmatrix(tx)

r = af.T @ tx  # don't get too caught up by the transpose a, the important thing is we're multiplying the array factor by the tx signal
print(r.shape) # r is now going to be a 2D array, 1D is time and 1D is the spatial dimension

plt.plot(np.asarray(r[0,:]).squeeze().real[0:200]) # the asarray and squeeze are just annoyances we have to do because we came from a matrix
plt.plot(np.asarray(r[1,:]).squeeze().real[0:200])
plt.plot(np.asarray(r[2,:]).squeeze().real[0:200])
plt.show()

n = np.random.randn(Nr, N) + 1j*np.random.randn(Nr, N)
r = r + 0.1*n # r and n are both 3x10000

theta_scan = np.linspace(-1*np.pi, np.pi, 1000) # 1000 different thetas between -180 and +180 degrees
results = []
for theta_i in theta_scan:
    #print(theta_i)
    w = np.asmatrix(np.exp(-2j * np.pi * d * np.arange(Nr) * np.sin(theta_i))) # look familiar?
    r_weighted = np.conj(w) @ r # apply our weights corresponding to the direction theta_i
    r_weighted = np.asarray(r_weighted).squeeze() # get it back to a normal 1d numpy array
    plt.plot(r_weighted.real[0:200])
    results.append(np.mean(np.abs(r_weighted)**2)) # energy detector

# print angle that gave us the max value
print(theta_scan[np.argmax(results)] * 180 / np.pi) # 19.99999999999998

plt.plot(theta_scan*180/np.pi, results) # lets plot angle in degrees
plt.xlabel("Theta [Degrees]")
plt.ylabel("DOA Metric")
plt.grid()
plt.show()

fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
ax.plot(theta_scan, results) # MAKE SURE TO USE RADIAN FOR POLAR
ax.set_theta_zero_location('N') # make 0 degrees point up
ax.set_theta_direction(-1) # increase clockwise
ax.set_rgrids([0,2,4,6,8])
ax.set_rlabel_position(22.5)  # Move grid labels away from other labels
plt.show()