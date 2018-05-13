#!/usr/bin/python3

import argparse
import os
import re
import png


parser = argparse.ArgumentParser(description='Converts sensor data from CSV to PNG')

parser.add_argument(
    'files',
    metavar='FILES',
    nargs='+',
    help='List of CSV files with sensor data'
)

#parser.add_argument(
#    '-m',
#    '--max-length',
#    default=9999,
#    type=int,
#    metavar='LEN',
#    help='Max length of sensor data'
#)

#parser.add_argument(
#    '-s',
#    '--skip',
#    default=80,
#    type=int,
#    metavar='SKIP',
#    help='Number of points to skip'
#)


args = parser.parse_args()

for fname in args.files:
    with open(fname) as f:
        values = ([[abs(float(x)) for x in l.split()] for l in f.readlines()])#[args.skip:args.max_length]])

        max_v = max(map(max, values))

        values_color = [[int(255*x/max_v) for x in l] for l in values]


        with open(fname + '.png', 'wb') as ff:
            w = png.Writer(len(values_color[0]), len(values_color), greyscale=True)
            w.write(ff, values_color)

