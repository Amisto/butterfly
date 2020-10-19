#!/usr/bin/python3

import argparse
import os
import re
import png
import math
import csv
import numpy as np
from scipy.signal import hilbert
import matplotlib.pyplot as plt
import matplotlib

matplotlib.use('Agg')

# filenames
input_data = "data/final_raw_data.csv"
output_data = "data/result.png"

def interpolate(v1, v2, factor):
    return v1 * (1 - factor) + v2 * factor


def interpolate3(v0, v1, v2, v3, factor):
    d = v1
    b = 0.5 * v0 - v1 + 0.5 * v2
    a = (-v0 + 3.0 * v2 - 3.0 * v2 + v3) / 6.0
    c = -0.5 * v0 + 0.5 * v1 - a
    return a * factor * factor * factor + b * factor * factor + c * factor + d


with open(input_data) as ff:
    data = [[abs(float(x)) for x in line.split()] for line in ff.readlines()]

    h = float(len(data))
    w = float(len(data[0]))

    angle_rad = math.pi / 2

    r = h / angle_rad / 2.0
    R = r + w
    q = r * math.cos(angle_rad / 2.0)

    nh = math.ceil(R - q)
    nw = math.ceil(2 * R * math.sin(angle_rad / 2))

    new_data = [[0 for x in range(nw + 1)] for y in range(nh + 1)]

    cx = nw / 2.0
    cy = q

    for i in range(nh):
        for j in range(nw):
            x = j + 0.5
            y = i + 0.5
            dx = x - cx
            dy = cy + y

            d = (dx ** 2 + dy ** 2) ** 0.5 - r

            d1 = math.floor(d)
            d2 = math.ceil(d)

            a = (0.5 + math.atan2(dx, dy) / angle_rad) * h

            ray1 = math.floor(a)
            ray2 = math.ceil(a)
            ray0 = ray1 - 1
            ray3 = ray2 + 1

            if all([0 <= d1 < w, 0 <= d2 < w, 0 <= ray1 < h, 0 <= ray2 < h, 0 <= ray0 < h, 0 <= ray3 < h]):
                r1v = interpolate(data[ray1][d1], data[ray1][d2], math.modf(d)[0])
                r2v = interpolate(data[ray2][d1], data[ray2][d2], math.modf(d)[0])
                r0v = interpolate(data[ray0][d1], data[ray0][d2], math.modf(d)[0])
                r3v = interpolate(data[ray3][d1], data[ray3][d2], math.modf(d)[0])
                v = interpolate(r1v, r2v, math.modf(a)[0])
                z = v * v * v
                new_data[i][j] = z
            else:
                new_data[i][j] = -1
    max_data = max(map(max, new_data))

    for i in range(nh):
        for j in range(nw):
            if (new_data[i][j] == -1):
                new_data[i][j] = max_data

    min_data = min(map(min, new_data))
    data_color = [[int(255 * (x - min_data) / (max_data - min_data)) for x in l] for l in new_data]
    with open(output_data, 'wb') as fff:
        w = png.Writer(nw + 1, nh + 1, greyscale=True)
        w.write(fff, data_color)
