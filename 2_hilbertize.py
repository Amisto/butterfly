#!/usr/bin/python3
from hilbertize_functions import final, dir  # function to create final files
import matplotlib.pyplot as plt  # directory for saving
import argparse
import os
import re
import png
import math
import csv
import numpy as np
from scipy.signal import hilbert
import matplotlib
matplotlib.use('Agg')

# Creating parser for powershell
parser = argparse.ArgumentParser(description='Hilbertizing sensor data')

parser.add_argument(
    '--window',
    required=True,
    type=int,
    metavar='WINDOW',
    help='Width of the frequency window'
)

args = parser.parse_args()
cnt = 0
cnt_dirs = 0
file_names = []
for i in range(0, 32):
    file_name = os.listdir('data/baseline/Sensor{}/'.format(i))
    file_names.append(file_name)
raw_data=[]
tpl = r'^baseline.txt_shift_0.00_0\d+.csv$'
for dr in file_names:
    for i in dr:
        if re.match(tpl, i):
            raw_data.append(i)
        else:
            continue
print(raw_data)



for one_file in raw_data:
    # for saving files by dirs
    with open(dir.format(cnt) + '/'+ one_file) as fi:
        with open(dir.format(cnt)+one_file+"2_hilb.csv", 'w') as fo:
            vals_init = [[float(x) for x in l.split()] for l in fi.readlines()]
            vals = list(zip(*vals_init))
            # Narrowband filtering + Hilbert transformation
            res_fft = []
            maxf = 0
            for i, row in enumerate(vals):
                fft = np.fft.rfft(row)
                # this is to check the spectrum and find the carrying freauency
                # near-zero freqencies somehow are exremely large
                plt.plot(np.abs(fft)[10:])
                maxf += np.abs(fft)[10:].argmax()
                #plt.savefig(args.output + "_fft_" + str(i) + ".png")
                # plt.clf()

            mid = maxf/len(vals)

            for i, row in enumerate(vals):
                fft = np.fft.rfft(row)
                for j, freq in enumerate(fft):
                    # if not (args.freq_min <= j <= args.freq_max):
                    if not (mid - args.window/2.0 <= j <= mid + args.window/2.0):
                        fft[j] = 0
                res_fft.append(np.abs(hilbert(np.fft.irfft(fft))))

            print(mid)
            # saving spectrum
            plt.savefig(dir.format(cnt) + one_file + "2_spectrum_" + ".png")
            plt.cla()
            res = list(zip(*res_fft))
            max_data = max(map(max, res))
            min_data = min(map(min, res))
            for r in res:
                stri = ""
                for t in r:
                    stri += str(round((t - min_data)/(max_data-min_data), 2))
                    stri += " "
                print(stri, file=fo)

            with open(dir.format(cnt) + one_file+"2_HIHIHILB.csv", 'w') as fu:
                for r in vals_init:
                    stri = ""
                    for t in r:
                        stri += str(round(t, 2))
                        stri += " "
                    print(stri, file=fu)

            final(file=dir.format(cnt) +
                  one_file, val=vals_init, pref="2_prehilb.png")
            # writing final graphics
            final(file=dir.format(cnt) +
                  one_file, val=res, pref="2_hilb.png")
    cnt += 1  # plussing to copy next files in another folder
