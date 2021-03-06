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

parser.add_argument(
    '-o',
    '--output',
    required=True,
    metavar='OUTPUT',
    help='Output file name'
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
        data = [[abs(float(x)) for x in l.split()] for l in ff.readlines()]

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
                    #z = math.log2(1 + x)
                    z = v*v*v#math.sqrt(v)
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
 
