#!/usr/bin/python3

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
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description='Hilbertizing sensor data')

parser.add_argument(
    'files',
    metavar='FILES',
    nargs='+',
    help='List of CSV files with solo sensor data'
)

#parser.add_argument(
#    '--freq_min',
#    required=True,
#    type=int,
#    metavar='FREQ_MIN',
#    help='Min range of the frequency window'
#)
#
#parser.add_argument(
#    '--freq_max',
#    required=True,
#    type=int,
#    metavar='FREQ_MAX',
#    help='Max range of the frequency window'

#)

parser.add_argument(
    '--window',
    required=True,
    type=int,
    metavar='WINDOW',
    help='Width of the frequency window'
)

args = parser.parse_args()

for one_file in args.files:
    with open(one_file) as fi:
        with open(one_file+"_hilb.csv", 'w') as fo:
            vals_init = [[float(x) for x in l.split()] for l in fi.readlines()]
            vals = list(zip(*vals_init))
            #Narrowband filtering + Hilbert transformation
            res_fft = []
            maxf = 0
            for i,row in enumerate(vals):
                fft = np.fft.rfft(row)

                #this is to check the spectrum and find the carrying freauency
                plt.plot(np.abs(fft)[10:]) #near-zero freqencies somehow are exremely large
                maxf += np.abs(fft)[10:].argmax()
                
                #plt.savefig(args.output + "_fft_" + str(i) + ".png")
                #plt.clf()
            mid = maxf/len(vals)
            for i,row in enumerate(vals):
                fft = np.fft.rfft(row)
                for j,freq in enumerate(fft):
                    #if not (args.freq_min <= j <= args.freq_max):
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
                    stri +=" "
                print(stri, file=fo)

            with open(one_file+"_HIHIHILB.csv", 'w') as fu:
                for r in vals_init:
                    stri = ""
                    for t in r:
                        stri += str(round(t, 2))
                        stri +=" "
                    print(stri, file=fu)


            max_datap = max(map(max, vals_init))
            min_datap = min(map(min, vals_init))
            datap_color = [[int(255*(x-min_datap)/(max_datap-min_datap)) for x in l] for l in vals_init]
            with open(one_file+"_prehilb.png", 'wb') as fpng:
                w = png.Writer(len(vals_init[0]), len(vals_init), greyscale=True)
                w.write(fpng, datap_color)

            max_data = max(map(max, res))
            min_data = min(map(min, res))
            print(max_data, min_data)
            data_color = [[int(255*(x-min_data)/(max_data-min_data)) for x in l] for l in res]
            with open(one_file+"_hilb.png", 'wb') as fpng:
                w = png.Writer(len(res[0]), len(res), greyscale=True)
                w.write(fpng, data_color)

            #for j in range(len(values[0])):
                #print(values[0][j][0], values[0][j][1], values[0][j][2], values[0][j][3], values[0][j][4], values[0][j][5], values[0][j][6], values[0][j][7], values[0][j][8], file=f)
        
