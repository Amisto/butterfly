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
    #nargs='+',
    help='List of CSV files with solo sensor data'
)

parser.add_argument(
    '-o',
    '--output',
    required=True,
    metavar='FILE',
    help='Output file name'
)

parser.add_argument(
    '--freq_min',
    required=True,
    type=int,
    metavar='FREQ_MIN',
    help='Min range of the frequency window'
)

parser.add_argument(
    '--freq_max',
    required=True,
    type=int,
    metavar='FREQ_MAX',
    help='Max range of the frequency window'

)

def interpolate(v1, v2, factor):
    return v1*(1-factor) + v2*factor

args = parser.parse_args()

with open(args.files) as ff:
    vals = [[abs(float(x)) for x in l.split()] for l in ff.readlines()]

    #Narrowband filtering + Hilbert transformation
    res_fft = []
    for i,row in enumerate(vals):
        print("starting " + str(i) + " of " + str(len(vals)))
        fft = np.fft.rfft(row)

        #this is to check the spectrum and find the carrying freauency
        #plt.plot(np.abs(fft)[10:]) #near-zero freqencies somehow are exremely large
        #plt.savefig(args.output + "_fft_" + str(i) + ".png")
        #plt.clf()

        for j,freq in enumerate(fft):
            if not (args.freq_min <= j <= args.freq_max):
                fft[j] = 0
        res_fft.append(np.abs(hilbert(np.fft.irfft(fft))))
    #plt.savefig(args.output + "_spectrum_" + ".png")
    #results -> 255 grayscale
    min_res_fft = min(map(min, res_fft))
    max_res_fft = max(map(max, res_fft))
    res_fft_color = [[int(255*(x-min_res_fft)/(max_res_fft-min_res_fft)) for x in l] for l in res_fft]
    max_res = max(map(max, vals))
    min_res = min(map(min, vals))
    res_color = [[int(255*(x-min_res)/(max_res-min_res)) for x in l] for l in vals]

    #printin'
    with open(args.output+".png", 'wb') as f:
        w = png.Writer(len(res_color[0]), len(res_color), greyscale=True)
        w.write(f, res_color)
    with open(args.output + "_" + str(args.freq_min) + "_" + str(args.freq_max) +"_fft.png", 'wb') as f:
        w = png.Writer(len(res_fft_color[0]), len(res_fft_color), greyscale=True)
        w.write(f, res_fft_color)


    data = res_fft_color
    h = len(data)
    w = len(data[0])

    angle_rad = 120*math.pi/180

    r = h/angle_rad
    R = r + w
    q = r*math.cos(angle_rad/2)

    nh = math.ceil(R - q)
    nw = math.ceil(2*R*math.sin(angle_rad/2))

    new_data = [[0 for x in range(nw+1)] for y in range(nh+1)]

    cx = nw/2
    cy = q

    for i in range(nh):
        for j in range(nw):
            x = j + 0.5
            y = i + 0.5
            dx = x - cx
            dy = cy + y

            d = (dx**2 + dy**2)**0.5 - r

            d1 = math.floor(d)
            d2 = math.ceil(d)

            a = (0.5 + math.atan2(dx, dy)/angle_rad)*h

            ray1 = math.floor(a)
            ray2 = math.ceil(a)

            if all([0 <= d1 < w, 0 <= d2 < w, 0 <= ray1 < h, 0 <= ray2 < h]):
                r1v = interpolate(data[ray1][d1], data[ray1][d2], math.modf(d)[0])
                r2v = interpolate(data[ray2][d1], data[ray2][d2], math.modf(d)[0])
                v = interpolate(r1v, r2v, math.modf(a)[0])
                new_data[i][j] = v
            else:
                new_data[i][j] = 0
    max_data = max(map(max, new_data))
    min_data = min(map(min, new_data))
    data_color = [[int(255*(x-min_data)/(max_data-min_data)) for x in l] for l in new_data]
    with open(args.output+"_sectorized.png", 'wb') as fff:
        w = png.Writer(nw+1, nh+1, greyscale=True)
        w.write(fff, data_color)

