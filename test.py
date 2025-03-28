from scipy.io import wavfile

def freq(file, start_time, end_time):
    sample_rate, data = wavfile.read(file)
    start_point = int(sample_rate * start_time / 1000)
    end_point = int(sample_rate * end_time / 1000)
    length = (end_time - start_time) / 1000
    counter = 0
    for i in range(start_point, end_point):
        if data[i][0] < 0 and data[i+1][0] > 0:
            counter += 1
    return counter/length  

file = open("./musica.txt", "w+")

for i in range(0, 100000, 500):
    file.write(str(freq("Megalovania - Undertale (8 Bit Universe Version).wav", i, i+500))+ '\n')

file.close()