#!/usr/bin/python3
from hilbertize_functions import final  # function to create final files
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

parser.add_argument(
    '-o',
    '--output',
    required=True,
    metavar='OUTPUT',
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
    a = (-v0 + 3.0*v2 - 3.0*v2 + v3)/6.0
    c = -0.5*v0 + 0.5*v1 - a
    return a*factor*factor*factor + b*factor*factor + c*factor + d

args = parser.parse_args()

for one_file in args.files:
    with open(one_file) as ff:
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
        plt.savefig(args.output + "_spectrum_" + ".png")
        #results -> 255 grayscale
        min_res_fft = min(map(min, res_fft))
        max_res_fft = max(map(max, res_fft))
        res_fft_color = [[int(255*(x-min_res_fft)/(max_res_fft-min_res_fft)) for x in l] for l in res_fft]
        max_res = max(map(max, vals))
        min_res = min(map(min, vals))
        res_color = [[int(255*(x-min_res)/(max_res-min_res)) for x in l] for l in vals]

        #printin'
        #with open(args.output+".png", 'wb') as f:
        #    w = png.Writer(len(res_color[0]), len(res_color), greyscale=True)
        #    w.write(f, res_color)
        #with open(args.output + "_" + str(args.freq_min) + "_" + str(args.freq_max) +"_fft.png", 'wb') as f:
        #    w = png.Writer(len(res_fft_color[0]), len(res_fft_color), greyscale=True)
        #    w.write(f, res_fft_color)

        data = res_fft_color
        h = len(data)*1.0
        w = len(data[0])*1.0

        angle_rad = 90*math.pi/180

        r = h/angle_rad/2.0
        R = r + w
        q = r*math.cos(angle_rad/2.0)

        nh = math.ceil(R - q)
        nw = math.ceil(2*R*math.sin(angle_rad/2))

        new_data = [[0 for x in range(nw+1)] for y in range(nh+1)]

        cx = nw/2.0
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

                #rayw = 4
                #rayC = math.floor(a)
                #rayB = rayC - math.floor(rayw/2)
                #rayT = rayC + math.floor(rayw/2) + 1

                #if all([0 <= d1 < w, 0 <= d2 < w, 0 < rayB, rayT < h]):
                #    rmax = 0
                #    ri = 6
                #    for ray in range(rayw):
                #        rv = interpolate(data[rayB + ray][d1], data[rayB + ray][d2], math.modf(d)[0])
                #        if (rv > rmax):
                #            rmax = rv
                #            ri = ray
                #    if (ri == math.floor(rayw/2)):
                #        new_data[i][j] = rmax*rmax
                #    else:
                #        new_data[i][j] = 0
                #    rCv = interpolate(data[rayC][d1], data[rayC][d2], math.modf(d)[0])
                #    new_data[i][j] += rCv*rCv

                ray1 = math.floor(a)
                ray2 = math.ceil(a)
                ray0 = ray1 - 1
                ray3 = ray2 + 1

                if all([0 <= d1 < w, 0 <= d2 < w, 0 <= ray1 < h, 0 <= ray2 < h, 0 <= ray0 < h, 0 <= ray3 < h]):
                    r1v = interpolate(data[ray1][d1], data[ray1][d2], math.modf(d)[0])
                    r2v = interpolate(data[ray2][d1], data[ray2][d2], math.modf(d)[0])
                    r0v = interpolate(data[ray0][d1], data[ray0][d2], math.modf(d)[0])
                    r3v = interpolate(data[ray3][d1], data[ray3][d2], math.modf(d)[0]) 
                #    v = abs(interpolate3(r0v, r1v, r2v, r3v, math.modf(a)[0]))
                    v = interpolate(r1v, r2v, math.modf(a)[0])
                    #x = v*v
                    #z = math.log2(1 + v)
                    z = math.sqrt(v)
                    #v = interpolate(r1v, r2v, math.modf(a)[0])
                    new_data[i][j] = z
                else:
                    new_data[i][j] = -1
        max_data = max(map(max, new_data))

        for i in range(nh):
            for j in range(nw):
                if (new_data[i][j] == -1):
                    new_data[i][j] = max_data

        min_data = min(map(min, new_data))
        data_color = [[int(255*(x-min_data)/(max_data-min_data)) for x in l] for l in new_data]
        with open(one_file+"_"+args.output+"_sectorized.png", 'wb') as fff:
            w = png.Writer(nw+1, nh+1, greyscale=True)
            w.write(fff, data_color)
 
