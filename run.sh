#!/bin/bash

g++ butterfly_csv.cpp -o bdcsv -lm
./bdcsv
./csv2png.py *.csv -s 0 -m 800
gcc phasing.c -o cphasing -O3 -lm
./cphasing
./hilbertize.py out.csv -o out_h_f500 --freq_min 150 --freq_max 350

