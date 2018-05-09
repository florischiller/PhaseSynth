# An Octave script calculating one periode of 4 waveforms to create a LUT for each waveform
# It calculates the LUTs of the waveforms sine, sawtooth, square and triangle
# It requires the packages control and signal

pkg load all;

resolution  = power(2, 10);

pointLength = (2*pi) / resolution;

period      = (0 : pointLength : 2*pi - pointLength);

sinus_w     = sin(period);
sawtooth_w  = sawtooth(period);
square_w    = square(period);
triangle_w  = sawtooth(period, 0.5);

csvwrite("sinus_wave.csv",    sinus_w',    "precision", "%f");
csvwrite("sawtooth_wave.csv", sawtooth_w', "precision", "%f");
csvwrite("square_wave.csv",   square_w',   "precision", "%f");
csvwrite("triangle_wave.csv", triangle_w', "precision", "%f");
