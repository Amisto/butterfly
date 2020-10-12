#!/usr/bin/python3
from hilbertize_functions import final #function to create final files
import matplotlib.pyplot as plt
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

parser.add_argument(  # creates a list of csv files
    'files',
    metavar='FILES',
    nargs='+',
    help='List of CSV files with solo sensor data'
)

parser.add_argument(
    '--window',
    required=True,
    type=int,
    metavar='WINDOW',
    help='Width of the frequency window'
)
args = parser.parse_args()  # 2 arguments at all



for one_file in args.files:
    with open(one_file) as fi:
        with open(one_file+"_hilb.csv", 'w') as fo:  # TODO! FIX THE ROOTS OF SAVING FILES
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
            plt.savefig(one_file + "_spectrum_" + ".png")
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

            with open(one_file+"_HIHIHILB.csv", 'w') as fu:
                for r in vals_init:
                    stri = ""
                    for t in r:
                        stri += str(round(t, 2))
                        stri += " "
                    print(stri, file=fu)

            # TODO! FIX THE ROOTS OF SAVING FILES AND OPENINGS
            final(file=one_file, val=vals_init, pref="_prehilb.png")
            # writing final graphics
            final(file=one_file, val=res, pref="_hilb.png")
