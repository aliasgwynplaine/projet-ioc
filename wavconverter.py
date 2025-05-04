import numpy as np
import wave
import scipy.fftpack

file_path = "Mario_Theme_Song.wav"
with wave.open(file_path, "rb") as wf:
    rate = wf.getframerate()
    nframes = wf.getnframes()
    data = wf.readframes(nframes)

data = np.frombuffer(data, dtype=np.int8)

time_frame = 0.075
freq_size = int(rate * time_frame)
tao = 20

prv_freq = None
curr_time = 0
freqs = []
times = []

for start in range(0, len(data), freq_size):
    frame = data[start:start + freq_size]

    fft_result = np.abs(scipy.fftpack.fft(frame))
    frequencies = np.fft.fftfreq(len(fft_result), d= 1/rate)
    strngst = frequencies[np.argmax(fft_result)]

    if not prv_freq is None and abs(strngst - prv_freq) <= tao:
        curr_time += time_frame
    else:
        if prv_freq is not None:
            freqs.append(prv_freq)
            times.append(curr_time)
        prv_freq = strngst
        curr_time = time_frame

if not prv_freq is None:
    freqs.append(prv_freq)
    times.append(curr_time)

n = 300
print("freqs")
for i in range(n):
    print(int(freqs[i]), end=', ')

print()
print("times")
for i in range(n):
    print(int(times[i]*100/time_frame), end=', ')
