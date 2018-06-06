#!/bin/bash

g++ butterfly_csv.cpp -o bdcsv -lm -O3
time ./bdcsv p0.9.txt
./hilbertize.py phased_out.csv -o lin_0.0 --freq_min 150 --freq_max 550
