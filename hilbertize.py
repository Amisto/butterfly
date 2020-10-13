#!/usr/bin/python3
from hilbertize_functions import final, dir  # function to create final files
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
def interpolate3(v0, v1, v2, v3, factor):
    #d = v0
    #c = -12.33*v0 + 15.0*v2 - 3.0*v2 + 0.33*v2
    #b = -1.5*c - 3.5*v0 + 4.0*v1 - 0.5*v2
    #a = v1 - v0 - b - c
    d = v1
    b = 0.5*v0 - v1 + 0.5*v2
    a = (-v0 + 3.0*v1 - 3.0*v2 + v3)/6.0
    c = -0.5*v0 + 0.5*v1 - a
    return a*factor*factor*factor + b*factor*factor + c*factor + d

args = parser.parse_args()
cnt = 0
with open(args.files) as ff:
    vals = [[abs(float(x)) for x in l.split()] for l in ff.readlines()]

    #Narrowband filtering + Hilbert transformation
    res_fft = []
    for i,row in enumerate(vals):
        #print("starting " + str(i) + " of " + str(len(vals)))
        fft = np.fft.rfft(row)

        #this is to check the spectrum and find the carrying freauency
        plt.plot(np.abs(fft)[10:]) #near-zero freqencies somehow are exremely large
        #plt.savefig(args.output + "_fft_" + str(i) + ".png")
        #plt.clf()

        for j,freq in enumerate(fft):
            if not (args.freq_min <= j <= args.freq_max):
                fft[j] = 0
        res_fft.append(np.abs(hilbert(np.fft.irfft(fft))))
    plt.savefig(dir.format(cnt)+args.output + "_spectrum_" + ".png")
    #results -> 255 grayscale
    final(file=dir.format(cnt) + args.output, val=res_fft, pref=".png")
    #Put two else parts into the function
    final(file=dir.format(cnt) + args.output + "_" + str(args.freq_min) +
        "_" + str(args.freq_max), val=vals, pref="_fft.png")

    
    min_res_fft = min(map(min, res_fft))
    max_res_fft = max(map(max, res_fft))
    res_fft_color = [[int(255*(x-min_res_fft)/(max_res_fft-min_res_fft))
    for x in l] for l in res_fft]
    data = res_fft_color
    h = len(data)*1.0
    w = len(data[0])*1.0

    angle_rad = 90.0*math.pi/180.0

    r = h/angle_rad/2.0
    R = r + w
    q = r*math.cos(angle_rad/2.0)

    nh = math.ceil(R - q)
    nw = math.ceil(2*R*math.sin(angle_rad/2))

    new_data = [[0 for x in range(nw+1)] for y in range(nh+1)]

    cx = nw/2.0
    cy = q
    maxv = 0
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

            rayw = 4
            rayC = math.floor(a)
            rayB = rayC - math.floor(rayw/2)
            rayT = rayC + math.floor(rayw/2) + 1
            #ray2 = math.ceil(a)

            if all([0 <= d1 < w, 0 <= d2 < w, 0 < rayB, rayT < h]):
                rmax = 0
                ri = 6
                for ray in range(rayw):
                    rv = interpolate(data[rayB + ray][d1], data[rayB + ray][d2], math.modf(d)[0])
                    if (rv > rmax):
                        rmax = rv
                        ri = ray
                if (ri == math.floor(rayw/2)):
                    new_data[i][j] = rmax*rmax
                else:
                    new_data[i][j] = 0   
                rCv = interpolate(data[rayC][d1], data[rayC][d2], math.modf(d)[0])
                new_data[i][j] += rCv*rCv

            #    r1v = interpolate(data[ray1][d1], data[ray1][d2], math.modf(d)[0])
            #    r2v = interpolate(data[ray2][d1], data[ray2][d2], math.modf(d)[0])
            #    r0v = interpolate(data[ray0][d1], data[ray0][d2], math.modf(d)[0])
            #    r3v = interpolate(data[ray3][d1], data[ray3][d2], math.modf(d)[0])
                #v = (interpolate3(r0v, r1v, r2v, r3v, math.modf(a)[0]))
                #v = interpolate(r1v, r2v, math.modf(a)[0])
                #v = max([r0v, r1v, r2v, r3v])
            #    if (maxv < r0v):
            #        maxv = r0v
            #    new_data[i][j] = r0v
            else:
                new_data[i][j] = 0
    print(maxv)

    final(file=dir.format(cnt) + args.output,
        val=new_data, pref="_sectorized.png")


