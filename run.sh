#!/bin/bash

g++ butterfly_csv.cpp -o bdcsv -lm -O3
./bdcsv
./csv2png.py *.csv -s 0 -m 4000
gcc phasing.c -o cphasing -O3 -lm
#echo starting 500
./cphasing 500.0 200
./hilbertize.py out.csv -o f500 --freq_min 1200 --freq_max 1800

