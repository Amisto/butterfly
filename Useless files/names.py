import os #Script to change the names of files
import re
for i in range(10, 32):
    os.rename('data/baseline/Sensor{}/baseline.txt_shift_0.00_0{}.csv'.format(i, i),
            'data/baseline/Sensor{}/raw{}.csv'.format(i, i))
