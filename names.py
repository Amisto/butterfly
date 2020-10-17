import os #Script to change the names of files
import re
for i in range(1, 10):
    os.remove('data/baseline/Sensor{}/baseline.txt_shift_0.00_00{}.csv_spectrum_.png'.format(i, i))
for i in range(10, 32):
    os.remove('data/baseline/Sensor{}/baseline.txt_shift_0.00_0{}.csv_spectrum_.png'.format(i, i))