import os  # Script to change the names of files
import os  # Script to change the names of files
import re
for i in range(10, 32):
    os.rename('data/baseline/Sensor{}/baseline.txt_shift_0.00_0{}.csv'.format(i, i),
            'data/baseline/Sensor{}/raw{}.csv'.format(i, i))

for i in range(0, 10):
    os.rename('data/baseline/Sensor{}/baseline.txt_shift_0.00_00{}.csv'.format(i, i),
            'data/baseline/Sensor{}/raw{}.csv'.format(i, i))
