import numpy as np
import scipy as sp
import scipy.signal as signal
import matplotlib.pyplot as plt
import timeit

sampling_frequency = 48000.0  # Sampling frequency in Hz
nyq = sampling_frequency * 0.5  # Nyquist frequency

max_loss_passband = 0.15  # The maximum loss allowed in the passband
min_loss_stopband = 42.5  # The minimum loss allowed in the stopband

starting_band = 14
octaveBands = [
    # (-42db lower freq, pass band lower freq, centre freq , upper passband freq, -42dB upper freq)

    (8.183223617, 22.38721139, 25.11886432, 28.18382931, 47.39804102),  # band 14
    (10.30206816, 28.18382931, 31.6227766, 35.48133892, 59.67059831),
    (12.9695354, 35.48133892, 39.81071706, 44.66835922, 75.12083255),
    (16.3276777, 44.66835922, 50.11872336, 56.23413252, 94.57152505),
    (20.55532837, 56.23413252, 63.09573445, 70.79457844, 119.0584961),
    (25.87762523, 70.79457844, 79.43282347, 89.12509381, 149.8857663),
    (32.578, 89.12509381, 100, 112.2018454, 188.695),
    (41.01327207, 112.2018454, 125.8925412, 141.2537545, 237.5529306),
    (51.63265042, 141.2537545, 158.4893192, 177.827941, 299.061421),
    (65.0016557, 177.827941, 199.5262315, 223.8721139, 376.4960225),
    (81.83223617, 223.8721139, 251.1886432, 281.8382931, 473.9804102),
    (103.0206816, 281.8382931, 316.227766, 354.8133892, 596.7059831),
    (129.695354, 354.8133892, 398.1071706, 446.6835922, 751.2083255),
    (163.276777, 446.6835922, 501.1872336, 562.3413252, 945.7152505),
    (205.5532837, 562.3413252, 630.9573445, 707.9457844, 1190.584961),
    (258.7762523, 707.9457844, 794.3282347, 891.2509381, 1498.857663),
    (325.78, 891.2509381, 1000, 1122.018454, 1886.95),
    (410.1327207, 1122.018454, 1258.925412, 1412.537545, 2375.529306),
    (516.3265042, 1412.537545, 1584.893192, 1778.27941, 2990.61421),
    (650.016557, 1778.27941, 1995.262315, 2238.721139, 3764.960225),
    (818.3223617, 2238.721139, 2511.886432, 2818.382931, 4739.804102),
    (1030.206816, 2818.382931, 3162.27766, 3548.133892, 5967.059831),
    (1296.95354, 3548.133892, 3981.071706, 4466.835922, 7512.083255),
    (1632.76777, 4466.835922, 5011.872336, 5623.413252, 9457.152505),
    (2055.532837, 5623.413252, 6309.573445, 7079.457844, 11905.84961),
    (2587.762523, 7079.457844, 7943.282347, 8912.509381, 14988.57663),
    (3257.8, 8912.509381, 10000, 11220.18454, 18869.5),
    (4101.327207, 11220.18454, 12589.25412, 14125.37545, 23755.29306),
    (5163.265042, 14125.37545, 15848.93192, 17782.7941, 29906.1421),
    (6500.16557, 17782.7941, 19952.62315, 22387.21139, 37649.60225),    # band 43
]
print(len(octaveBands))
cmsis_coeffs = []

for band in range(len(octaveBands)):
    lowerStopBandFreq, lowerPassBandFreq, midFreq, upperPassBandFreq, upperStopbandFreq = octaveBands[band]

    # order, normal_cutoff = signal.buttord(
    #     [lowerPassBandFreq, upperPassBandFreq],
    #     [lowerStopBandFreq, upperStopbandFreq],
    #     max_loss_passband,
    #     min_loss_stopband,
    #     fs=sampling_frequency,
    # )
    # iir_b, iir_a = signal.butter(order, normal_cutoff, btype="bandpass", fs=sampling_frequency)


    # iir_b, iir_a = signal.butter(
    #     2, [lowerPassBandFreq, upperPassBandFreq], btype="bandpass", fs=sampling_frequency, output="ba"
    # )
    # w, h = signal.freqz(
    #     iir_b, iir_a, worN=np.logspace(0, 4, 1000), fs=sampling_frequency
    # )

    # returns an array of 2nd order stages.
    # ie. 
    # sos = [[b10, b11, b12, a10, a11, a12], [b20, b21, b22, a20, a21, a22], ... [bN0, bN1, bN2, aN0, aN1, aN2]]
    # Note: aN0 is alwayss1, and the cmsis methods assume this and so don't accept it int heir coefficient list. 
    secondOrderStageCoefficients = signal.butter(
        3, [lowerPassBandFreq, upperPassBandFreq], btype="bandpass", fs=sampling_frequency, output='sos'
    )
    w, h = signal.sosfreqz(
        secondOrderStageCoefficients, worN=np.logspace(0, 4, 1000), fs=sampling_frequency
    )

    # the CMSIS coefficient format assume aN0 = 0, and disregards it, and it also uses a different difference equation for the biqaud direct form II.
    # See: 
    #  - https://arm-software.github.io/CMSIS_5/DSP/html/group__BiquadCascadeDF2T.html
    #  - https://dsp.stackexchange.com/questions/79021/iir-design-scipy-cmsis-dsp-coefficient-format/79024#79024
    #  - https://gaidi.ca/weblog/implementing-a-biquad-cascade-iir-filter-on-a-cortex-m4/
    # So, to convert each 2nd order stage coeffecients to the CMSIS format:
    # - remove the aN0 column
    # - flip the sign of the remaining a coefs
    secondOrderStageCoefficients = np.delete(secondOrderStageCoefficients,3, axis=1)
    secondOrderStageCoefficients[:,3] *=  -1
    secondOrderStageCoefficients[:,4] *=  -1
    
    cmsis_coef = np.concatenate((secondOrderStageCoefficients[0], secondOrderStageCoefficients[1]))
    cmsis_coeffs.append(cmsis_coef)


    plt.semilogx(w, 20 * np.log10(abs(h)))
    plt.title("Butterworth IIR bandpass filter fit to constraints")
    plt.xlabel("Frequency [Hz]")
    plt.ylabel("Amplitude [dB]")
    plt.grid(which="both", axis="both")
    # plt.show()
    # plt.show()

    pass
print(f"// =================== Generated IIR Filters ================")

print(f"// @formatter:off")

for band in range(len(cmsis_coeffs)):
    print(f"static float32_t _biquad_state_band{band+starting_band}[IIR_ORDER] __attribute__ ((section (\".my_data\")));")
print(f"")

for band in range(len(cmsis_coeffs)):
    print(f"static float32_t _biquad_coeffs_band{band+starting_band}[5 * IIR_NUMSTAGES] __attribute__ ((section (\".my_data\"))) = {{", end="")
    print(", ".join(str(x) for x in cmsis_coeffs[band]), end="")
    print(f"}};")
print(f"")

for band in range(len(cmsis_coeffs)):
    print(f"static arm_biquad_cascade_df2T_instance_f32 const iir_instance_band{band+starting_band}  = {{ IIR_ORDER / 2, _biquad_state_band{band+starting_band}, _biquad_coeffs_band{band+starting_band} }};")
print(f"")


print(f"static const arm_biquad_cascade_df2T_instance_f32 * iir_instances[executive::audioProcessor::FILTER_BANDS] __attribute__ ((section (\".my_data\"))) = {{", end="")
for band in range(len(cmsis_coeffs)):
    print(f"&iir_instance_band{band+starting_band}, ", end="")
print(f"}};")

print(f"// filter variables for centre mic")
print(f"static float32_t _biquad_state_centre[IIR_ORDER] __attribute__ ((section (\".my_data\")));")
print(f"static float32_t _biquad_coeffs_centre[5 * IIR_NUMSTAGES] __attribute__ ((section (\".my_data\"))) = {{", end="")
print(", ".join(str(x) for x in cmsis_coeffs[band]), end="")
print(f"}};")
print(f"static arm_biquad_cascade_df2T_instance_f32 const iir_instance_centre __attribute__ ((section (\".my_data\"))) = {{ IIR_ORDER / 2, _biquad_state_centre, _biquad_coeffs_centre }};")



print(f"// @formatter:on")
print(f"// =============================================================")