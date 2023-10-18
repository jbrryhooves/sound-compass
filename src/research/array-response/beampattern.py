import numpy as np
import matplotlib.pyplot as plt

def generate_steered_beam_pattern(array_radius, wavelength, steering_angle, num_microphones, frequency, theta_range):
    # Constants
    c = 343  # Speed of sound in m/s

    # Calculate the wavenumber
    k = 2 * np.pi * frequency / c

    # Define the array geometry
    microphone_positions = np.array([
        [array_radius * np.cos(2 * np.pi * i / num_microphones), array_radius * np.sin(2 * np.pi * i / num_microphones)]
        for i in range(num_microphones)
    ])

    # Calculate the steering delays
    steering_delays = np.dot(microphone_positions, np.array([np.cos(steering_angle), np.sin(steering_angle)])) / c

    # Calculate the array response
    theta = np.linspace(0, 2 * np.pi, 1000)
    array_response = np.zeros_like(theta, dtype=complex)

    for i, angle in enumerate(theta):
        micsSigs = np.exp(-1j * k * steering_delays * np.cos(angle))
        array_response[i] = np.sum(np.exp(-1j * k * steering_delays * np.cos(angle)))

    return theta, 20 * np.log10(np.abs(array_response))

def plot_beam_pattern(theta, beam_pattern):
    plt.figure(figsize=(8, 8))
    plt.polar(theta, beam_pattern, "-b", lw=2)
    plt.title("Steered Beam Pattern (Delay and Sum)")
    plt.ylim(-.3, 0)  # Adjust the y-axis limits as needed
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    array_radius = 0.1  # Radius of the circular microphone array in meters
    wavelength = 0.1  # Wavelength of the signal in meters
    steering_angle = np.pi / 4  # Steering angle in radians
    num_microphones = 8  # Number of microphones in the array
    frequency = 3000  # Frequency of the signal in Hz
    theta_range = np.linspace(0, 2 * np.pi, 1000)  # Range of angles for plotting

    theta, beam_pattern = generate_steered_beam_pattern(array_radius, wavelength, steering_angle, num_microphones, frequency, theta_range)
    plot_beam_pattern(theta, beam_pattern)
