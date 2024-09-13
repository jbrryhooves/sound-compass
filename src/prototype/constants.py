
c = 343  # Speed of sound in m/s

N = 1000 # number of samples to simulate
sample_rate = 48000
buffer_length = int(sample_rate * 0.01)  # 10 ms
NUM_MICS_outer = 3 # number of samples to simulate
NUM_MICS_inner = 0 # number of samples to simulate
NUM_MICS = NUM_MICS_outer + NUM_MICS_inner + 1 # number of samples to simulate
ARRAY_RADIUS = .3    # meters
ARRAY_RADIUS_inner = 0.1    # meters

TAPS = 500