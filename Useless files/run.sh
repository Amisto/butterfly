#!/bin/bash

g++ butterfly_csv.cpp -o bdcsv -lm -O3
time ./bdcsv fin_0.8.txt
./hilbertize.py phased_fin_0.8.txt.csv -o fin_0.8_3 --freq_min 150 --freq_max 550
time ./bdcsv fin_1.0.txt
./hilbertize.py phased_fin_1.0.txt.csv -o fin_1.0 --freq_min 150 --freq_max 550
g++ butterfly_csv.cpp -o bdcsv -lm -O3
time ./bdcsv fin_0.8.txt
./hilbertize.py phased_fin_0.8.txt.csv -o fin_0.8_5 --freq_min 150 --freq_max 550
