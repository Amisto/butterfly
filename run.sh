#!/bin/bash

g++ butterfly_csv.cpp -o bdcsv -lm -O3
./bdcsv
./csv2png.py *.csv 
./hilbertize.py out.csv -o og --freq_min 250 --freq_max 550
